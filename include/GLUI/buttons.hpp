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

void draw_button (std::string text, Rect r, RGB c, Size* window);

class Button;
typedef std::function<void(Button&)> click_callback_t;

class Button: public Focusable, public Hoverable {
    private:
        std::unique_ptr<Text> text_;

        uint8_t font_size = DEFAULT_FONT_SIZE;

        // RGB fg_color       = WHITE;

    public:
        click_callback_t cb_onclick;

        Button(const std::string text, RGB c): Element(Size(LAYOUT_FIT_CONTENT, LAYOUT_FIT_CONTENT), c )
        {
            capabilities |= CClickable;            

            auto t = std::make_unique<Text>(text);  
            child(t.get());                         

            text_ = std::move(t);                   
        }

        /**
         * Remember kids: 
         * Buttons children must be Text and Img 
         * OR
         * A direct Element if a *FIXED SIZE*;
         */

        Text* get_text() {
            return dynamic_cast<Text*>(children[0]);
        }  

        void onclick( click_callback_t cb ) {
            cb_onclick = cb;
        }

        void click()  {
            std::cout << "CLICK EVENT ON :: " << id << std::endl;

            if (cb_onclick)
            {
                cb_onclick( *this );
            }
        }

};

#endif
