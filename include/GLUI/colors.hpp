#ifndef COLORS_H
#define COLORS_H

#include <glad/glad.h>
#include <shader.hpp>
#include <stdint.h>

#define __RGB_TYPE__ float;
#define __RGB_FRAGMENT_MAX__ 1.0f;

#define WHITE RGB(1,1,1)
#define BLACK RGB(0,0,0)

struct RGB{
    float R;
    float G;
    float B;

    RGB( float r, float g, float b ): R(r), G(g), B(b) {}
};

typedef struct RGBA {
    float R;
    float G;
    float B;
    float A;

    RGBA( float r, float g, float b, float a): R(r), G(g), B(b), A(a) {}
    RGBA( RGB rgb, float a ): R(rgb.R), G(rgb.G), B(rgb.B), A(a) {}
} RGBA;


#endif
