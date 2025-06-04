#include <PDI/components.hpp>
#include <GLUI/components/Dropdown.hpp>


void pdi_make_file_dropdown(PDI *pdi, Element *nav, Element *fcont);
void pdi_make_transform_dropdown(PDI *pdi, Element *nav, Element *fcont);
void pdi_make_segmentation_dropdown(PDI *pdi, Element *nav, Element *fcont);
void pdi_make_filters_dropdown(PDI *pdi, Element *nav, Element *fcont);
void pdi_make_morphology_dropdown(PDI *pdi, Element *nav, Element *fcont);

/**
 *   nice cock 
 */
Modal* pdi_make_open_file_modal( PDI* pdi );

/**
 *  Ai
 */
Element* make_pipeline_stage( PDI *pdi, Stage s, int index );

/**
 *  ========================================
 * 
 */
void Components::layout( PDI* pdi ) {

    std::cout << "SETTING LAYOUT" << std::endl; 

    auto glui = pdi->get_glui();
    auto window_size = glui->get_window_size();

    /**
     * Set root
     * ===========================
     */
    Pile *root = new Pile(Rect(0, 0, LAYOUT_FILL, LAYOUT_FILL) , 0, Theme::BG_SHADE_200 );
    glui->set_root(root);

    Row *navbar  = new Row(Rect(0, 0, LAYOUT_FILL, 80), 20, Theme::BG_SHADE_300);
    Row *body    = new Row(Rect(0, 0, LAYOUT_FILL, LAYOUT_FILL), 20, Theme::BG_SHADE_100);

    // std::cout << "ROOT HAS BEEN SET" << std::endl; 

    /**
     * 
     * ============================================================
     *  O CÓDIGO COMETADO ABAIXO É USADO PARA DEBUGAR OS ELEMENTOS
     *  VISUAIS DA INTERFACE.
     * ============================================================
     * 
     */
    {
        // Row* ROW_1   = new Row(  Size( FitContent, FitContent), 100, TRANSPARENT );
        // Pile* PILE_1 = new Pile( Size( FitContent, FitContent),  20, TRANSPARENT );

        // ROW_1->set_border( Border(2, RGBA(1,0,0)));
        // PILE_1->set_border(Border(2, RGBA(1,1,0)));

        // ROW_1->set_padding(Padding(2));
        // PILE_1->set_padding(Padding(2));

        // Element* BOX_R = new Element( Size( 100, 100 ), RGBA(1,0,0) );
        // Element* BOX_Y = new Element( Size( 100,  40 ), RGBA(1,1,0) );
        // Element* BOX_G = new Element( Size( 100,  80 ), RGBA(0,1,0) );

        // ROW_1->child( BOX_R );
        // ROW_1->child( PILE_1 );
        // PILE_1->child( BOX_Y );
        // PILE_1->child( new Text( "Ola meus amigos 1", RGBA( 1, 0 ,1 ) ) );
        // PILE_1->child( BOX_G );

        // PILE_1->child( new Text( "Ola meus amigos 2", WHITE ) );

        // PILE_1->child( new Button( "Botão 1", RGBA( 0, 1, 1 ) ) );
        // PILE_1->child( new Button( "Botão 2", RGBA( 0.5, .5, 1 ) ) );

        // ROW_1->child( new Text( "Texto qualquer", RGBA( 1, 0 ,1 ) ) );
        // ROW_1->child( new Button( "Botão 4", RGBA( .8, .2, .5 ) ) );

        // root->child( ROW_1 );

        // return;
    }
    
    pdi_make_file_dropdown( pdi, navbar, root );
    pdi_make_transform_dropdown( pdi, navbar, root );
    pdi_make_segmentation_dropdown( pdi, navbar, root );
    pdi_make_filters_dropdown( pdi, navbar, root );
    pdi_make_morphology_dropdown( pdi, navbar, root );


    root->child(navbar);
    root->child(body);

    std::cout << "SETTING LAYOUT" << std::endl; 

    navbar->set_padding(Padding(20));
    body->set_padding(Padding(0));
    
    TextInput *input_img_path = new TextInput( 320 );    

    input_img_path->set_background_color(Theme::BG_SHADE_100);
    input_img_path->set_border(Border(1, Theme::BG_SHADE_400));

    auto img_h     = pdi->get_input();
    auto img_h_out = pdi->get_output();

    input_img_path->onchange([=]( Input<std::string>& i ) {
        auto v = i.get_value();

        if ( img_h->is_valid_image_path(v) ) {
            img_h->set_path ( *v );
            img_h->load();

            img_h->copy_to(img_h_out);

            img_h_out->request_texture_reload();

            #if USE_GPU == 1
                img_h_out->reset_fbo();
            #endif

            pdi->reset_transform();
        }
    });

    std::cout << "LAYOUT ::IMAGES" << std::endl; 

    auto im1 = new Image(img_h,     Rect(Size(512, 512)));
    auto im2 = new Image(img_h_out, Rect(Size(512, 512)));

    Pile *in_img_pile  = new Pile( Rect(0,0, Layout::FitContent, Layout::FitContent), Spacing::Medium, Theme::BG_SHADE_100 );
    Pile *out_img_pile = new Pile( Rect(0,0, Layout::FitContent, Layout::FitContent), Spacing::Medium, Theme::BG_SHADE_100 );

    in_img_pile->set_border(Border( 1, Opacity(WHITE, .2) ));
    out_img_pile->set_border(Border( 1, Opacity(WHITE, .2) ));
    in_img_pile->set_padding(  Padding( Spacing::Medium ) );
    out_img_pile->set_padding( Padding( Spacing::Medium ) );

    Text *head_in  = new Text( "Entrada" );
    Text *head_out = new Text( "Saída" );

    Text *size_in  = new Text( img_h->get_size()->pretty()    , 14, Opacity( WHITE, .4 ) );
    Text *size_out = new Text( img_h_out->get_size()->pretty(), 14, Opacity( WHITE, .4 ) );

    // img_h->ondatachanged([](  ) {
    //     std::cout << "Hello world" << std::endl;
    // });

    head_in->set_foreground_color( Opacity( WHITE, .6 ) );
    head_out->set_foreground_color( Opacity( WHITE, .6 ) );

    // in_img_pile->child(input_img_path);
    in_img_pile->child(head_in);
    in_img_pile->child(size_in);
    in_img_pile->child(im1);

    out_img_pile->child(head_out);
    out_img_pile->child(size_out);
    out_img_pile->child(im2);
     
    Row  *images  = new Row(  Size ( LAYOUT_FILL, LAYOUT_FILL ), 20,  Theme::BG_SHADE_100 );
    Pile *sidebar = new Pile( Size ( 400, LAYOUT_FILL ), 20, Theme::BG_SHADE_200 );
    images->set_padding(20);

    // TODO: make this less dumb
    pdi->render_pipeline = sidebar;
 
    images->set_padding(  Padding(20) );
    sidebar->set_padding( Padding(20) );

    // Row *fixedCont = new Row( Size( 400, 500 ), Alignment::Even, Alignment::Center );
    // fixedCont->set_padding(20);
    // fixedCont->set_border(Border(2, Opacity(WHITE, .3)));

    // fixedCont->child( new InputRange( Size(Fill, 20), Opacity(WHITE, .1) ) );
    auto list = std::vector<Dropdown::Option>({
        Dropdown::Option{ .name="Elemento 1", .value=1 },
        Dropdown::Option{ .name="Elemento 2", .value=2 },
        Dropdown::Option{ .name="Elemento 3", .value=3 },
        Dropdown::Option{ .name="Elemento 4", .value=4 },
        Dropdown::Option{ .name="Elemento 5", .value=5 },
        Dropdown::Option{ .name="Elemento 6", .value=6 },
    });

    // fixedCont->child( new Selectbox( glui, list ) );
    // fixedCont->child( new Element( Size(20, 20), RGBA(1, 1, 0) ) );

    // images->child(fixedCont);

    images->child( in_img_pile );
    images->child( out_img_pile );

    body->child( images );
    body->child( sidebar );

    std::cout << "LAYOUT ::DONE" << std::endl; 

}

