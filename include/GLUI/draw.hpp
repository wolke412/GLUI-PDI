#ifndef GLUI_DRAW_H
#define GLUI_DRAW_H

#include <GLUI/rect.hpp>
#include <GLUI/img_loader.hpp>

void initialize_drawing();

GLShitFBO init_fbo(  Rect *r, Size* window, ImageHandler *i );

void read_fbo( uint8_t *nout, Size* sz, GLShitFBO* g );

void calc_mtx( ImageHandler *img, glm::mat3x3 mat, GLShitFBO *g, uint8_t *nout, Rect *r, Size* win );


void normalize(Rect * rect, Size*window, float* vertices);
void normalize_rect(Rect * rect, Size *window );
float normalize_to_range( float v, float min, float max );
void apply_tex(float* vertices, float* nvertices);


/**
 * 
 */
void draw_quad(Rect rect, RGB c, Size*window);
void draw_quad(Rect *rect, RGB c, Size*window);

/**
 * 
 */
void draw_tex_quad( Rect *r, ImageHandler *img, Size* window);
void draw_compute_tex_quad( GLShitFBO* g, glm::mat3 kernel, Rect *r, ImageHandler *img, Size* window);


/**
 * 
 */
void draw_rounded_quad( Rect *r, RGB c, glm::vec4 corners, Size* window);



#endif