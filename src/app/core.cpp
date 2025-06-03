#include <PDI/core.hpp>
#include <PDI/blueprints.hpp>
#include <PDI/theme.hpp>
#include <GLUI/components/InputRange.hpp>
#include <GLUI/components/Dropdown.hpp>


void BP::handle( PDI *p, BP::Fn* f) {
    f->apply(p);
}

Element* label_field_horizontal( std::string label, Element* field ) {
    auto r = new Row(Size(Fill, FitContent), Alignment::Even, Alignment::Center);
    r->child(new Text(label, 20, Opacity(WHITE, .6)));
    r->child( field );
    return r;
}

Stylesheet GET_MY_SLIDER_HANDLE_STYLE (){
    return Stylesheet {
        .background_color = Theme::PRIMARY,
        .border = Border(2, Opacity(WHITE, .2), Corners(4)),
        .size = Size(10, 20)
    };
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

Element* BP::Brightness::build (PDI *pdi) {
    auto p    = new Pile(Rect(0, 0, LAYOUT_FILL, LAYOUT_FIT_CONTENT), 10, TRANSPARENT );
    p->set_padding(20);

    auto t_in = new InputRange( GET_MY_SLIDER_HANDLE_STYLE() );
    auto c_in = new InputRange( GET_MY_SLIDER_HANDLE_STYLE() );

    t_in->set_size( Size( 200, 20 ) );
    c_in->set_size( Size( 200, 20 ) );

    t_in->onchange([&, pdi](Input<float> &i) {
        brightness = *i.get_value();
        pdi->request_update();
    });
    t_in->set_value(brightness);
    c_in->onchange([&, pdi](Input<float> &i) {
        contrast = *i.get_value();
        pdi->request_update();
    });
    c_in->set_range( 0, 2 );
    c_in->set_value(contrast);

    p->child(label_field_horizontal( "Brilho",    t_in ));
    p->child(label_field_horizontal( "Contraste", c_in ));

    return p;


}

void BP::Invert::apply( PDI *pdi) const {

    pdi->get_pipeline()->push_shader( [&]( GLuint tex, GLShit* g, MultiPassFBO* m ) {
            
        std::cout << "Inverter" << std::endl; 
        shader_invert->use();

        m->apply(tex);
    });
}

Element* BP::Invert::build (PDI *pdi) {
    auto p    = new Pile(Rect(0, 0, LAYOUT_FILL, LAYOUT_FIT_CONTENT), 10, TRANSPARENT );
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

Element* BP::Threshold::build (PDI *pdi) {
    auto p    = new Pile(Rect(0, 0, LAYOUT_FILL, LAYOUT_FIT_CONTENT), 10, TRANSPARENT );
    p->set_padding(20);

    auto t_in = new InputRange( GET_MY_SLIDER_HANDLE_STYLE() );
    t_in->set_size( Size( 200, 20 ) );
    t_in->onchange([&, pdi](Input<float> &i) {
        threshold = *i.get_value();
        pdi->request_update();
    });
    t_in->set_value(threshold);

    auto r = label_field_horizontal( "Limiar", t_in );

    p->child(r);

    return p;

}

void BP::Greyscale::apply(PDI *pdi) const {

    std::cout << "Greyscale (" << R << ", " << G << ", " << B << ")" << std::endl;

    // check gpu
    pdi->get_pipeline()->push_shader([&](GLuint tex, GLShit *g, MultiPassFBO *m) {

        shader_greyscale->use();
        shader_greyscale->setFloat3("weights", R, G, B);

        m->apply(tex); 
    });
}

std::string ftos_l(float value, int precision) {
    char buffer[64];
    std::snprintf(buffer, sizeof(buffer), ("%." + std::to_string(precision) + "f").c_str(), value);
    return std::string(buffer);
}

Element* BP::Greyscale::build (PDI *pdi) {
    auto p = new Pile(Rect(0, 0, LAYOUT_FILL, LAYOUT_FIT_CONTENT), 10, TRANSPARENT );
    auto r = new Row(Rect(0, 0, LAYOUT_FILL, LAYOUT_FIT_CONTENT), 10, TRANSPARENT );

    auto pr = new Pile(Rect(0, 0, LAYOUT_FILL, LAYOUT_FIT_CONTENT), 10, TRANSPARENT );
    auto pg = new Pile(Rect(0, 0, LAYOUT_FILL, LAYOUT_FIT_CONTENT), 10, TRANSPARENT );
    auto pb = new Pile(Rect(0, 0, LAYOUT_FILL, LAYOUT_FIT_CONTENT), 10, TRANSPARENT );

    p->set_padding(20);
    p->child(new Text( "Greyscale" ));

    auto r_lb = new Text("R");
    auto r_in = new TextInput(Fill);
    auto g_lb = new Text("G");
    auto g_in = new TextInput(Fill);
    auto b_lb = new Text("B");
    auto b_in = new TextInput(Fill);

    r_in->set_value( ftos_l( R, 2 ) );
    g_in->set_value( ftos_l( G, 2 ) );
    b_in->set_value( ftos_l( B, 2 ) );

    r_in->onchange( [&, pdi](Input<std::string>& i){
        auto v = *i.get_value();
        R = std::stof( v.empty() ? "0" : v );
        pdi->request_update();
    });
    g_in->onchange( [&, pdi](Input<std::string>& i){
        auto v = *i.get_value();
        G = std::stof( v.empty() ? "0" : v );
        pdi->request_update();
    });
    b_in->onchange( [&, pdi](Input<std::string>& i){
        auto v = *i.get_value();
        B = std::stof( v.empty() ? "0" : v );
        pdi->request_update();
    });

    p->child( new Element(Rect( Size(LAYOUT_FILL, 1) ), RGBA(.4,.4,.4)) );

    pr->child( r_lb  );
    pr->child( r_in );
    pg->child( g_lb  );
    pg->child( g_in );
    pb->child( b_lb  );
    pb->child( b_in );

    r->child( pr );
    r->child( pg );
    r->child( pb );

    p->child(r);

    return p;

}


/**
 * ============================================================
 *                          FILTERS
 * ============================================================
 */

void BP::Filter::apply(PDI *pdi) const {

    pdi->get_pipeline()->push_shader([&](GLuint tex, GLShit *g, MultiPassFBO *m) {
        switch ( c ) {
            case LowPass::Median : {
                std::cout << "LowPassFilter (" << "Median" << ", " << kernel_size << ")" << std::endl;
                shader_median->use();
                shader_median->setInt("kernel_size", kernel_size);
                shader_median->setFloat2("texelSize", 1./(float)m->width, 1./(float)m->height );
                m->apply(tex); 
                return;
            }

            case LowPass::Gaussian : {
                std::cout << "LowPassFilter (" << "Gaussian" << ", " << kernel_size << ")" << std::endl;
                shader_convolution->use();
                shader_convolution->setFloat2("texelSize", 1./(float)m->width, 1./(float)m->height );
                m->apply(tex); 
                return;
            }

            default: {
                std::cout << "No convolution selected!" << std::endl;
                return;
            }
        }

    });
}

Element* BP::Filter::build (PDI *pdi) {

    std::cout << "Rendering Filter"  << std::endl;

    auto p = new Pile( Size(Fill, FitContent), 10, TRANSPARENT );

    p->set_padding(20);

    switch ( c ) {
    case LowPass::Median:
    {

        auto list = std::vector<Dropdown::Option>({
            Dropdown::Option{ .name="3x3", .value=3 },
            Dropdown::Option{ .name="5x5", .value=5 },
            Dropdown::Option{ .name="7x7", .value=7 },
        });
        auto s = new Selectbox( pdi->get_glui(), list );
        auto r = new Row(Size(Fill, FitContent), Alignment::Even, Alignment::Center);
        r->child(new Text("Kernel Size", 20, Opacity(WHITE, .6)));
        r->child( s );
        s->onchange([&, pdi](Input<int> &i) {
            kernel_size = *i.get_value();
            pdi->request_update(); 
        });

        p->child(r);
        break;
    }

    case LowPass::Gaussian:
    {
        p->child(new Text("Filtro Gaussiano"));
        break;
    }

    default:
    {
        p->child(new Text("Filtro Não Suportado"));
    }
    }


    return p;

}

void BP::Sobel::apply(PDI *pdi) const
{

    pdi->get_pipeline()->push_shader([&](GLuint tex, GLShit *g, MultiPassFBO *m){

        std::cout << "Sobel (" << "Limiar" << ", " << t << ")" << std::endl;
        shader_sobel->use();
        shader_sobel->setFloat2("texelSize", 1./(float)m->width, 1./(float)m->height );
        shader_sobel->setFloat("threshold", t);

        m->apply(tex); 
        return; 
    });
}

Element* BP::Sobel::build (PDI *pdi) {
    auto p    = new Pile(Rect(0, 0, LAYOUT_FILL, LAYOUT_FIT_CONTENT), 10, TRANSPARENT );
    p->set_padding(20);

    auto t_in = new InputRange( GET_MY_SLIDER_HANDLE_STYLE() );
    t_in->set_size( Size( 200, 20 ) );
    t_in->onchange([&, pdi](Input<float> &i) {
        t = *i.get_value();
        pdi->request_update();
    });
    t_in->set_value(t);

    auto r = label_field_horizontal( "Limiar", t_in );

    p->child(r);

    return p;

}


void BP::Robinson::apply(PDI *pdi) const
{

    pdi->get_pipeline()->push_shader([&](GLuint tex, GLShit *g, MultiPassFBO *m){

        std::cout << "Robinson (" << "Limiar" << ", " << t << ")" << std::endl;
        shader_sobel->use();
        shader_sobel->setFloat2("texelSize", 1./(float)m->width, 1./(float)m->height );
        shader_sobel->setFloat("threshold", t);

        m->apply(tex); 
        return; 
    });
}

Element* BP::Robinson::build (PDI *pdi) {
    auto p    = new Pile(Rect(0, 0, LAYOUT_FILL, LAYOUT_FIT_CONTENT), 10, TRANSPARENT );
    p->set_padding(20);

    auto t_in = new InputRange( GET_MY_SLIDER_HANDLE_STYLE() );
    t_in->set_size( Size( 200, 20 ) );
    t_in->onchange([&, pdi](Input<float> &i) {
        t = *i.get_value();
        pdi->request_update();
    });
    t_in->set_value(t);

    auto r = label_field_horizontal( "Limiar", t_in );

    p->child(r);

    return p;
}

/**
 * 
 * MORPHOLOGY
 * 
 * 
 * 
 */

void BP::Erosion::apply(PDI *pdi) const
{
    pdi->get_pipeline()->push_shader([&](GLuint tex, GLShit *g, MultiPassFBO *m){

        std::cout << "Erodir (" << "Type" << ", " << (k == MorphKernel::Cross ? "+" : "Sq" ) << ")" << std::endl;
        shader_morph_erosion->use();
        shader_morph_erosion->setFloat2("texelSize", 1./(float)m->width, 1./(float)m->height );
        shader_morph_erosion->setInt("kernel_type", (int)k);
        m->apply(tex); 
        return; 
    });
}

Element* BP::Erosion::build (PDI *pdi) {
    auto p    = new Pile(Rect(0, 0, LAYOUT_FILL, LAYOUT_FIT_CONTENT), 10, TRANSPARENT );
    // p->set_padding(20);
    // p->child(new Text("Erosão"));
    return p;
}

void BP::Dilation::apply(PDI *pdi) const
{
    pdi->get_pipeline()->push_shader([&](GLuint tex, GLShit *g, MultiPassFBO *m){

        std::cout << "Dilatar (" << "Type" << ", " << (k == MorphKernel::Cross ? "+" : "Sq" ) << ")" << std::endl;

        shader_morph_dilation->use();
        shader_morph_dilation->setFloat2("texelSize", 1./(float)m->width, 1./(float)m->height );
        shader_morph_dilation->setInt("kernel_type", (int)k);

        m->apply(tex); 
        return; 
    });
}

Element* BP::Dilation::build (PDI *pdi) {
    auto p    = new Pile(Rect(0, 0, LAYOUT_FILL, LAYOUT_FIT_CONTENT), 10, TRANSPARENT );
    // p->set_padding(20);
    // p->child(new Text("Dilatação"));
    return p;
}

void BP::Opening::apply(PDI *pdi) const
{
    pdi->get_pipeline()->push_shader([&](GLuint tex, GLShit *g, MultiPassFBO *m){

        std::cout << "Abertura[1/2] (" << "Type" << ", " << (k == MorphKernel::Cross ? "+" : "Sq" ) << ")" << std::endl;

        shader_morph_erosion->use();
        shader_morph_erosion->setFloat2("texelSize", 1./(float)m->width, 1./(float)m->height );
        shader_morph_erosion->setInt("kernel_type", (int)k);

        m->apply(tex); 
        return; 
    });

    pdi->get_pipeline()->push_shader([&](GLuint tex, GLShit *g, MultiPassFBO *m){

        std::cout << "Abertura[2/2] (" << "Type" << ", " << (k == MorphKernel::Cross ? "+" : "Sq" ) << ")" << std::endl;

        shader_morph_dilation->use();
        shader_morph_dilation->setFloat2("texelSize", 1./(float)m->width, 1./(float)m->height );
        shader_morph_dilation->setInt("kernel_type", (int)k);

        m->apply(tex); 
        return; 
    });
}

Element* BP::Opening::build (PDI *pdi) {
    auto p    = new Pile(Rect(0, 0, LAYOUT_FILL, LAYOUT_FIT_CONTENT), 10, TRANSPARENT );
    // p->set_padding(20);
    // p->child(new Text("Abertura"));
    return p;
}

void BP::Closing::apply(PDI *pdi) const
{
    pdi->get_pipeline()->push_shader([&](GLuint tex, GLShit *g, MultiPassFBO *m){
        std::cout << "Fechamento[1/2] (" << "Type" << ", " << (k == MorphKernel::Cross ? "+" : "Sq" ) << ")" << std::endl;

        shader_morph_dilation->use();
        shader_morph_dilation->setFloat2("texelSize", 1./(float)m->width, 1./(float)m->height );
        shader_morph_dilation->setInt("kernel_type", (int)k);

        m->apply(tex); 
        return; 
    });

    pdi->get_pipeline()->push_shader([&](GLuint tex, GLShit *g, MultiPassFBO *m){
        std::cout << "Fechamento[2/2] (" << "Type" << ", " << (k == MorphKernel::Cross ? "+" : "Sq" ) << ")" << std::endl;

        shader_morph_erosion->use();
        shader_morph_erosion->setFloat2("texelSize", 1./(float)m->width, 1./(float)m->height );
        shader_morph_erosion->setInt("kernel_type", (int)k);

        m->apply(tex); 
        return; 
    });

}

Element* BP::Closing::build (PDI *pdi) {
    auto p    = new Pile(Rect(0, 0, LAYOUT_FILL, LAYOUT_FIT_CONTENT), 10, TRANSPARENT );
    // p->set_padding(20);
    // p->child(new Text("Fechamento"));
    return p;
}

void BP::ThinningHolt::apply (PDI *pdi) const 
{
    for (int i = 0; i < iter; i++ ) {
        pdi->get_pipeline()->push_shader([&](GLuint tex, GLShit *g, MultiPassFBO *m){

            std::cout << "Holt (" << "Type" << ", " <<" HOLT" << ")" << std::endl;
            shader_thinner_holt->use();
            shader_thinner_holt->setFloat2("texelSize", 1./(float)m->width, 1./(float)m->height );
            shader_thinner_holt->setInt("subIter", 1);

            m->apply(tex); 
            return; 
        });
        pdi->get_pipeline()->push_shader([&](GLuint tex, GLShit *g, MultiPassFBO *m){

            std::cout << "Holt (" << "Type" << ", " <<" HOLT" << ")" << std::endl;
            shader_thinner_holt->use();
            shader_thinner_holt->setFloat2("texelSize", 1./(float)m->width, 1./(float)m->height );
            shader_thinner_holt->setInt("subIter", 2);

            m->apply(tex); 
            return; 
        });
    }
}

Element* BP::ThinningHolt::build (PDI *pdi) {
    auto p    = new Pile(Rect(0, 0, LAYOUT_FILL, LAYOUT_FIT_CONTENT), 10, TRANSPARENT );
    p->set_padding(20);

    auto t_in = new InputRange( GET_MY_SLIDER_HANDLE_STYLE() );
    t_in->set_size( Size( 200, 20 ) );
    t_in->set_range(1, 200);
    t_in->onchange([&, pdi](Input<float> &i) {
        float p_iter = *i.get_value();
        iter = (int)p_iter;
        pdi->request_update();
    });
    t_in->set_value(iter);

    auto r = label_field_horizontal( "Iterações", t_in );

    p->child(r);
    return p;
}