void Components::generate_pipeline_components(PDI *pdi) {
    auto stages   = pdi->get_pipeline()->get_stages();
    auto renderat = pdi->render_pipeline;

    renderat->drop_children();

    int i = 0;
    for ( auto s : stages ) {
        renderat->child( make_pipeline_stage( pdi, s, i++) );
    }
}

Element* make_pipeline_stage( PDI *pdi, Stage s, int index ) {

    auto p = new Pile( Size(Fill, FitContent), 0, Theme::BG_SHADE_300 );
    auto container_s = Stylesheet {
        .background_color = Theme::BG_SHADE_300,
        .border = Border( 1, Opacity(WHITE, .1), Corners(10) )
    };

    auto heading = new Row( Size(Fill, FitContent ), Alignment::Even, Alignment::Center);
    auto heading_s = Stylesheet {
        .background_color = Theme::BG_SHADE_300,
        .border = Border(0, TRANSPARENT, Corners(10, 10, 0, 0)),
        .padding = Padding(Spacing::Medium)
    };

    TEST_apply_stylesheet( heading, &heading_s ); 
    TEST_apply_stylesheet( p, &container_s); 

    auto options = new Row( Size(FitContent, FitContent), Spacing::Small);
    options->set_alignment_y( Alignment::Center );

    auto xb = new Button( "x", Theme::ERROR ); 
    auto xbs = Stylesheet{
        .background_color = Theme::ERROR,
        .foreground_color = WHITE,
        .border = Border( 1, 1, 4, 1, Opacity(BLACK, .2), Corners(4) ),
        .padding = Padding( 2, 8 ),
        .font_size = 16
    };
    TEST_apply_stylesheet( xb, &xbs);

    auto en = new Checkbox( 20 ); 

    en->set_background_color(TRANSPARENT);
    en->set_border( Border(2, Theme::BG_SHADE_400, Corners(4)) );

    en->set_value( s->is_enabled() );

    en->onchange( [pdi, s](Input<bool>& t) {
        std::cout << "Stage input: " <<  std::to_string(*t.get_value()) << std::endl;

        if ( *t.get_value() ) {
            s->enable();
        } else {
            s->disable();
        }

        pdi->update_pipeline();
    });

    xb->onclick( [pdi, index](Button &b) {
        pdi->get_pipeline()->remove(index);
        pdi->update_pipeline();
    });

    options->child(en);
    options->child(xb);
    heading->child( new Text( s->get_title() ) );
    heading->child( options );

    p->child( heading );
    p->child( s->render(pdi) );

    return p;
}

