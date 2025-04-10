#ifndef GLUI_MODAL_H
#define GLUI_MODAL_H

#include <GLUI/glui.hpp>
#include <functional>
class Modal: public Row {

using cb_modal = std::function<void( Modal& )>;

private: 

    GLUI * m_glui;

    Pile*    m_modal;
    Row*     m_header;
    Button*  m_close;
    Text*    m_heading;
    Element* m_content;


    cb_modal cb_close = nullptr ;
    cb_modal cb_open = nullptr ;

public:

    Modal( GLUI * g, const std::string H, Element* content ):
        Row( Size( Layout::Fill, Layout::Fill ), 40, Opacity(BLACK, .55) )
    {
        m_glui = g;
        
        m_header  = new Row( Size( Fill, FitContent ), Spacing::LargeX, TRANSPARENT );
        m_close   = new Button( "X", Opacity(WHITE, .1) );
        m_heading = new Text( H );
        m_content = content;

        // botãozao pra fechar esta bosta
        m_close->onclick( [=](Button& b) { this->hide(); } );
        m_close->set_hover_background_color( Opacity(WHITE, .2) );
        m_close->set_padding( Padding( Spacing::Small, Spacing::SmallM ) );
        m_close->get_text()->set_font_size( FontSize::S_700 );

        // ain
        m_header->child( m_heading );
        m_header->child( m_close );

        m_modal = new Pile( Size( Layout::FitContent, Layout::FitContent ), 20, BLACK );
        m_modal->set_border( Border(1, Opacity(WHITE, .1) ) );
        m_modal->set_padding( Spacing::Large );

        m_modal->child(m_header);
        m_modal->child(m_content);
        
        child( m_modal );

        set_padding( Spacing::LargeX );

        // hide itself
        hide();

        g->push_fixed(this);
    }

    void set_modal_background( RGBA bg ) {
        m_modal->set_background_color(bg);
    }

    virtual void draw( Size *window ) {
        
        if ( hidden ) {
            return;
        }

        if ( cb_before_draw ) {
            cb_before_draw(this);
        }

        draw_quad(true_rect, bg_color, window);

        draw_border( window );

        draw_children(NULL, window);

    }

    ~Modal() {
        // são tujdo filho dessa merda, vão ser deletado junto
        delete m_modal;
    }

};

#endif