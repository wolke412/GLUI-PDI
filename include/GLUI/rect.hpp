#ifndef RECT_H  
#define RECT_H

#include <glad/glad.h>
#include <shader.hpp>

#include <stdint.h>
#include <GLUI/colors.hpp>

#define __RECT_TYPE__ int16_t 

struct Coord {
    __RECT_TYPE__ x; 
    __RECT_TYPE__ y;

    Coord(__RECT_TYPE__ x, __RECT_TYPE__ y) : x(x), y(y) {}
    Coord(__RECT_TYPE__ x ) : x(x), y(x) {}


    float distance( Coord *other ) {
        float dx = x - other->x;
        float dy = y - other->y;

        return std::sqrt(dx * dx + dy * dy);
    }

    Coord difference( Coord *other ) {
        return Coord( x - other->x, y - other->y );
    }

    Coord add( Coord *other ) {
        return Coord( x + other->x, y + other->y );
    }
};

struct Size {
    __RECT_TYPE__ width; 
    __RECT_TYPE__ height;

    Size(__RECT_TYPE__ width, __RECT_TYPE__ height) : width(width), height(height) {}
    Size(__RECT_TYPE__ sql ) : width(sql), height(sql) {}

    void debug(const char* label = nullptr) const {
        if (label) {
            std::cout << label << " ";
        }
        std::cout << "(w=" << width << ", h=" << height << ")\n";
    }
};

struct Rect {
    __RECT_TYPE__ x; 
    __RECT_TYPE__ y; 
    __RECT_TYPE__ width; 
    __RECT_TYPE__ height;

    Rect(__RECT_TYPE__ x, __RECT_TYPE__ y, __RECT_TYPE__ width, __RECT_TYPE__ height)
        : x(x), y(y), width(width), height(height) {}
    
    Rect( Coord c, Size s ): x(c.x), y(c.y), width(s.width), height(s.height) {}
    Rect( Size s): x(0), y(0), width(s.width), height(s.height) {}

    Rect(): x(0), y(0), width(0), height(0) {}

    Rect to_scaled( __RECT_TYPE__ offset ) {

        auto new_rect = *this;

        new_rect.x -= offset;
        new_rect.y -= offset;

        new_rect.width  += offset * 2;
        new_rect.height += offset * 2;

        return new_rect;
    }

    Size *get_size() {
        static Size sz( width, height );
        sz.width = width;
        sz.height = height;
        return &sz;
    }

    Coord get_pos() {
        Coord c( x, y );
        return c;
    }

    void set_x( __RECT_TYPE__ f ) { x = f; };
    void set_y( __RECT_TYPE__ f ) { y = f; };

    void debug(const char* label = nullptr) const {
        if (label)
            std::cout << label << " ";
        std::cout << "(x=" << x << ", y=" << y 
                  << ", w=" << width << ", h=" << height << ")\n";
    }
}; 

/**
 * 
 */
struct Circle {
    __RECT_TYPE__ x; 
    __RECT_TYPE__ y;
    
    __RECT_TYPE__ radius;
};

#endif