struct DropdownOption {
    const std::string name;
    click_callback_t callback;
};

void make_dropdown( GLUI* glui, Element* app_btn, Element* app_modal, const std::string name, DropdownOption *opts, size_t c_opt ) {

    Button *btn_file = new Button(name, Theme::BG_SHADE_300);

    btn_file->set_border(Border( 1, Theme::BG_SHADE_500 ));
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

    auto modal_open = pdi_make_open_file_modal( pdi );

    pdi->get_glui()->push_fixed( modal_open );

    DropdownOption d[] = {

        { "Abrir imagem", 
            [=](Focusable& f){
                modal_open->show();
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
                pdi->request_update();
            } 
        },
        { "Rotacionar", 
            [=](Focusable& f){
                // fmod is pretty damn bad
                pdi->m_angle = fmod(pdi->m_angle + 10, 360) ;
                pdi->request_update();
            } 
        },
        { "Espelhar", 
            [=](Focusable& f){
                pdi->m_mirror_axis = (Axis) ( ( (int)pdi->m_mirror_axis + 1 ) % 4);
                pdi->request_update();
            } 
        },
        { "Aumentar", 
            [=](Focusable& f){
                pdi->m_scale_x += .1; 
                pdi->m_scale_y += .1; 
                pdi->request_update();
            } 
        },
        { "Diminuir", 
            [=](Focusable& f){
                pdi->m_scale_x -= .1; 
                pdi->m_scale_y -= .1; 
                pdi->request_update();
            } 
        },
    };

    make_dropdown(pdi->get_glui(), nav, fcont, "Transformações Geométricas", d, sizeof( d ) / sizeof(DropdownOption) );
}

