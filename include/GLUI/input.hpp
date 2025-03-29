#ifndef INPUTS_H
#define INPUTS_H

#include "GLUI/glui.hpp"
#include "GLUI/layout.hpp"
#include "GLUI/draw.hpp"
#include "GLUI/rect.hpp"
#include "GLUI/text.hpp"
#include <functional>
#include <unordered_set>
#include <cstdio>
#include <iostream>

#include <algorithm>


bool is_valid_tex_input(unsigned int codepoint);

/**
 * TODO: consider an implementation of
 * "ChildlessElement"
 */
class Input :  public Focusable {
    
    private:
    
        std::unique_ptr<Text> text_;

        using text_input_callback_t = std::function<void(Input&)>;
    
        std::string m_text = ""; 
        std::string m_placeholder = "..."; 

        size_t cursor_pos = 0;      // Cursor position in the text

        // Callback when text changes
        text_input_callback_t cb_input_change;                               

    public: 
        
        Input( __RECT_TYPE__ fixed_width ): Element( Size(fixed_width, LAYOUT_FIT_CONTENT), BLACK) {

            set_padding(Padding(10, 20));
           
            auto t = std::make_unique<Text>(Text(m_text));

            t.get()->set_font_size(DEFAULT_FONT_SIZE);

            child(t.get());  

            text_ = std::move(t);

            // rect.height = DEFAULT_FONT_SIZE + padding.top + padding.bottom;
        }

        void set_value( std::string s ) {
            m_text = s;
        }

        std::string* get_value() {
            return &m_text;
        }

        void onchange( text_input_callback_t c ) {
            cb_input_change = c;
        }
        
        void changed() {
            if ( cb_input_change ) {
                cb_input_change( *this );
            }
        }
       
        void cb_key_press( GLFWwindow* w, int key, int scanode, int action, int mods ) override {
            
            if ( action != GLFW_RELEASE ) {
                if ( key == GLFW_KEY_BACKSPACE ) {
                    // std::cout << " FOCUSABLE::CALLBACK::KEY::PRESS  ::DELETE_BACK" << std::endl;
                    if ( cursor_pos == 0 ) return;
                    m_text.erase( cursor_pos-- - 1, 1);
                }  

                if ( key == GLFW_KEY_DELETE) {
                    // std::cout << " FOCUSABLE::CALLBACK::KEY::PRESS  ::DELETE_FRONT" << std::endl;
                    if ( cursor_pos == m_text.size() ) return;
                    m_text.erase( cursor_pos, 1);
                }
                
                if ( key == GLFW_KEY_RIGHT ) {
                    cursor_pos = std::clamp( cursor_pos+1, (size_t)0, m_text.size() );
                }

                if ( key == GLFW_KEY_LEFT) {
                    cursor_pos = std::clamp( cursor_pos-1, (size_t)0, m_text.size()  );
                }
            }

            changed();
        }

        void cb_char_event( unsigned int key, GLFWwindow* w) override {
            
            if ( is_valid_tex_input( key ) ) {
                std::cout << " FOCUSABLE::CALLBACK::KEY::PRESS  ::VALID" << std::endl;
                m_text.insert(cursor_pos, 1, key);
                cursor_pos++;
                changed();
            } else {
                std::cout << " FOCUSABLE::CALLBACK::KEY::PRESS  ::INVALID" << std::endl;
            }


        }

        void draw(Size* window) override {

            auto tr = get_true_rect();
            // Display the text and cursor (you may want to render it differently)
            std::string display_text = m_text;

            if ( m_focus ) {

                // Draw focused border 
                draw_quad( tr->to_scaled( 2 ), WHITE, window);

                // Cursor at the current position (or an underscore)
                display_text.insert(cursor_pos, "_");
            }

            draw_quad(get_true_rect(), bg_color, window);

            auto t = text_.get();

            if ( ! m_text.empty() ) {
                // draw content... no need to overhead with the for loop... this is always single child
                t->set_text(display_text);
                t->set_foreground_color(WHITE);
                t->draw(window);

                return;
            }

            t->set_text( m_placeholder );
            t->set_foreground_color(RGB(.6, .6, .6));
            t->draw(window);
        } 
        // Text string stored in the input
};


#endif 