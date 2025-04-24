#ifndef BEHAVIOUR_H
#define BEHAVIOUR_H

// I don't like this, but i cant waste any more time with these dump includes
#include "GLFW/glfw3.h"
#include <GLUI/layout.hpp>
#include <GLUI/collision.hpp>
#include <functional>
#include <algorithm>

// std::unordered_map<Element*, Hoverable*> hoverable_registry;

class Focusable;
class Hoverable;
class Toggable;
class Draggable;

typedef std::function<void(Focusable&)> focus_callback_t;
typedef std::function<void(Hoverable&)> hover_callback_t;
typedef std::function<void(Toggable&)>  toggle_callback_t;

typedef std::function<void(Draggable&)> drag_callback_t;

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
        click_callback_t cb_onmousedown;
        click_callback_t cb_onmouseup;

        Clickable( ) {
          capabilities |= CClickable  ;
        }

        virtual void onclick( click_callback_t cb ) {
          cb_onclick = cb;
        }
        virtual void onmousedown( click_callback_t cb ) {
          cb_onmousedown = cb;
        }
        virtual void onmouseup( click_callback_t cb ) {
          cb_onmouseup= cb;
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

enum DraggableBoundary {
  Parent
};

class Draggable : public Hoverable, public virtual Element {

  bool dragging    = false;
  Coord drag_start_at     = Coord(0,0);
  Coord original_position = Coord(0,0);

  drag_callback_t cb_drag_start;
  drag_callback_t cb_drag_move;
  drag_callback_t cb_drag_end;

  DraggableBoundary m_bound_type = DraggableBoundary::Parent;

public:
  Draggable() {
    capabilities |= ElementCapabilities::CDraggable;
  }

  bool is_dragging (  ) const { return dragging; }

  void set_boundary_type( DraggableBoundary t ) {
    m_bound_type = t;
  }

  /**
   * TODO: maybe implement somekind of "Ghost element" do render while dragging,
   * instead of truly altering element's position
   */
  void start_drag( Coord mouse ) {
    dragging = true;

    // convert to parent-local space
    drag_start_at = m_parent->to_local( mouse );

    // position in local space
    original_position = rect.get_pos();

    if ( cb_drag_start ) cb_drag_start(*this);

    // std::cout << "stat at: " << mouse.x << ", " << mouse. y << std::endl;
  }

  void move_drag( Coord mouse ) {
    if ( cb_drag_move ) cb_drag_move(*this);

    // global to parent-local
    Coord local_mouse = m_parent->to_local( mouse );

    Coord delta   = local_mouse.difference(&drag_start_at);
    Coord target  = original_position.add(&delta);
    Coord clamped = target;

    switch (m_bound_type) {

      case Parent: {

        const Rect& content = m_parent->get_content_rect( ); // local space
        const Rect* parent  = m_parent->get_true_rect( );    // local space

        auto size = *rect.get_size();

        clamped.x = std::clamp<int16_t>(target.x, content.x, content.x + content.width  - size.width);
        clamped.y = std::clamp<int16_t>(target.y, content.y, content.y + content.height - size.height);

        // content.debug("CONTENT:: ");
        // size.debug("SIZE:: ");
        // parent->debug("PARENT:: ");

        break;
      }

      default: break;
    }

    set_position(clamped);

    // std::cout << "Drag: global_mouse=(" << mouse.x << ", " << mouse.y << ") ";
    // std::cout << "Drag: local_mouse=(" << local_mouse.x << ", " << local_mouse.y << ") ";
    // std::cout << "Target=(" << target.x << ", " << target.y << ") ";
    // std::cout << "Clamped=(" << clamped.x << ", " << clamped.y << ")\n";
  }

  void end_drag( Coord mouse) {
    dragging = false;
    if ( cb_drag_end ) cb_drag_end(*this);
  }

  void ondragstart ( drag_callback_t c ) { cb_drag_start = c; }
  void ondragmove  ( drag_callback_t c ) { cb_drag_move  = c; }
  void ondragend   ( drag_callback_t c ) { cb_drag_end   = c; }

  void draw( Size *w ) override {
    Element::draw(w);
    // if ( ! dragging ) {
    //   Element::draw(w);
    //   return;
    // }

    // RGBA temp = Element::get_background_color();

    // set_background_color( hover_background_color );
    // set_foreground_color( hover_foreground_color );

    // Element::draw(w);

    // Element::set_background_color( temp );
  }
};

#endif