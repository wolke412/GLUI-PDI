#include <PDI/core.hpp>
#include <PDI/blueprints.hpp>
#include <PDI/theme.hpp>


void BP::handle( PDI *p, BP::Fn* f) {
    f->apply(p);
}



/**
 *  ============================================================
 *                          BRIGHTNESS
 *  ============================================================
 */
void BP::Brightness::apply( PDI *pdi) const {

    std::cout << "Brightness (" << brightness << ")" << std::endl; 
    std::cout << "Contrast (" << contrast << ")" << std::endl; 
    
    pdi->get_pipeline()->push_shader( [&]( GLuint tex, GLShit* g, MultiPassFBO* m ) {
            
        shader_brightness->use();
        shader_brightness->setFloat("brightness", brightness);
        shader_brightness->setFloat("contrast", contrast);

        m->apply(tex);
    });
}

Element* BP::Brightness::render (PDI *pdi) {

    auto p = new Pile(Rect(0, 0, LAYOUT_FILL, LAYOUT_FIT_CONTENT), 10, Theme::BG_SHADE_100);
    p->set_padding(20);
    p->child(new Text( "Brilho e Contraste" ));

    auto b_lb = new Text("Brilho");
    auto b_in = new Input(LAYOUT_FILL);

    auto c_lb = new Text("Contraste");
    auto c_in = new Input(LAYOUT_FILL);

    b_in->set_value( std::to_string(brightness) );
    c_in->set_value( std::to_string(contrast  ) );

    b_in->onchange( [&, pdi](Input& i){
        float b = std::stof( *i.get_value() );
        brightness = b;
        pdi->update();
    });

    c_in->onchange( [&, pdi](Input& i){
        float c = std::stof( *i.get_value() );
        contrast = c;
        pdi->update();
    });

    p->child( b_lb );
    p->child( b_in );

    p->child( new Element(Rect( Size(LAYOUT_FILL, 1) ), RGB(.4,.4,.4)) );

    p->child( c_lb );
    p->child( c_in );

    return p;
}

void BP::Threshold::apply(PDI* pdi) const {
    std::cout << "Threshold (" << threshold << ")" << std::endl; 
    //check gpu
    pdi->get_pipeline()->push_shader( [&]( GLuint tex, GLShit* g, MultiPassFBO* m ) {
            
        shader_threshold->use();
        shader_threshold->setFloat("threshold", threshold);

        m->apply(tex);
    });
}


Element* BP::Threshold::render (PDI *pdi) {
    std::cout << "Rendering threshold"  << std::endl;
    auto p = new Pile(Rect(0, 0, LAYOUT_FILL, LAYOUT_FIT_CONTENT), 10, Theme::BG_SHADE_100);
    p->set_padding(20);
    p->child(new Text( "Threshold" ));

    auto t_lb = new Text("Threshol");
    auto t_in = new Input(LAYOUT_FILL);

    t_in->set_value( std::to_string(threshold) );

    t_in->onchange( [&, pdi](Input& i){
        float b = std::stof( *i.get_value() );
        threshold = b;
        pdi->update();
    });

    p->child( new Element(Rect( Size(LAYOUT_FILL, 1) ), RGB(.4,.4,.4)) );

    p->child( t_lb  );
    p->child( t_in );

    return p;

}

void BP::Greyscale::apply(PDI *pdi) const {

    std::cout << "Greyscale (" << f1 << ", " << f2 << ", " << f3 << ")" << std::endl;

    // check gpu
    pdi->get_pipeline()->push_shader([&](GLuint tex, GLShit *g, MultiPassFBO *m) {

        shader_greyscale->use();
        shader_greyscale->setFloat3("weights", f1, f2, f3);

        m->apply(tex); 
    });
}