#include <GLUI/glui.hpp>
#include <GLUI/stylesheet.hpp>

namespace Dropdown
{

    struct Option
    {
        const std::string name;
        click_callback_t callback;
        int value;
    };

    struct Styles {
        Stylesheet list;
        Stylesheet option;

        Stylesheet active_option;

        Stylesheet button;
        Stylesheet hover_button;
        Stylesheet active_button;
    };

    Styles get_default_styles() {
        return Styles {
           .list = Stylesheet {
                .background_color = RGBA(.08, .10, .15),
                .border = Border( 0, 1, 1, 1, WHITE ),
                .padding = Padding(Spacing::Small),
           }, 

           .option = Stylesheet {
                .background_color = TRANSPARENT,
                .foreground_color = Opacity(WHITE, .6),
                .padding = Padding(Spacing::Small, Spacing::MediumS)
           },

           .active_option = Stylesheet {
                .background_color = Opacity(WHITE, .1),
                .foreground_color = Opacity(WHITE, 1.),
                .border = Border(0, 0, 0, 4, RGBA(.17, .23, .78, 1.)),
                .padding = Padding(Spacing::Small, Spacing::MediumS)
           },

           .button = Stylesheet {
                .background_color = Property( DARKGREY ),
                .foreground_color = Property( WHITE ),
                .border = Border(1, Opacity(WHITE, .2)),
                .padding = Padding( Spacing::Small, Spacing::MediumS )
           }, 
        };
    }
}

/**
 *  Class that generates a SelectBox input
 */
class Selectbox: public Button, Input<int> 
{
private:
    Element* m_dropdown = nullptr;
    Dropdown::Styles m_stylesheet = Dropdown::get_default_styles();

    int m_selected = 0;

    std::vector<Dropdown::Option> m_options;
    std::vector<Element*> m_options_buttons;

    void set_active_option( size_t index ) {

        auto cnew = m_dropdown->get_children()[index];
        auto cold = m_dropdown->get_children()[m_selected];

        TEST_apply_stylesheet( cnew, &m_stylesheet.active_option );
        TEST_apply_stylesheet( cold, &m_stylesheet.option );

        m_selected = index;

        auto opt = m_options[ m_selected ];
        
        get_text()->set_text( opt.name );

        set_value( opt.value );
        changed();
    }
public: 

    Selectbox( GLUI *glui, std::vector<Dropdown::Option> opts ): 
        Button("Sem opções", TRANSPARENT), 
        Element(Size(FitContent), TRANSPARENT),
        m_stylesheet(Dropdown::get_default_styles()),
        m_options(opts)
    {

        set_padding(m_stylesheet.button.padding);
        set_border(m_stylesheet.button.border);
        get_text()->set_font_size(18);
        get_text()->set_foreground_color(m_stylesheet.button.foreground_color);

        if ( opts.size() ) {
            get_text()->set_text( opts[0].name );
        }

        auto list_style = m_stylesheet.list;
        m_dropdown = new Pile(Size(FitContent, FitContent), 0, list_style.background_color.value);
        m_dropdown->set_padding( list_style.padding );
        m_dropdown->set_border(  list_style.border  );
        m_dropdown->hide();

        onfocus([=](Focusable &f) { m_dropdown->show(); });
        onblur([=](Focusable &f ) { m_dropdown->hide(); });

        auto c_opt = m_options.size();
        auto opt_style = m_stylesheet.option;
        int max_width = 0;

        for (int i = 0; i < c_opt; i++)
        {
            auto o = opts[i];

            Button *btn_drop = new Button( o.name, opt_style.background_color );
            TEST_apply_stylesheet(btn_drop, &opt_style);

            btn_drop->onclick([=](Button &b) {
                this->set_active_option( i );
            });

            btn_drop->set_rect(Size(Fill, FitContent));
            btn_drop->get_text()->set_font_size(18);
            get_text()->set_foreground_color(m_stylesheet.button.foreground_color);

            m_dropdown->child(btn_drop);
            m_dropdown->onbeforedraw([=](Element *e) {
                // sets the true_rect to below the button.
                auto btr = this->get_true_rect();
                auto dtr = m_dropdown->get_true_rect();
                dtr->y = btr->y + btr->height;
                dtr->x = btr->x;

                auto s = glui->get_window_size();
                auto root = glui->get_root();

                m_dropdown->calc_children_true_rects(root->get_true_rect(), &s); 
            });

            m_options_buttons.push_back( btn_drop );

            int w = TextCalcRenderWidth( o.name, btn_drop->get_text()->get_scale() );
            if ( w > max_width ) max_width = w;

            glui->push_fixed(m_dropdown);
        }

        TEST_apply_stylesheet( m_dropdown->get_children()[0], &m_stylesheet.active_option );

        static int CHEVRON_SIZE = 24;
        child( new Image("public/chevron-down.png", Rect( Coord(max_width + Spacing::MediumS, 0), Size(CHEVRON_SIZE) )) );
        
        auto t = get_text();
        auto c = t->get_rect()->get_pos();
        c.y = ( CHEVRON_SIZE - t->get_line_height() ) >> 1;
        t->set_position( c );

        auto r = m_dropdown->get_reserved_size().width;

        m_dropdown->set_size( Size(  
            max_width + (Spacing::MediumS * 2) + r + CHEVRON_SIZE, 
            Layout::FitContent 
        ) );

        set_size( Size(  
            m_dropdown->get_rect()->get_size()->width,
            Layout::FitContent 
        ) );
    }

};