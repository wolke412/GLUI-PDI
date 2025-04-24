#include <GLUI/rect.hpp>

namespace Collision {

  bool is_point_in_rect( Coord *point, Rect *r );
  bool is_rect_in_rect(Rect* inner, Rect* outer);
  
}