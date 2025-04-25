#ifndef COLORS_H
#define COLORS_H

#include <glad/glad.h>
#include <shader.hpp>
#include <stdint.h>

#define __RGB_TYPE__ float;
#define __RGB_FRAGMENT_MAX__ 1.0f;

#define WHITE               RGBA(1,1,1,1)
#define LIGHTGREY           RGBA(.75,.75,.75, 1)
#define DARKGREY            RGBA(.25,.25,.25, 1)
#define BLACK               RGBA(0,0,0,1)
#define TRANSPARENT         RGBA(0,0,0,0)

constexpr float lerp(float a, float b, float t) {
    return a + (b - a) * t;
}

struct RGB{
    float R;
    float G;
    float B;

    constexpr RGB( float r, float g, float b ): R(r), G(g), B(b) {}
};

typedef struct RGBA {
    float R;
    float G;
    float B;
    float A;

    constexpr RGBA( float r, float g, float b, float a): R(r), G(g), B(b), A(a) {}
    constexpr RGBA( float r, float g, float b ): R(r), G(g), B(b), A(1.0) {}
    constexpr RGBA( RGB rgb, float a ): R(rgb.R), G(rgb.G), B(rgb.B), A(a) {}

    constexpr operator glm::vec4() {
        return glm::vec4(R,G,B,A);
    }

    RGB to_rgb() const {
        return RGB(R, G, B);
    }

    void opacity( float a ) {
        A = a;
    }

    void lerp( RGBA* to, RGBA* current, float t ) {
        current->R = ::lerp(R, to->R, t);
        current->G = ::lerp(G, to->G, t);
        current->B = ::lerp(B, to->B, t);
        current->A = ::lerp(A, to->A, t);
    }

} RGBA;


constexpr RGBA Opacity( const RGBA c, const float a ) {
    return RGBA{ c.R, c.G, c.B, a };
}

#endif
