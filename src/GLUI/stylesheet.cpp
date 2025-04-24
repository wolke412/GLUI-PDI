#include <GLUI/stylesheet.hpp>


void TEST_apply_stylesheet( Element* to, Stylesheet* s ) {
    to->set_padding( s->padding );
    to->set_border( s->border );
    to->set_background_color( s->background_color );
    to->set_foreground_color( s->foreground_color );
}