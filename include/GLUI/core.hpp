#ifndef GLUI_CORE_H
#define GLUI_CORE_H

#include <glad/glad.h>
#include <GL/glext.h>
#include <GL/gl.h>

#include <GLFW/glfw3.h>

#include <cstdio>
#include <iostream>

#include <algorithm>

#include <ostream>
#include <sys/types.h>

// --

#include <shader.hpp>
#include <GLUI/behaviour.hpp>
#include <GLUI/rect.hpp>
#include <GLUI/collision.hpp>
#include <GLUI/buttons.hpp>
#include <GLUI/layout.hpp>


#include <GLUI/hotkeys.hpp>

/**
 * FOR DEBUGGING ONLY
 */
#include <GLUI/benchmark.hpp>

#define MOUSE_LEFT   ( 0 )
#define MOUSE_RIGHT  ( 1 )
#define MOUSE_MIDDLE ( 2 )

#define MOUSE_DISTANCE_CLICK_THRESHOLD (10)

struct Mouse {

  Coord at;
  Coord downed_at = Coord(0);

  int buttons[3] = { 0 ,0 ,0 };

  Mouse(Coord c): at(c){};

  void set_button( int b, int v ) {
    buttons[b] = v;
  }

  void downed() {
    downed_at.x = at.x;
    downed_at.y = at.y;
  }

  int get_left_button() {
    return buttons[MOUSE_LEFT];
  }

  Coord* get_downed_at () {
    return &downed_at;
  }

  float get_distance_to_downed() {
    return at.distance(get_downed_at());
  }

};

class GLUI {
public:
  Size m_window_size      = Size(800, 600);
  Size m_last_window_size = Size(800, 600);

private:

  GLFWwindow *window;

  Mouse mouse;
  Hotkeys m_hotkeys;


  RGB m_clear_color = BLACK;

  Element *shadow_root;
  Element *root;

  std::vector<Element *> fixed;

  Focusable *focused;

  // For now just one will suffice
  std::vector<Element *> m_hover_path;

  static GLUI *singleton;

  // static std::map<GLFWwindow*, GLUI*> m_geese;

  GLUI(const char *title) : mouse(Mouse(Coord(0, 0)))
  {

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    window = glfwCreateWindow(800, 600, title, NULL, NULL);
    // GLUI::m_geese[window] = this;
  };

public:
  static GLUI &create(const char *title)
  {
    if (singleton == nullptr)
    {
      singleton = new GLUI(title);
    }

    return *singleton;
  }

  static GLUI *instance()
  {

    if (!singleton)
    {
      throw std::runtime_error("GLUI::create() must be called before GLUI::instance()");
    }

    return singleton;
  }

  // static GLUI* find(GLFWwindow* win) {
  //   if (m_geese.find(win) == m_geese.end()) {
  //     throw std::runtime_error("GLUI instance not found for the provided window.");
  //   }

  //   return m_geese[win];
  // }

  // static void destroy(GLFWwindow* win) {
  //   if (m_geese.find(win) != m_geese.end()) {
  //     delete m_geese[win];
  //     m_geese.erase(win);
  //   }
  // }

  // Delete copy constructor and assignment operator to enforce singleton property
  GLUI(const GLUI &) = delete;
  GLUI &operator=(const GLUI &) = delete;

  bool begin( RGB clear_color )
  {

    if (window == NULL)
    {
      std::cout << "Failed to create GLFW window" << std::endl;
      glfwTerminate();
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(0);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
      std::cout << "Failed to initialize GLAD" << std::endl;
    }

    m_clear_color = m_clear_color;

    glViewport(0, 0, 800, 600);

    glfwSetFramebufferSizeCallback(window, cb_framebuffer_resize);
    glfwSetKeyCallback(window, cb_key_press_event);
    glfwSetCharCallback(window, cb_char_event);
    glfwSetMouseButtonCallback(window, cb_mouse_event);

    glfwSetCursorPosCallback(window, cb_mouse_move_event);

    /**
     * Initializes drawing lib
     */
    initialize_drawing();

    /**
     * Initializes text rendering shit
     */
    DEBUG_init_text();

    return true;
  }

