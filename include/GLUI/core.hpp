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
  Size window_size = Size(800, 600);

private:
  Mouse mouse;

  GLFWwindow *window;

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

  bool begin()
  {

    if (window == NULL)
    {
      std::cout << "Failed to create GLFW window" << std::endl;
      glfwTerminate();
    }

    glfwMakeContextCurrent(window);
    // glfwSwapInterval(0);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
      std::cout << "Failed to initialize GLAD" << std::endl;
    }

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
    process_input();

    clear();
  }

  void loop_end()
  {
    glfwSwapBuffers(window);
    glfwPollEvents();

    // glFinish();
  }

  void process_input()
  {
    double x, y;

    glfwGetCursorPos(window, &x, &y);

    auto sz = get_window_size();

    mouse.at.x = std::clamp((int16_t)x, (int16_t)0, sz.width);
    mouse.at.y = std::clamp((int16_t)y, (int16_t)0, sz.height);

    // // 
    check_hovering(  );
  }

  void kill () {
    glfwTerminate();
  }

  void clear() {
    static auto b = BLACK;
    glClearColor( 1., b.G, b.B, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
  }

  Mouse *get_mouse()
  {
    return &mouse;
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

    static Size window_size(0, 0);

    glfwGetFramebufferSize(window, (int*)&window_size.width, (int*)&window_size.height);

    return window_size;
    // return m_window_size;
  }


  bool is_element_focused(Element *e)
  {
    return focused == e;
  }

  void blurred(Focusable *from, Focusable *to)
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

    if ( child == focused ) {
      return;
    } 

    // trigger blur events if needed.
    blurred(focused, child);
    
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
      // TODO: handle focus loss
      std::cout << "Clicked outside of any button" << std::endl;

      if ( focused ) {
        focused->blur();
        focused = nullptr;
      } 

      return;
    }

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

  Element *check_mouse_collision(Element *child)
  {
    auto tr = child->get_true_rect();

    if ( child->is_hidden() ) return nullptr;

    std::cout << "Checking collision of " << child->id << std::endl;

    if (Collision::is_point_in_rect(&mouse.at, tr))
    {
      if (auto i = dynamic_cast<Focusable *>(child))
      {
        if (i)
        {
          return child;
        }
      }

      // std::cout << "Found collision of " << child->id << std::endl;
      /**
       * FOR NOW:
       * If is a BUTTON; means its th e ONLY clickable thing in the path
       * there cannot be clickable shit inside of each other
       */
      if (dynamic_cast<Button *>(child))
      {
        return child;
      }


      auto r = check_mouse_collisions(child);

      if (r)
      {
        return r;
      }

      // its a non-clickable, then... fuckit
      return nullptr;
    }
  }

  Element *check_mouse_collisions(Element *el)
  {

    if (el == nullptr)
    {
      return nullptr;
    }

    auto c = el->get_children();

    for (auto it = c.rbegin(); it != c.rend(); ++it) {

      if (auto i = check_mouse_collision(*it) ) {
        return i;
      }
    }

    // for (auto child : el->get_children() )
    // {
    //   if (auto i = check_mouse_collision(child) ) {
    //     return i;
    //   }
    // }

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

    std::cout << "ROOT::CALC" << std::endl;

    shadow_root->set_true_rect(Rect(Coord(0), sz));

    shadow_root->calc_children_true_rects(shadow_root->get_true_rect(), &sz);

    std::cout << "ROOT::CALC ::END" << std::endl;
  }

  void render()
  {
    Size sz = get_window_size();


    /**
     * this is very bad for performance:
     * TODO: change it to a manual check agaisn last buffer window_size
     * 
     * @info: This is here because displayServer his asynchrnoous, so the BG can be rendered
     *        with a given context size, while the components did not receive the update yet
     */
    Benchmark::mark(); 
    calc_elements();
    std::cout << "CALC TOOK: " << Benchmark::since_mark(Micro) << " us" << std::endl;

    if (root != nullptr)
    {
      shadow_root->draw(&sz);
    }
  }

  static void cb_framebuffer_resize(GLFWwindow *window, int width, int height)
  {

    std::cout << "Received resize event after : " << Benchmark::since_mark( Unit::Micro ) << "micross" << std::endl;

    // glfwMakeContextCurrent(window);
    glViewport(0, 0, width, height);

    auto g = GLUI::instance();
    g->window_size.width  = width;
    g->window_size.height = height;

    /**
     * recalculates ui
     */
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

      glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
    }


  }
  static void cb_mouse_move_event(GLFWwindow* window, double xpos, double ypos)
  {
    // std::cout << "MOUSE::MOVED" << std::endl;

      auto e = GLUI::instance();
      auto sz = e->get_window_size();
      e->mouse.at.x = std::clamp((int16_t)xpos, (int16_t)0, sz.width);
      e->mouse.at.y = std::clamp((int16_t)ypos, (int16_t)0, sz.height);
      e->check_hovering();
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