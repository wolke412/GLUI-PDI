#ifndef STYLESHEETS_H
#define STYLESHEETS_H

#include <GLUI/colors.hpp>
#include <GLUI/layout.hpp>


template <typename T, int TIME_MS = 0>
struct Property {
    T value;
    int time = TIME_MS;
    float progress = 0.0;

    Property( T v ):value(v){}
    Property( T v, int time ): value(v), time(time){}

    operator T() const {
        return value;
    }

    /**
     * @param delta_time: Delta time int miliseconds
     * Receives elapsed time since last frame and updates 
     */
    void transition( float delta_time ) {

        if ( time == 0 ) { 
            progress = 1.0;
            return;
        }

        if ( progress > 1.0 ) {
            progress += delta_time / (float)time;
        }
    }
    void reset_transition() {
        progress = 0.0;
    }

    void set_transition_time ( int time_ms ) {
        time = time_ms;
    }
};

struct Stylesheet {
    Property<RGBA,    0>   background_color = BLACK;
    Property<RGBA,    0>   foreground_color = WHITE;
    Property<Border,  0>   border = Border(0, TRANSPARENT);
    Property<Padding, 0>   padding = Padding(0);
};

struct HoverableStylesheet {};
struct ClickableStylesheet {};
struct FocusableStylesheet {};

struct DraggableStylesheet {
    Property<RGBA, 150>   background_color;
    Property<RGBA, 150>   foreground_color;
};

void TEST_apply_stylesheet( Element* to, Stylesheet* s );

#endif