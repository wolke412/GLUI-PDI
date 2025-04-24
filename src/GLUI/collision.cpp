#include <GLUI/collision.hpp>

namespace Collision {
  bool is_point_in_rect ( Coord *c, Rect *r ) {
    return ( c->x >= r->x && 
             c->x <= r->x + r->width &&
             c->y >= r->y && 
             c->y <= r->y + r->height
           );
  }
  bool is_rect_in_rect(Rect* inner, Rect* outer) {
    return (
      inner->x >= outer->x &&
      inner->y >= outer->y &&
      inner->x + inner->width <= outer->x + outer->width &&
      inner->y + inner->height <= outer->y + outer->height
    );
  }
}