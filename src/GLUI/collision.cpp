#include <GLUI/collision.hpp>

namespace Collision {
  bool is_point_in_rect ( Coord *c, Rect *r ) {
    return ( c->x >= r->x && 
             c->x <= r->x + r->width &&
             c->y >= r->y && 
             c->y <= r->y + r->height
           );
  }
}