void pdi_make_segmentation_dropdown(PDI *pdi, Element *nav, Element *fcont){

    DropdownOption d[] = {
        { "Inverter", 
            [=](Focusable& f){
                pdi->get_pipeline()->push( new BP::Invert() );
                pdi->update_pipeline();
            } 
        },
        { "Greyscale", 
            [=](Focusable& f){
                pdi->get_pipeline()->push( new BP::Greyscale(1., 1., 1.) );
                pdi->update_pipeline();
            } 
        },
        { "Briho e Constraste", 
            [=](Focusable& f){
                pdi->get_pipeline()->push( new BP::Brightness(0, 1.) );
                pdi->update_pipeline();
            } 
        },
        { "Limiar", 
            [=](Focusable& f){
                pdi->get_pipeline()->push( new BP::Threshold(1.) );
                pdi->update_pipeline();
            } 
        },
    };

    make_dropdown(pdi->get_glui(), nav, fcont, "Segmentações", d, sizeof( d ) / sizeof(DropdownOption) );
}

void pdi_make_filters_dropdown(PDI *pdi, Element *nav, Element *fcont){

    DropdownOption d[] = {
        { "Mediana", 
            [=](Focusable& f){
                pdi->get_pipeline()->push( new BP::Filter(BP::LowPass::Median, 3) );
                pdi->update_pipeline();
            } 
        },
        { "Gaussiano", 
            [=](Focusable& f){
                pdi->get_pipeline()->push( new BP::Filter(BP::LowPass::Gaussian, 7) );
                pdi->update_pipeline();
            } 
        },

        { "Sobel", 
            [=](Focusable& f){
                pdi->get_pipeline()->push( new BP::Sobel( .7 ) );
                pdi->update_pipeline();
            } 
        },

        { "Robinson", 
            [=](Focusable& f){
                pdi->get_pipeline()->push( new BP::Robinson( .7 ) );
                pdi->update_pipeline();
            } 
        },
    };

    make_dropdown(pdi->get_glui(), nav, fcont, "Filtros", d, sizeof( d ) / sizeof(DropdownOption) );
}

void pdi_make_morphology_dropdown(PDI *pdi, Element *nav, Element *fcont){

    DropdownOption d[] = {

        { "Erodir", 
            [=](Focusable& f){
                pdi->get_pipeline()->push( new BP::Erosion(BP::MorphKernel::Cross) );
                pdi->update_pipeline();
            } 
        },
        { "Dilatar", 
            [=](Focusable& f){
                pdi->get_pipeline()->push( new BP::Dilation(BP::MorphKernel::Cross) );
                pdi->update_pipeline();
            } 
        },

        { "Abertura", 
            [=](Focusable& f){
                pdi->get_pipeline()->push( new BP::Opening(BP::MorphKernel::Cross) );
                pdi->update_pipeline();
            } 
        },
        { "Fechamento", 
            [=](Focusable& f){
                pdi->get_pipeline()->push( new BP::Closing(BP::MorphKernel::Cross) );
                pdi->update_pipeline();
            } 
        },
        { "Afinamento", 
            [=](Focusable& f){
                pdi->get_pipeline()->push( new BP::ThinningHolt(1) );
                pdi->update_pipeline();
            } 
        },
    };

    make_dropdown(pdi->get_glui(), nav, fcont, "Morfologia", d, sizeof( d ) / sizeof(DropdownOption));
}

/**
 *  ============================================================
 *      RANDOM SHIT
 *  ============================================================
 * 
 */