  bool should_close() {
    return glfwWindowShouldClose(window);
  }

  void loop_start()
  {
    clear();
    process_input();

    /**
     * dumb shit :
     */
    int win_w, win_h, fb_w, fb_h;
    glfwGetWindowSize(window, &win_w, &win_h);
    glfwGetFramebufferSize(window, &fb_w, &fb_h);

    auto s = get_window_size();

    if (win_w != fb_w || win_h != fb_h)
    {
        std::cout << "Size mismatch! Window: " << win_w << "x" << win_h << std::endl
                  << ", My window: " << s.width << "x" << s.height << std::endl
                  << ", Framebuffer: " << fb_w << "x" << fb_h << std::endl;
    }
  }

  void loop_end()
  {

    // send commands
    // glFinish(); 
    // glFlush();
    glfwSwapBuffers(window);
    glfwPollEvents();
  }


  void process_input()
  {
    double x, y;

    glfwGetCursorPos(window, &x, &y);

    auto sz = get_window_size();

    mouse.at.x = std::clamp((int16_t)x, (int16_t)0, sz.width);
    mouse.at.y = std::clamp((int16_t)y, (int16_t)0, sz.height);

    // 
    // Benchmark::mark();
    check_hovering(  );
    // std::cout << Benchmark::since_mark(Unit::Micro) << " us" << std::endl;
  }

  void kill () {
    glfwTerminate();
  }

  void clear() {
    auto c = &m_clear_color;

    glClearColor( c->R, c->G, c->B, 1.0 );
    glClear(GL_COLOR_BUFFER_BIT);
  }

  Mouse *get_mouse()
  {
    return &mouse;
  }

  Hotkeys *get_hotkeys()
  {
    return &m_hotkeys;
  }

  void set_focused(Focusable *e)
  {
    focused = e;
  }

  Focusable *get_focused() const
  {
    return focused;
  }

  std::vector<Element*>* get_hovered_path()
  {
    return &m_hover_path;
  }

  Element *get_root()
  {
    return root;
  }

  void set_root(Element *el)
  {
    std::cout << "Settings root: " << el->id << std::endl;

    root = el;

    auto sz = get_window_size();

    shadow_root = new Element(Rect(0, 0, sz.width, sz.height), BLACK);
    shadow_root->child( root );
  }

  void push_fixed(Element *e)
  {
    shadow_root->child(e);
  }

  Size get_window_size()
  {

    static Size static_window_size(0, 0);

    glfwGetFramebufferSize(window, (int*)&static_window_size.width, (int*)&static_window_size.height);

    // glfwGetWindowSize(window, (int*)&static_window_size.width, (int*)&static_window_size.height);
    // glViewport(0, 0, static_window_size.width, static_window_size.height);

    if ( 
         static_window_size.height != m_last_window_size.height 
      || static_window_size.width != m_last_window_size.width
    ) {
      // glViewport( 0, 0, static_window_size.width, static_window_size.height );
      // glfwSwapBuffers(window);
    }

    return static_window_size;
    // return m_window_size;
  }


  bool is_element_focused(Element *e)
  {
    return focused == e;
  }

  void trigger_focus_events(Focusable *from, Focusable *to)
  {
    if (from)
    {
      from->blur();
    }

    if (to)
    {
      to->focus();
    }
  }

  void update_focus_to(Focusable *child)
  {

    std::cout << "FOCUS ON :: " << typeid(*child).name()  << " | " << child->id << std::endl;


    if ( child == focused ) {
      return;
    } 

    // trigger blur events if needed.
    trigger_focus_events(focused, child);
    
    focused = child;
  }
 
