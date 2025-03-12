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


#include<GLUI/img_loader.hpp>



class Image : public Element {
    
    private:
        ImageHandler* il = nullptr;

    public: 
        
        Image( ImageHandler * il, Rect r ): Element( r, WHITE ), il(il) {
            
            Text *t = new Text( "Selecione uma imagem" );
            t->set_font_size(16);
            t->set_foreground_color(RGB(.12, .17, .24));

            child(t);
        }

        void draw(Size* window) override {
            auto tr = get_true_rect(); 

            if ( il->is_loaded() ) {

                tr->width = il->get_size()->width;
                tr->height = il->get_size()->height;

                draw_tex_quad( tr, il, window );

                return;
            } 

            draw_quad(tr, RGB(.1, .1, .1), window);
            draw_children(tr, window);
        }    
        // Text string stored in the input
};


#endif 