#include <PDI/components.hpp>


void pdi_make_file_dropdown(PDI *pdi, Element *nav, Element *fcont);
void pdi_make_transform_dropdown(PDI *pdi, Element *nav, Element *fcont);

void Components::layout( PDI* pdi ) {

    std::cout << "SETTING LAYOUT" << std::endl; 

    auto glui = pdi->get_glui();

    auto window_size = glui->get_window_size();


    /**
     *  ===========================
     * */
    Pile *root = new Pile(Rect(0, 0, LAYOUT_FILL, LAYOUT_FILL) ,20, Theme::BG_SHADE_200 );
    glui->set_root(root);

    Row *navbar  = new Row(Rect(0, 0, LAYOUT_FILL, 80), 20, Theme::BG_SHADE_300);
    Row *body    = new Row(Rect(0, 0, LAYOUT_FILL, LAYOUT_FILL), 20, Theme::BG_SHADE_100);
    // Row footer(Rect(0, 0, LAYOUT_FILL, 80), 20, RGB(0.6f, 0.2f, 0.27f));

    /**
     * 
     */
    pdi_make_file_dropdown( pdi, navbar, root );
    pdi_make_transform_dropdown( pdi, navbar, root );


    root->child(navbar);
    root->child(body);

    std::cout << "SETTING LAYOUT" << std::endl; 

    navbar->set_padding(Padding(20));
    body->set_padding(Padding(20));
    
    Input *input_img_path = new Input( 320 );    
    input_img_path->set_background_color(RGB(.3,.35,.4));

    auto img_h     = pdi->get_input();
    auto img_h_out = pdi->get_output();

    input_img_path->onchange([=](Input& i ) {
        auto v = i.get_value();

        if ( img_h->is_valid_image_path(v) ) {
            img_h->set_path ( *v );
            img_h->load();

            img_h->copy_to(img_h_out);

            auto p = img_h_out->get_binary();
            auto s = img_h_out->get_size();
            for (auto i = 0; i < s->width * s->height * img_h->get_channel_count(); i++) {
                p[i] = std::min(p[i] + 50, 255);
            }
        }
    });

    std::cout << "LAYOUT ::IMAGES" << std::endl; 

    auto im1 = new Image(img_h,     Rect(Size(512, 512)));
    auto im2 = new Image(img_h_out, Rect(Size(512, 512)));


    Pile *in_img_pile  = new Pile( Rect(0,0, LAYOUT_FILL, LAYOUT_FILL), 20, RGB(BLACK) );
    Pile *out_img_pile = new Pile( Rect(0,0, LAYOUT_FILL, LAYOUT_FILL), 20, RGB(BLACK) );

    in_img_pile->child(input_img_path);
    in_img_pile->child(im1);

    out_img_pile->child(im2);
     
    Row  *images  = new Row(Size(LAYOUT_FILL, LAYOUT_FILL), 20,  Theme::BG_SHADE_100 );
    Pile *sidebar = new Pile( Size (400, LAYOUT_FILL), 20, Theme::BG_SHADE_200 );

 
    images->set_padding( Padding(20) );
    sidebar->set_padding( Padding(20) );


    images->child( in_img_pile );
    images->child( out_img_pile );

    body->child( images );
    body->child( sidebar );

    std::cout << "LAYOUT ::DONE" << std::endl; 
}

struct DropdownOption {
    const std::string name;
    click_callback_t callback;
};

void make_dropdown( GLUI* glui, Element* app_btn, Element* app_modal, const std::string name, DropdownOption *opts, size_t c_opt ) {

    Button *btn_file = new Button(name, Theme::BG_SHADE_300);

    btn_file->set_padding(Padding(10, 20));
    btn_file->get_text()->set_font_size( 18 );

    Pile *dropdown = new Pile(Rect(0, 0, 300, LAYOUT_FIT_CONTENT), 0, Theme::BG_SHADE_200);

    dropdown->hide();

    for ( int i = 0; i < c_opt; i++ ) {
        auto o = opts[i];

        Button *btn_drop = new Button(o.name, Theme::BG_SHADE_300);

        btn_drop->onclick(o.callback);

        btn_drop->set_padding(Padding(10, 20));
        btn_drop->set_rect(Size(LAYOUT_FILL, LAYOUT_FIT_CONTENT));
        btn_drop->get_text()->set_font_size( 18 );

        dropdown->child(btn_drop);
    }

    // por valor pq é tudo ponteiro nesse merda mesmo :: [=] <-
    btn_file->onfocus([=](Focusable &f){ dropdown->show(); });
    btn_file->onblur([=](Focusable &f) { dropdown->hide(); });

    dropdown->onbeforedraw( [=](Element *e) {
        // sets the true_rect to below the button.
        auto btr = btn_file->get_true_rect();

        auto dtr = dropdown->get_true_rect();

        dtr->y = btr->y + btr->height ;
        dtr->x = btr->x; 

        auto s    = glui->get_window_size();
        auto root = glui->get_root();

        // std::cout << "dropdown" << std::endl;
        dropdown->calc_children_true_rects( root->get_true_rect(), &s);
    } );

    dropdown->set_padding(Padding(20));

    app_btn->child(btn_file);
    glui->push_fixed(dropdown);
}

void pdi_make_file_dropdown(PDI *pdi, Element *nav, Element *fcont){

    DropdownOption d[] = {

        { "Abrir imagem", 
            [=](Focusable& f){
                std::cout << "Button 1" << std::endl;
            } 
        },
        { "Salvar imagem", 
            [=](Focusable& f){
                std::cout << "Button 2" << std::endl;
            } 
        },
        { "Sobre", 
            [=](Focusable& f){
                std::cout << "Button 3" << std::endl;
            } 
        },
        { "Sair", 
            [=](Focusable& f){
                std::cout << "Button 4" << std::endl;
                // pdi->exit();
            } 
        },
    };

    make_dropdown(pdi->get_glui(), nav, fcont, "Arquivo", d, 4);
}

void pdi_make_transform_dropdown(PDI *pdi, Element *nav, Element *fcont){

    static float sx = 0.;
    static float sy = 0.;
    static int total_deg = 0;


    DropdownOption d[] = {
        // Transladar, Rotacionar, Espelhar, Aumentar e Diminuir.
        { "Transladar", 
            [=](Focusable& f){
                pdi->m_translate_x += 10;
                pdi->m_translate_y += 10;
                pdi->transform();
            } 
        },
        { "Rotacionar", 
            [=](Focusable& f){
                // fmod is pretty damn bad
                pdi->m_angle = fmod(pdi->m_angle + 10, 360) ;
                pdi->transform();
            } 
        },
        { "Espelhar", 
            [=](Focusable& f){
                pdi->m_mirror_axis = (Axis) ( ( (int)pdi->m_mirror_axis + 1 ) % 4);
                pdi->transform();
            } 
        },
        { "Aumentar", 
            [=](Focusable& f){
                pdi->m_scale_x += .1; 
                pdi->m_scale_y += .1; 
                pdi->transform();
            } 
        },
        { "Diminuir", 
            [=](Focusable& f){
                pdi->m_scale_x -= .1; 
                pdi->m_scale_y -= .1; 
                pdi->transform();
            } 
        },
    };

    make_dropdown(pdi->get_glui(), nav, fcont, "Transformações Geométricas", d, sizeof( d ) / sizeof(DropdownOption) );
}
