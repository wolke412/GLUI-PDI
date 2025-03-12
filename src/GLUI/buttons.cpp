#include <GLUI/buttons.hpp>

// Draw a simple button
void draw_button (std::string text, Rect r, RGB c, Size* window) {


    draw_quad(r, c, window);

    Coord textat(r.x, r.y);

    RenderText(text, textat, 1.0f, WHITE , window);
}
