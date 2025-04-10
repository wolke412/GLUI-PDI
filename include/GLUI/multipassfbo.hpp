#ifndef MULTIPASS_FBO_H
#define MULTIPASS_FBO_H

#include <glad/glad.h>
#include <GLUI/framebuffer.hpp>
#include <GLUI/rect.hpp>
#include <GLUI/draw.hpp>

#include <vector>
#include <functional>
#include <iostream>

// just for the ass to stop hurting
class MultiPassFBO;

typedef std::function<void(GLuint, GLShit*, MultiPassFBO*)>  FnShader;


class MultiPassFBO {
public:
    MultiPassFBO(int width, int height);
    ~MultiPassFBO();

    void process(GLuint inputTexture, const std::vector<FnShader> &passes, Size* win, GLuint outputFBO );
    void setup();

    void get_framebuffer_quad( float* buffer );
    void apply( GLuint tex);

    inline GLuint getFinalTexture() const { return ping_pong_tex[readIndex]; }

    /**
     *  ------------------------------------------------------------
     */
    int width, height;

private:
    GLShit glshit;

    GLuint ping_pong_fbos[2], ping_pong_tex[2];
    int readIndex = 0, writeIndex = 1;

    void swap_buffers();
    void setup_fbos();
};


#endif 
