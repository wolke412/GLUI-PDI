#ifndef GLUI_DRAW_H
#define GLUI_DRAW_H

#include <GLUI/rect.hpp>
#include <GLUI/img_loader.hpp>

void initialize_drawing();

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

/**
 * 
 */
void draw_rounded_quad( Rect *r, RGB c, glm::vec4 corners, Size* window);


/**
 *  ============================================================
 *      FBO SHIT :
 *      ----------------------------------------
 *      eh difisio mexe em fbo :( 
 *  ============================================================
 */
void set_buffers( GLuint *VAO, GLuint *VBO );
void compute_tex_quad( GLShitFBO* g, glm::mat3 kernel, ImageHandler *img );
void fbo_to_screen( GLShitFBO *g, Rect *r, ImageHandler *img, Size* win );



#endif