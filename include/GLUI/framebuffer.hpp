#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <glad/glad.h>
#include <GLUI/rect.hpp>


struct GLShit {
    unsigned int VBO;
    unsigned int VAO;
};

struct GLShitFBO {
    GLuint VBO;
    GLuint VAO;
    GLuint FBO = 0;
    GLuint texture;
    GLuint RBO;

    void read( uint8_t* nout, Size* sz, int channel_count) {
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);
        glReadBuffer(GL_COLOR_ATTACHMENT0);

        auto a = channel_count == 3 ? GL_RGB : GL_RGBA;
        glReadPixels(0, 0, sz->width, sz->height, a, GL_UNSIGNED_BYTE, nout);

        glBindFramebuffer(GL_FRAMEBUFFER, 0); // Unbind the framebuffer
    };

    void init() {

    }
};

GLShitFBO init_fbo( Size *r );
void free_fbo( GLShitFBO *fbo );

void set_fbo_buffers( GLuint *VAO, GLuint *VBO );

void init_uv_vao_vbo( GLShit *gl );

#endif