  void check_hovering() {

    for ( auto c : m_hover_path ) {
      if ( auto i = dynamic_cast<Hoverable*>(c) ) {
        i->mouse_leave();
      }
    }    

    m_hover_path.clear();

    get_collision_path( shadow_root, &m_hover_path );
    
    auto i = 0;
    for ( auto c : m_hover_path ) {
      if ( auto h = dynamic_cast<Hoverable*>(c) ) {
        h->mouse_enter();
        i++;
      }
    }    

    // std::cout << "COLLISION PATH ::SIZE= " << m_hover_path.size() << std::endl;
    // std::cout << "COLLISION PATH ::HOVERABLE= " << i << std::endl;
  }

  size_t get_collision_path( Element* el,  std::vector<Element*>* result ) {

      auto children = el->get_children();

      for ( auto child : children  ) {

        if (Collision::is_point_in_rect( &mouse.at, child->get_true_rect() ))  {

          result->push_back(child);

          get_collision_path(child, result);
        }
      }

      return result->size();
  }

  void handle_click()
  {

    auto e = check_mouse_collisions(shadow_root);

    if (!e)
    {
      std::cout << "Clicked outside of any button" << std::endl;

      if ( focused ) {
        focused->blur();
        focused = nullptr;
      } 

      return;
    }

    std::cout << "CLICK ON :: " << e->id << std::endl;

    if (auto i = dynamic_cast<Focusable *>(e))
    {
      if (i)
      {
        update_focus_to(i);
      }
    }

    Button *b = dynamic_cast<Button *>(e);

    if (b)
    {
      b->click();
    }
  }

  bool check_mouse_collision(Element *child)
  {
    auto tr = child->get_true_rect();

    if ( child->is_hidden() ) return false;

    return Collision::is_point_in_rect(&mouse.at, tr);
  }

  Element *check_mouse_collisions(Element *el)
  {

    if (el == nullptr)
    {
      return nullptr;
    }

    auto v = el->get_children();

    for ( int i = v.size()-1; i >= 0; i-- )
    {
      auto c = v.at(i);

      if (check_mouse_collision(c))
      {
        if ( c->has_capability( CFocusable ) ) {
          return c;
        }
        if ( c->has_capability( CClickable) ) {
          return c;
        }

        return check_mouse_collisions( c );
      }
    }

    return nullptr;
  }

  GLFWwindow *get_window()
  {
    return window;
  }

  Coord &get_mouse_position()
  {
    return mouse.at;
  }

  void calc_elements()
  {

    if (root == nullptr)
    {
      std::cout << "ROOT::CALC  ::ROOT_IS_NULL" << std::endl;
      return;
    }

    Size sz = get_window_size();

    GLint vp [4]; 
    glGetIntegerv (GL_VIEWPORT, vp);

    // std::cout << "win is (" << sz.width << "," << sz.height << ")" << std::endl;
    // std::cout << "win is (" << vp[2] << "," << vp[3] << ")" << std::endl;
    // std::cout << "ROOT::CALC" << std::endl;

    shadow_root->set_true_rect(Rect(Coord(0), sz));

    shadow_root->calc_children_true_rects(shadow_root->get_true_rect(), &sz);

    // std::cout << "ROOT::CALC ::END" << std::endl;
  }

  void render()
  {

    Size sz = get_window_size();

    Benchmark::mark(); 

    /**
     * this is very bad for performance:
     * TODO: change it to a manual check agaisn last buffer window_size
     * 
     * @info: This is here because displayServer his asynchrnoous, so the BG can be rendered
     *        with a given context size, while the components did not receive the update yet
     */
    calc_elements();

    // std::cout << "CALC TOOK: " << Benchmark::since_mark(Micro) << " us" << std::endl;

    // Rect r( 40, 100, 200, 200 );
    // draw_rounded_quad( &r, WHITE, glm::vec4( 5, 10, 0, 20 ), &sz );

    // return;

    if (root != nullptr)
    {
      shadow_root->draw(&sz);
    }

    // m_last_window_size = sz;
  }

