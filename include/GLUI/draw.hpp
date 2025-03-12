#include <GLUI/rect.hpp>
#include <GLUI/img_loader.hpp>

void initialize_drawing();


void normalize(Rect * rect, Size*window, float* vertices);

void draw_quad(Rect rect, RGB c, Size*window);
void draw_quad(Rect *rect, RGB c, Size*window);
void draw_tex_quad( Rect *r, ImageHandler *img, Size* window);
