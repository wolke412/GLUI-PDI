#ifndef GLUI_DRAW_H
#define GLUI_DRAW_H

#include <GLUI/rect.hpp>
#include <GLUI/img_loader.hpp>


void printVector(float* vec, int totalSize, int rowSize);



void initialize_drawing();
void load_shaders();

void normalize(Rect * rect, Size*window, float* vertices);
void normalize_rect(Rect * rect, Size *window );
float normalize_to_range( float v, float min, float max );
void apply_tex(float* vertices, float* nvertices);


/**
 * 
 */
void draw_quad(Rect rect, RGBA c, Size*window);
void draw_quad(Rect *rect, RGBA c, Size*window);

// void draw_circle(Circle *circle, RGBA c, Size*window);
void draw_circle(Rect *rect, RGBA c, Size*window);
/**
 * 
 */
void draw_tex_quad( Rect *r, ImageHandler *img, Size* window);

/**
 * 
 */
void draw_rounded_quad( Rect *r, RGBA c, glm::vec4 corners, glm::vec4 widths, Size* window);


/**
 *  ============================================================
 *      FBO SHIT :
 *      ----------------------------------------
 *      eh difisio mexe em fbo :( 
 *  ============================================================
 */
void set_buffers( GLuint *VAO, GLuint *VBO );
void compute_tex_quad( GLShitFBO* g, glm::mat3 kernel, ImageHandler *img, Size* win );
void fbo_to_screen( GLShitFBO *g, Rect *r, ImageHandler *img, Size* win );



#endif