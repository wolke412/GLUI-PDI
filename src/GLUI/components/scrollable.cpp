#include <glui.hpp>
#include <GLUI/components/scrollable.hpp>

// Base class for UI elements with children
class ElementWithChildren {
    public:
        virtual void drawChildren() {
            std::cout << "Drawing children elements\n";
        }
    }; 


class ScrollablePane {
  private:
    GLFWwindow *window;
    Rect rect;

    float contentHeight;

    float scroll_offset = 0.0f;

    float lastMouseY = 0.0f;

    bool is_dragging_scroll = false;
    bool is_auto_positioned = false;
  
  public:
      // Constructor
      ScrollablePane(GLFWwindow* window, Rect r): window(window), rect(r){
        glfwSetMouseButtonCallback(window, ScrollablePane::mouseButtonCallback);
        glfwSetCursorPosCallback(window, ScrollablePane::cursorPositionCallback);
      };

      ScrollablePane(GLFWwindow* window, Size sz): window(window), rect(Rect(Coord(0,0), sz)), is_auto_positioned(true) {
        glfwSetMouseButtonCallback(window, ScrollablePane::mouseButtonCallback);
        glfwSetCursorPosCallback(window, ScrollablePane::cursorPositionCallback);
      };

      /**
       * this will be used by auto-layout components
       */
      void setTruePosition( Coord c ) {
        rect.x = c.x;
        rect.y = c.y;
      }

      // Set content height dynamically
      void setContentHeight(float newContentHeight) {
          contentHeight = newContentHeight;
      }
  
        // Mouse Button Callback
    static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
        void* ptr = glfwGetWindowUserPointer(window);
        if (!ptr) return;

        ScrollablePane* pane = static_cast<ScrollablePane*>(ptr);
        if (button == GLFW_MOUSE_BUTTON_LEFT) {
            double mouseX, mouseY;
            glfwGetCursorPos(window, &mouseX, &mouseY);

            int window_width, window_height;
            glfwGetFramebufferSize(window, &window_width, &window_height);
            mouseY = window_height - mouseY; // Convert to OpenGL coordinates

            Coord mousePos = { (float)mouseX, (float)mouseY };
            Rect scrollbarRect = pane->getScrollbarRect();

            if (action == GLFW_PRESS && Collision::is_point_in_rect(&mousePos, &scrollbarRect)) {
                pane->is_dragging_scroll = true;
                pane->lastMouseY = mousePos.y;
            } else if (action == GLFW_RELEASE) {
                pane->is_dragging_scroll = false;
            }
        }
    }

    // Mouse Cursor Position Callback
    static void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos) {
        void* ptr = glfwGetWindowUserPointer(window);
        if (!ptr) return;

        ScrollablePane* pane = static_cast<ScrollablePane*>(ptr);
        if (pane->is_dragging_scroll) {
            int window_width, window_height;
            glfwGetFramebufferSize(window, &window_width, &window_height);
            ypos = window_height - ypos; // Convert to OpenGL coordinates

            float deltaY = ypos - pane->lastMouseY;
            pane->lastMouseY = ypos;

            float scrollRange = pane->rect.height - pane->getScrollbarHeight();
            float contentRange = pane->contentHeight - pane->rect.height;

            if (scrollRange > 0) {
                pane->scroll_offset -= (deltaY / scrollRange) * contentRange;
                pane->scroll_offset = std::max(0.0f, std::min(pane->scroll_offset, contentRange));
            }
        }
    }
      // Draw the scrollable pane
      void draw() {
          // Enable scissor test to clip rendering inside the pane
          glEnable(GL_SCISSOR_TEST);
  
          int window_width, window_height;
          glfwGetFramebufferSize(window, &window_width, &window_height);
          int scissor_y = window_height - (rect.y + rect.height);
          glScissor(rect.x, scissor_y, rect.width, rect.height);
  
          // Draw the pane background
        //   draw_quad(Rect(rect.x, y, width, height), );  // Using your draw_quad function
  
          // Draw buttons or any other content
          // Add code here to draw the actual content (buttons, images, etc.)
  
          glDisable(GL_SCISSOR_TEST); // Disable scissor test before drawing the border
        
          Size s(window_width, window_height);


          // Draw scrollbar
          draw_quad(Rect(
              getScrollbarRect().x, 
              getScrollbarRect().y, 
              getScrollbarWidth(), 
              getScrollbarHeight()),
              RGB(.2,.25,.4),
              &s
            ); 
      }
  
  private:

      // Helper to get the scrollbar rectangle
      Rect getScrollbarRect() {
          float scrollbarX = rect.x + rect.width - getScrollbarWidth() - 2;
          float scrollbarY = rect.y + (rect.height - getScrollbarHeight()) - (scroll_offset / contentHeight) * rect.height;
          return { scrollbarX, scrollbarY, getScrollbarWidth(), getScrollbarHeight() };
      }
  
      // Helper to get the scrollbar height based on content
      float getScrollbarHeight() {
          return (rect.height / contentHeight) * rect.height;
      }
  
      // Helper to get the scrollbar width
      float getScrollbarWidth() {
          return 10.0f;
      }
  };
  