  static void cb_framebuffer_resize(GLFWwindow *window, int width, int height)
  {

    /**
     * isso aqui estava fodendo tudo.
     */
    // return;

    int fbWidth, fbHeight;

    glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
std::cout << "Framebuffer resized: " << fbWidth << "x" << fbHeight << std::endl;
    glViewport(0, 0, fbWidth, fbHeight);

    // Update your application's size variable using the framebuffer size
    auto g = GLUI::instance();
    g->m_window_size.width  = fbWidth;
    g->m_window_size.height = fbHeight;

    // Recalculate UI elements if needed
    g->calc_elements();
  }

  virtual ~GLUI() = default;

  static void cb_key_press_event(GLFWwindow *w, int key, int scanode, int action, int mods)
  {
    switch (key)
    {

    case GLFW_KEY_ESCAPE:
    {
      if (action == GLFW_PRESS)
      {
        glfwSetWindowShouldClose(w, GL_TRUE);
        return;
      }
    }
    }

    auto focused = GLUI::instance()->get_focused();

    if (focused)
    {
      focused->cb_key_press(w, key, scanode, action, mods);
    }
    else {
      if ( action == GLFW_PRESS ) {
        GLUI::instance()->get_hotkeys()->handleEvent( key, mods );
      }
    }

  }

  static void cb_char_event(GLFWwindow *window, unsigned int codepoint)
  {
    auto glui    = GLUI::instance();
    auto focused = glui->get_focused();

    if (focused)
    {
      focused->cb_char_event(codepoint, window);
      return;
    }

    if ( codepoint == 'f' ) {

      std::cout << "Marked fullscreen" << std::endl;
      Benchmark::mark();

      static int windowed_x, windowed_y, windowed_width, windowed_height;

      GLFWmonitor *monitor = glfwGetPrimaryMonitor();
      if (!monitor)
        return;

      const GLFWvidmode *mode = glfwGetVideoMode(monitor);
      if (!mode)
        return;

      glfwGetWindowPos(window, &windowed_x, &windowed_y);
      glfwGetWindowSize(window, &windowed_width, &windowed_height);

      // glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
      glfwSetWindowSize( window, windowed_width + 10 , windowed_height + 10 );
      // glfwSwapBuffers(window);
    }


  }
  static void cb_mouse_move_event(GLFWwindow* window, double xpos, double ypos)
  {
    // std::cout << "MOUSE::MOVED" << std::endl;

        // auto e = GLUI::instance();
      // auto sz = e->get_window_size();
      // e->mouse.at.x = std::clamp((int16_t)xpos, (int16_t)0, sz.width);
      // e->mouse.at.y = std::clamp((int16_t)ypos, (int16_t)0, sz.height);
      // e->check_hovering();
  }

  static void cb_mouse_event(GLFWwindow *window, int button, int action, int mods)
  {
    switch (button)
    {
    case GLFW_MOUSE_BUTTON_LEFT:
    {
      if (action == GLFW_PRESS)
      {
        GLUI::instance()->get_mouse()->downed();
      }

      if (action == GLFW_RELEASE)
      {
        auto m = GLUI::instance()->get_mouse();

        if (m->get_left_button())
        {

          if (m->get_distance_to_downed() < MOUSE_DISTANCE_CLICK_THRESHOLD)
          {

            auto inst = GLUI::instance();

            // this aplies focus as well
            inst->handle_click();
          }
        }
      }

      GLUI::instance()->get_mouse()->set_button(MOUSE_LEFT, action);
    }

    case GLFW_MOUSE_BUTTON_RIGHT:
    {
      GLUI::instance()->get_mouse()->set_button(MOUSE_LEFT, action);
    }

    case GLFW_MOUSE_BUTTON_MIDDLE:
    {
      GLUI::instance()->get_mouse()->set_button(MOUSE_LEFT, action);
    }
    }
  }
};

#endif