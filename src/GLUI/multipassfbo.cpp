#include "GLUI/core.hpp"
#include <GLUI/multipassfbo.hpp>
#include <string>

MultiPassFBO::MultiPassFBO(int width, int height) : width(width), height(height) {
    setup_fbos();
    setup();
}

MultiPassFBO::~MultiPassFBO() {
    glDeleteFramebuffers(2, ping_pong_fbos);
    glDeleteTextures(2, ping_pong_tex);
}

void MultiPassFBO::setup_fbos() {
    glGenFramebuffers(2, ping_pong_fbos);
    glGenTextures(2, ping_pong_tex);

    for (int i = 0; i < 2; ++i) {
        glBindTexture(GL_TEXTURE_2D, ping_pong_tex[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glBindFramebuffer(GL_FRAMEBUFFER, ping_pong_fbos[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ping_pong_tex[i], 0);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            std::cerr << "ERROR::MultiPassFBO::Framebuffer incomplete!" << std::endl;
        }
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// self descriptive
void MultiPassFBO::swap_buffers() {
    std::swap(readIndex, writeIndex);
}

// process passes
void MultiPassFBO::process(GLuint inputTexture, const std::vector<FnShader> &passes, Size* win, GLuint outputFBO ) {
    std::vector<float> times(passes.size());
    process(inputTexture, passes, times, win, outputFBO);
}
void MultiPassFBO::process(GLuint inputTexture, const std::vector<FnShader> &passes, std::vector<float> times, Size* win, GLuint outputFBO ) {

    readIndex = 0;
    writeIndex = 1;

    glViewport(0, 0, width, height);
    
    Logger::Log( 
        Color::Cyan( "Initializing Multipass FBO with " + std::to_string(passes.size()) + " passes" ) 
    );
    // Logger::Log("----------------------------------------");
    Logger::Log( "  +" );
    Logger::Log( "  |" );
    
    auto tex = inputTexture;
    for (size_t i = 0; i < passes.size(); ++i) {
        glBindFramebuffer(GL_FRAMEBUFFER, ping_pong_fbos[writeIndex]);
        
        GLuint query;
        glGenQueries(1, &query);
        glBeginQuery(GL_TIME_ELAPSED, query);

        passes[i](tex, &glshit, this);

        glEndQuery(GL_TIME_ELAPSED);

        GLuint time;
        glGetQueryObjectuiv(query, GL_QUERY_RESULT, &time);
        glDeleteQueries(1, &query);

        auto timestr = std::string(std::to_string(time/ 1e6)) + " ms";

        Logger::Log( "  +----+ Stage: \t\t"   + std::to_string(i));
        Logger::Log( "  |    | Texture: \t\t" + std::to_string(tex) );
        Logger::Log( "  |    | Took: \t\t"    + Color::Green( timestr ) );

        times.push_back( time / 1000000.0 );

        swap_buffers();

        tex = ping_pong_tex[readIndex];

        Logger::Log( "  |     ");
    }

    glBindFramebuffer(GL_READ_FRAMEBUFFER, ping_pong_fbos[readIndex]);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, outputFBO);
    glBlitFramebuffer(
        0, 0, width, height, // source
        0, 0, width, height, // destination
        GL_COLOR_BUFFER_BIT, GL_NEAREST);

    glBindFramebuffer(GL_FRAMEBUFFER, 0); // Unbind FBO
    glViewport(0, 0, win->width, win->height);
}

void MultiPassFBO::setup(  ) {
    GLShit g;

    Rect r( 0, 0, width, height );
    auto s = r.get_size();

    set_fbo_buffers( &g.VAO, &g.VBO );

    glshit = g;
}

void MultiPassFBO::get_framebuffer_quad( float* buffer ) {

    float jvertices[8]; 

    Rect r( 0, 0, width, height );
    auto s = r.get_size();

    // Normalizes vertexes from window space to GL space
    normalize(&r, s, jvertices);

    apply_tex(jvertices, buffer);
}

void MultiPassFBO::apply( GLuint tex) {

    glClear(GL_COLOR_BUFFER_BIT);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex);

    float vertices[16];

    get_framebuffer_quad( vertices );

    glBindVertexArray(glshit.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, glshit.VBO);

    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}