Modal* pdi_make_open_file_modal( PDI* pdi ) {

    std::cout << "Building modal" << std::endl;
    Pile* open_file_form = new Pile( Size(400, Layout::FitContent ), Spacing::Medium, Opacity(WHITE, .0) );

    TextInput *input_img_path = new TextInput( Fill );    
    Text *error_message       = new Text( "Erro desconhecido", Theme::ERROR );
    Text *success_message     = new Text( "Sucesso!", Theme::SUCCESS );
    Button *btn_open_image    = new Button( "Abrir imagem", DARKGREY );    


    // input style
    input_img_path->set_background_color(Theme::BG_SHADE_100);
    input_img_path->set_border(Border(1, Theme::BG_SHADE_400));
    input_img_path->set_padding( Padding( Spacing::SmallM, Spacing::MediumL ) );

    // button style
    btn_open_image->set_hover_background_color(Theme::SECONDARY);
    btn_open_image->set_border(  Border( 2, Opacity(WHITE, .2) ) );
    btn_open_image->set_padding( Padding( Spacing::SmallM, Spacing::MediumL ) );

    // messages styles
    error_message->set_font_size( FontSize::S_700 );
    success_message->set_font_size( FontSize::S_700 );

    error_message->hide();
    success_message->hide();

    std::cout << "Settings components modal" << std::endl;

    // set them children
    open_file_form->child( input_img_path );
    open_file_form->child( success_message );
    open_file_form->child( error_message );
    open_file_form->child( btn_open_image );

    Modal* open_file     = new Modal( pdi->get_glui(), "Carregar imagem", open_file_form);
    open_file->set_modal_background( Theme::BG_SHADE_400 );

    auto img_i     = pdi->get_input();
    auto img_o     = pdi->get_output();

    input_img_path->set_value( img_i->get_path() );

    std::cout << "Setting callbacks" << std::endl;

    input_img_path->onfocus([=]( Focusable& i ) {
        input_img_path->set_border(Border(1, Theme::BG_SHADE_400));
    });

    // TODO: implement "disabled" for buttons and other components wich may require it
    input_img_path->onchange([=]( Input<std::string>& i ) {
        auto v = i.get_value();
        bool is_valid = img_i->is_valid_image_path(v);

        if ( is_valid ) {
            auto size = img_o->get_size();
            success_message->set_text( "Imagem válida" );
            input_img_path->set_border(Border( 1, Theme::SUCCESS));
            
            btn_open_image->set_background_color( Theme::PRIMARY );
        }

        else {
            error_message->set_text("Caminho inválido.");
            input_img_path->set_border(Border( 1, Theme::ERROR ));

            btn_open_image->set_background_color( DARKGREY );
        }

        success_message->toggle_hidden( ! is_valid );
        error_message->toggle_hidden(     is_valid );

    });

    btn_open_image->onclick([=]( Button& b ) {
        auto v = input_img_path->get_value();

        bool is_valid = img_i->is_valid_image_path(v);

        std::cout << "Input: " << v << std::endl;

        if ( is_valid ) {

            img_i->set_path( *v );
            img_i->load();

            img_i->copy_to(img_o);
            img_o->request_texture_reload();
            // img_o->generate_texture();

            #if USE_GPU == 1
                img_o->reset_fbo();
            #endif

            pdi->reset_transform();
            pdi->request_update();

            /**
             * top 10 mensagens
             */
            auto size = img_o->get_size();
            success_message->set_text( "Imagem carregada: (" + std::to_string( size->width ) + "x" + std::to_string(size->height) + ")");
            input_img_path->set_border(Border( 1, Theme::SUCCESS));
        }

        else {
            error_message->set_text("Caminho inválido.");
            input_img_path->set_border(Border( 1, Theme::ERROR ));
        }

        success_message->toggle_hidden( !is_valid );
        error_message->toggle_hidden( is_valid );
    });
 
    
    // open_file->onbeforedraw( [=](Element *e) {
    //     auto s    = pdi->get_glui()->get_window_size();
    //     auto root = pdi->get_glui()->get_root();

    //     std::cout << "Calculating modal" << std::endl;
    //     open_file->calc_children_true_rects( root->get_true_rect(), &s);
    // } );

    return open_file;
}
