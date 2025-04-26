#include <GLUI/glui.hpp>
#include <GLUI/stylesheet.hpp>

class Handle : public Draggable {
private:
public:
    Handle( Size sz, RGBA color ): Element(sz, color){}
};

class InputRange : public Input<float>, public Element {
private:
   Size handle_size = Size(10, 20); 

   int track_width = 4;

   float m_min = 0; 
   float m_max = 1;

   Element * m_handle_element = nullptr;

public:
    InputRange( Size sz, RGBA color ): Element(sz, color){

        int offset_track = (handle_size.height >> 1) - (track_width >> 1);

        Element* ghost_track = new Element( Size( Layout::Fill, handle_size.height ), TRANSPARENT );
        Element* visible_track       = new Element( Rect( 0, offset_track, Layout::Fill, track_width ), Opacity(WHITE, .1) );
        visible_track->set_border( Border(1, Opacity(WHITE, .4)) );

        Handle *h = new Handle( handle_size, RGBA(.4, .7, .2));

        h->set_boundary_type(Parent);

        h->ondragstart([=](Draggable &d) {
        });
        h->ondragmove([=](Draggable &d) {
            float truew = ghost_track->get_true_rect()->width - handle_size.width;
            float percentage = (float)d.get_rect()->x / truew;
            float range_value = m_min + (m_max - m_min) * percentage; 

            set_value( range_value );

            changed();

            // d.get_true_rect()->debug("HANDLE:: ");
            // ghost_track->get_true_rect()->debug("GHOST:: ");
            // visible_track->get_true_rect()->debug("VISIBLE:: ");

            std::cout << "VALUE:: :" << std::to_string(percentage) << std::endl;

        });
        h->ondragend([=](Draggable &d) { 
        });

        ghost_track->child(visible_track);
        ghost_track->child(h);
        child(ghost_track);

        m_handle_element = h;

        // onclick([=](Clickable* c) {
        //     std::cout << "CLiquei na input range" << std::endl;
        // });
    }

    void set_value(float v) override {
       m_value = std::clamp( v, m_min, m_max );
    //    calc_handle_position();
    }

    void set_range(float min, float max) {
        m_min = min;
        m_max = max;
    }

    Element* TEMP_get_handle() { return m_handle_element; }

private:
    void calc_handle_position() {
        float true_width = m_handle_element->m_parent->get_true_rect()->width - handle_size.width;
        float at = *get_value(); 

        int16_t x = at * true_width;
        int16_t y = m_handle_element->get_rect()->get_pos().y;

        m_handle_element->set_position( Coord( x, y ) ); 
        m_handle_element->get_rect()->debug("handle: ");
    }
};