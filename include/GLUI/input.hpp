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

template <typename T = std::string> 
class Input {
    
// Callback when text changes
using input_callback_t = std::function<void(Input<T> &)>;

public:
    Input() = default;

    T m_value; 
    T m_default;

    input_callback_t cb_input_change;

    virtual void set_value( T s ) { m_value = s; }
    T *get_value() { return &m_value; }

    void onchange( input_callback_t c ) { cb_input_change = c; }

    void changed()
    {
        if (cb_input_change)
        {
            cb_input_change(*this);
        }
    }
};


/**
 * TODO: consider an implementation of
 * "ChildlessElement"
 */
class TextInput : public Input<std::string>, public Focusable {
private:

    std::unique_ptr<Text> text_;

    std::string m_placeholder = "...";

    size_t cursor_pos = 0; // Cursor position in the text

    // border focus
    RGBA focus_border_color = RGBA(.5, .55, .6);

public:
    TextInput(__RECT_TYPE__ fixed_width) : Element(Size(fixed_width, LAYOUT_FIT_CONTENT), BLACK)
    {
        set_padding(Padding(10, 20));

        auto t = std::make_unique<Text>(Text(m_value));

        t.get()->set_font_size(DEFAULT_FONT_SIZE);

        child(t.get());

        text_ = std::move(t);
    }

    void cb_key_press(GLFWwindow *w, int key, int scanode, int action, int mods) override
    {

        if (action != GLFW_RELEASE)
        {
            if (key == GLFW_KEY_BACKSPACE)
            {
                if (cursor_pos == 0) return;
                m_value.erase(cursor_pos-- - 1, 1);
            }

            if (key == GLFW_KEY_DELETE)
            {
                if (cursor_pos == m_value.size()) return;
                m_value.erase(cursor_pos, 1);
            }

            if (key == GLFW_KEY_RIGHT)
            {
                cursor_pos = std::clamp(cursor_pos + 1, (size_t)0, m_value.size());
            }

            if (key == GLFW_KEY_LEFT)
            {
                cursor_pos = std::clamp(cursor_pos - 1, (size_t)0, m_value.size());
            }
        }

        changed();
    }

    void cb_char_event(unsigned int key, GLFWwindow *w) override
    {
        if (is_valid_tex_input(key))
        {
            // std::cout << " FOCUSABLE::CALLBACK::KEY::PRESS  ::VALID" << std::endl;
            m_value.insert(cursor_pos, 1, key);
            cursor_pos++;
            changed();
        }
        else
        {
            // std::cout << " FOCUSABLE::CALLBACK::KEY::PRESS  ::INVALID" << std::endl;
        }
    }

    void draw(Size *window) override
    {

        auto tr = get_true_rect();

        // Display the text and cursor (you may want to render it differently)
        std::string display_text = m_value;

        draw_quad(get_true_rect(), bg_color, window);

        if (m_focus)
        {
            // Cursor at the current position
            display_text.insert(cursor_pos, "|");

            if (get_border()->exists())
            {
                auto original_border_color = get_border()->get_color();
                get_border()->set_color(WHITE);

                draw_border(window);
                get_border()->set_color(original_border_color);
            }
        }
        else
        {
            draw_border(window);
        }

        auto t = text_.get();

        if (!m_value.empty())
        {
            // draw content... no need to overhead with the for loop... this is always single child
            t->set_text(display_text);
            t->set_foreground_color(WHITE);
            t->draw(window);
            return;
        }

        t->set_text(m_placeholder);
        t->set_foreground_color(RGBA(.6, .65, .7));
        t->draw(window);
    }
    // Text string stored in the input
};


/**
 * Checkbox
 */
class Checkbox : public Input<bool>, public Toggable, public Clickable, public Focusable {
private:

    // border focus
    RGBA focus_border_color  = WHITE; 
    RGBA active_bg_color     = DARKGREY;
    RGBA active_border_color = WHITE; 

public:
    Checkbox(__RECT_TYPE__ fixed_width) : Element(Size(fixed_width), BLACK)
    {
        // set_background_color(TRANSPARENT);
        set_border(Border( 1, LIGHTGREY ));

        ontoggle( [this]( Toggable& t ) {
            set_value( t.is_active() );
            changed();
            std::cout << "Toggled CHECKBOX" << std::endl;
        } );

        onclick( [this](Clickable& c){
            toggle();
        } );
    }

    void cb_key_press(GLFWwindow *w, int key, int scanode, int action, int mods) override
    {
        if (action != GLFW_RELEASE)
        {
            if ( key == GLFW_KEY_SPACE || key == GLFW_KEY_ENTER )
            {
                toggle();
            }
        }

    }

    void cb_char_event(unsigned int key, GLFWwindow *w) override { /**/ }

    void draw(Size *window) override
    {

        // Element::draw(window);
        // return;

        auto tr = get_true_rect();

        auto background = bg_color;
        auto border     = get_border()->get_color();

        if ( m_value ) {
           background = active_bg_color;
           border     = active_border_color;
        }

        draw_quad( get_true_rect(), background, window );

        if ( m_value ) {
            // auto little_circle = get_true_rect()->to_scaled(-6);
            // draw_circle( &little_circle, active_border_color, window );
        }

        if ( m_focus )
        {
            border = focus_border_color;
        }

        if (get_border()->exists())
        {
            auto original_border_color = get_border()->get_color();
            get_border()->set_color(border);

            draw_border(window);
            get_border()->set_color(original_border_color);
        }

    }
    // Text string stored in the input
};

#endif