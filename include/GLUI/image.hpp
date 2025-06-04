#ifndef IMAGE_H
#define IMAGE_H

#include "GLUI/glui.hpp"
#include "GLUI/layout.hpp"
#include "GLUI/rect.hpp"
#include "GLUI/text.hpp"
#include <functional>
#include <unordered_set>
#include <cstdio>
#include <iostream>
#include <algorithm>

#include <glm/ext.hpp>
#include <glm/gtx/string_cast.hpp>
#include<GLUI/img_loader.hpp>



class Image : public Element {
    
    private:
        ImageHandler* il = nullptr;

    public: 
        
        Image( const std::string path, Rect r ) : Element( r, TRANSPARENT ) {
            il = new ImageHandler( path );
            if ( il->is_valid_image_path((std::string*)&path) ) {
                std::cout<<"Loading " << path << std::endl;
                il->load();
            }
        }
    
        Image( ImageHandler * il, Rect r ): Element( r, WHITE ), il(il) {
            
            Text *t = new Text( "Selecione uma imagem" );
            t->set_font_size(16);
            t->set_foreground_color(RGBA(.12, .17, .24));

            child(t);
        }

        void draw(Size* window) override {
            auto tr = get_true_rect(); 

            if ( il->is_loaded() ) {

                // tr->width  = il->get_size()->width;
                // tr->height = il->get_size()->height;

                // this is a very dumb way of achieving this
                if ( il->is_framebuffer() ) {
                    fbo_to_screen( &il->m_fbo, tr, il, window );
                } else {
                    draw_tex_quad(tr, il, window);
                }

                return;
            } 

            // Children are the default "Image not loaded yet..."
            draw_quad(tr, RGBA(.1, .1, .1), window);
            draw_children(tr, window);
        }    
        // Text string stored in the input
};


#endif 