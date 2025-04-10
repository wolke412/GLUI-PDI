#ifndef BEHAVIOUR_H
#define BEHAVIOUR_H

// I don't like this, but i cant waste any more time with these dump includes
#include "GLFW/glfw3.h"
#include <GLUI/layout.hpp>
#include <functional>

// std::unordered_map<Element*, Hoverable*> hoverable_registry;

class Focusable;
class Hoverable;
class Toggable;

typedef std::function<void(Focusable&)> focus_callback_t;
typedef std::function<void(Hoverable&)> hover_callback_t;
typedef std::function<void(Toggable&)>  toggle_callback_t;

class Focusable : public virtual Element {

  protected:
    bool m_focus = false;

    focus_callback_t cb_focus;
    focus_callback_t cb_blur;

  public:
    Focusable()
    {
      capabilities |= CFocusable;
      // hoverable_registry[this] = this; // Store reference
    }

    void onfocus( focus_callback_t f ) {
      cb_focus = f;
    }

    void onblur ( focus_callback_t f ) {
      cb_blur = f;
    }

    virtual void focus()
    {
      m_focus = true;
      
      if (cb_focus)  {
        cb_focus(*this);
      }
    }

    virtual void blur()
    {
      m_focus = false;

      if (cb_blur)
      {
        cb_blur(*this);
      }
    }

    /**
     * this is callbacks to the GLFW; not to GLUI...
     */
    virtual void cb_char_event(  unsigned int pressed, GLFWwindow *window ) {
      std::cout << "CHAR EVENT" << std::endl;
    }

    virtual void cb_key_press( GLFWwindow* w, int key, int scanode, int action, int mods ) {
      std::cout << "KEY::PRESS EVENT" << std::endl;
    }
};

class Clickable : public virtual Element {
  public:
    using click_callback_t = std::function<void(Clickable&)>; 
    public:
        click_callback_t cb_onclick;

        Clickable( ) {
          capabilities |= CClickable  ;
        }

        virtual void onclick( click_callback_t cb ) {
          cb_onclick = cb;
        }

        virtual void click()  {
            // std::cout << "CLICK EVENT ON :: " << id << std::endl;
            if (cb_onclick)
            {
                cb_onclick( *this );
            }
        }
};


class Toggable : public virtual Element {

  protected:
    bool m_active= false;

    toggle_callback_t cb_toggle;
    toggle_callback_t cb_activated;
    toggle_callback_t cb_deactivated;

  public:
    Toggable()
    {
      capabilities |= CToggable;
      // hoverable_registry[this] = this; // Store reference
    }

    bool is_active () { return m_active; }

    void ontoggle( toggle_callback_t f ) {
      cb_toggle = f;
    }

    void onactivated( toggle_callback_t f ) {
      cb_activated = f;
    }

    void ondeactivated ( toggle_callback_t f ) {
      cb_deactivated = f;
    }

    void _handle_callbacks ( ) {

      if (cb_toggle)  {
        cb_toggle(*this);
      }

      if ( m_active == true && cb_activated )  {
        cb_activated(*this);
      }

      if (  m_active == true && cb_deactivated )  {
        cb_deactivated(*this);
      }
    }

    virtual void toggle () {
      m_active = ! m_active;
      _handle_callbacks();
    }

    virtual void toggle ( bool s )
    {
      m_active = s;
      _handle_callbacks();
    }

    /**
     * this is callbacks to the GLFW; not to GLUI...
     */
    virtual void cb_char_event(  unsigned int pressed, GLFWwindow *window ) {
      std::cout << "CHAR EVENT" << std::endl;
    }

    virtual void cb_key_press( GLFWwindow* w, int key, int scanode, int action, int mods ) {
      std::cout << "KEY::PRESS EVENT" << std::endl;
    }
};



class Hoverable : public virtual Element {

  protected:

    bool m_hover = false;

    hover_callback_t cb_enter;
    hover_callback_t cb_leave;

    RGBA hover_background_color = BLACK;
    RGBA hover_foreground_color = BLACK;


  public:
    Hoverable()
    {
      capabilities |= CHoverable;
      // hoverable_registry[this] = this; // Store reference
    }

    void set_hover_background_color (RGBA c) {
      hover_background_color = c;
    }

    void set_hover_foreground_color (RGBA c) {
      hover_foreground_color = c;
    }
        

    void draw( Size *w ) override {


      // std::cout << "HOVER::DRAWING" << std::endl;

      if ( ! m_hover ) {
        Element::draw(w);
        return;
      }
      
      
      RGBA temp = Element::get_background_color();

      set_background_color( hover_background_color );
      set_foreground_color( hover_foreground_color );

      Element::draw(w);

      Element::set_background_color( temp );
      
    }

    void onmouse_enter( hover_callback_t f ) {
      cb_enter = f;
    }

    void onmouse_leave ( hover_callback_t f ) {
      cb_leave = f;
    }

    virtual void mouse_enter()
    {
      m_hover = true;
      
      if (cb_enter)  {
        cb_enter(*this);
      }
    }

    virtual void mouse_leave()
    {
      m_hover = false;

      if (cb_leave)
      {
        cb_leave(*this);
      }
    }

    /**
     * this is callbacks to the GLFW; not to GLUI...
     */
    virtual void cb_char_event(  unsigned int pressed, GLFWwindow *window ) {
      std::cout << "CHAR EVENT" << std::endl;
    }

    virtual void cb_key_press( GLFWwindow* w, int key, int scanode, int action, int mods ) {
      std::cout << "KEY::PRESS EVENT" << std::endl;
    }
};

#endif