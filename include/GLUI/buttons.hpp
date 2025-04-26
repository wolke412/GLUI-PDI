#ifndef BUTTONS_H  
#define BUTTONS_H

#include <functional>
#include <glad/glad.h>
#include <memory>
#include <shader.hpp>
#include <GLUI/colors.hpp>
#include <GLUI/rect.hpp>
#include <GLUI/text.hpp>
#include <GLUI/layout.hpp>
#include <GLUI/behaviour.hpp>

class Button;
typedef std::function<void(Button&)> click_callback_t;

class Button: public Focusable, public Hoverable {
    private:
        std::unique_ptr<Text> text_;


    public:
        click_callback_t cb_onclick;

        Button(const std::string text, RGBA c): 
            Element(Size{FitContent, FitContent}, c )
        {

            capabilities |= CClickable;            

            auto t = std::make_unique<Text>(text);  
            t->set_font_size( font_size );
            child(t.get());                         

            text_ = std::move(t);                   
        }

        Text* get_text() {
            return dynamic_cast<Text*>(children[0]);
        }  

        virtual void set_font_size( uint8_t fs ) override  {
            text_->set_font_size( fs );
        }

        virtual void set_foreground_color (RGBA c) override  {
            text_->set_foreground_color( c );
        }

        void onclick( click_callback_t cb ) {
            cb_onclick = cb;
        }

        void click()  {
            // std::cout << "CLICK EVENT ON :: " << id << std::endl;
            if (cb_onclick)
            {
                cb_onclick( *this );
            }
        }

};

class ToggleButton: public Button {

};

#endif
