#ifndef MULTIPASS_FBO_H
#define MULTIPASS_FBO_H

#include <glad/glad.h>
#include <vector>
#include <functional>
#include <iostream>

class MultiPassFBO {
public:
    MultiPassFBO(int width, int height);
    ~MultiPassFBO();

    void process(GLuint inputTexture, const std::vector<std::function<void(GLuint)>> &passes, GLuint outputFBO = 0);
    
    inline GLuint getFinalTexture() const { return ping_pong_tex[readIndex]; }

private:
    int width, height;
    GLuint ping_pong_fbos[2], ping_pong_tex[2];
    int readIndex = 0, writeIndex = 1;

    void swap_buffers();
    void setup_fbos();
};

// Constructor
MultiPassFBO::MultiPassFBO(int width, int height) : width(width), height(height) {
    setup_fbos();
}

// Destructor
MultiPassFBO::~MultiPassFBO() {
    glDeleteFramebuffers(2, ping_pong_fbos);
    glDeleteTextures(2, ping_pong_tex);
}

// Setup FBOs
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
            std::cerr << "ERROR::MultiPassFBO::Framebuffer is not complete!" << std::endl;
        }
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// Swap read/write indices
void MultiPassFBO::swap_buffers() {
    std::swap(readIndex, writeIndex);
}

// Process passes
void MultiPassFBO::process(GLuint inputTexture, const std::vector<std::function<void(GLuint)>> &passes, GLuint outputFBO) {
    readIndex = 0;
    writeIndex = 1;

    // First pass: use input texture
    glBindFramebuffer(GL_FRAMEBUFFER, ping_pong_fbos[writeIndex]);
    glViewport(0, 0, width, height);
    passes[0](inputTexture);
    swap_buffers();

    // Process additional shader passes
    for (size_t i = 1; i < passes.size(); ++i) {
        glBindFramebuffer(GL_FRAMEBUFFER, ping_pong_fbos[writeIndex]);
        passes[i](ping_pong_tex[readIndex]);
        swap_buffers();
    }

    // Final pass: render to output FBO if provided
    if (outputFBO != 0) {
        glBindFramebuffer(GL_FRAMEBUFFER, outputFBO);
        passes.back()(ping_pong_tex[readIndex]);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0); // Unbind FBO
}

#endif // MULTIPASS_FBO_H
