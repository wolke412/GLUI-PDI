#include <GLUI/glui.hpp>

namespace Dropdown
{

    struct Option
    {
        const std::string name;
        click_callback_t callback;
    };

    struct Stylesheet {
        RGBA DropdownBackground;
        RGBA ButtonBackground;
        RGBA OptionBackground;
    };

    void make(GLUI *glui, Element *app_btn, Element *app_modal, const std::string name, Dropdown::Option *opts, size_t c_opt, Stylesheet s)
    {

        Button *btn_file = new Button(name, s.ButtonBackground );

        btn_file->set_padding(Padding(10, 20));
        btn_file->get_text()->set_font_size(18);

        Pile *dropdown = new Pile(Rect(0, 0, 300, LAYOUT_FIT_CONTENT), 0, s.DropdownBackground );

        dropdown->hide();

        for (int i = 0; i < c_opt; i++)
        {
            auto o = opts[i];

            Button *btn_drop = new Button( o.name, s.OptionBackground );

            btn_drop->onclick(o.callback);

            btn_drop->set_padding(Padding(10, 20));
            btn_drop->set_rect(Size(LAYOUT_FILL, LAYOUT_FIT_CONTENT));
            btn_drop->get_text()->set_font_size(18);

            dropdown->child(btn_drop);
        }

        // por valor pq é tudo ponteiro nesse merda mesmo :: [=] <-
        btn_file->onfocus([=](Focusable &f)
                          { dropdown->show(); });
        btn_file->onblur([=](Focusable &f)
                         { dropdown->hide(); });

        dropdown->onbeforedraw([=](Element *e)
                               {
        // sets the true_rect to below the button.
        auto btr = btn_file->get_true_rect();

        auto dtr = dropdown->get_true_rect();

        dtr->y = btr->y + btr->height ;
        dtr->x = btr->x; 

        auto s    = glui->get_window_size();
        auto root = glui->get_root();

        // std::cout << "dropdown" << std::endl;
        dropdown->calc_children_true_rects( root->get_true_rect(), &s); });

        dropdown->set_padding(Padding(20));

        app_btn->child(btn_file);
        glui->push_fixed(dropdown);
    }

}