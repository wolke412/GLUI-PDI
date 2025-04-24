#include <PDI/core.hpp>
#include <PDI/blueprints.hpp>
#include <PDI/theme.hpp>
#include <GLUI/components/InputRange.hpp>


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

Element* BP::Brightness::build (PDI *pdi) {

    auto p = new Pile(Rect(0, 0, LAYOUT_FILL, LAYOUT_FIT_CONTENT), 10, TRANSPARENT);

    p->set_padding(20);
    p->child(new Text( "Brilho e Contraste" ));

    auto b_lb = new Text("Brilho");
    auto b_in = new TextInput(LAYOUT_FILL);

    auto c_lb = new Text("Contraste");
    auto c_in = new TextInput(LAYOUT_FILL);

    b_in->set_value( std::to_string(brightness) );
    c_in->set_value( std::to_string(contrast  ) );

    b_in->onchange( [&, pdi](Input<std::string>& i){
        float b = std::stof( *i.get_value() );
        brightness = b;
        pdi->request_update();
    });

    c_in->onchange( [&, pdi](Input<std::string>& i){
        float c = std::stof( *i.get_value() );
        contrast = c;
        pdi->request_update();
    });

    p->child( b_lb );
    p->child( b_in );

    p->child( new Element(Rect( Size(LAYOUT_FILL, 1) ), RGBA(.5,.4,.4)) );

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


Element* BP::Threshold::build (PDI *pdi) {
    std::cout << "Rendering threshold"  << std::endl;
    auto p = new Pile(Rect(0, 0, LAYOUT_FILL, LAYOUT_FIT_CONTENT), 10, TRANSPARENT );
    p->set_padding(20);
    p->child(new Text( "Threshold" ));

    auto t_lb = new Text("Threshold");
    auto t_in = new TextInput(LAYOUT_FILL);

    t_in->set_value( std::to_string(threshold) );

    t_in->onchange( [&, pdi](Input<std::string>& i){
        float b = std::stof( *i.get_value() );
        threshold = b;
        pdi->request_update();
    });

    p->child( new Element(Rect( Size(LAYOUT_FILL, 1) ), RGBA(.4,.4,.4)) );

    p->child( t_lb  );
    p->child( t_in );

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
    auto p = new Pile(Rect(0, 0, LAYOUT_FILL, LAYOUT_FIT_CONTENT), 10, TRANSPARENT );
    p->set_padding(20);

    switch ( c ) {
    case LowPass::Median:
    {
        p->child(new Text("Filtro Mediana"));

        auto t_lb = new Text("Kernel Size");
        auto t_in = new TextInput(LAYOUT_FILL);
        t_in->set_value(std::to_string(kernel_size));
        t_in->onchange([&, pdi](Input<std::string> &i)
                       {
                    auto v = *i.get_value();
                    v = v.empty() ? "0" : v;

                    int b = std::stof( v );

                    kernel_size = b;
                    pdi->request_update(); });

        p->child(new Element(Rect(Size(LAYOUT_FILL, 1)), RGBA(.4, .4, .4)));

        p->child(t_lb);
        p->child(t_in);
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
    auto t_lb = new Text("Threshold");
    auto t_in = new InputRange( Size(Fill, FitContent), TRANSPARENT );

    p->set_padding(20);
    p->child(new Text("Filtro de Sobel"));

    t_in->onchange([&, pdi](Input<float> &i) {
        t = *i.get_value();
        pdi->request_update();
    });

    p->child(new Element(Rect(Size(LAYOUT_FILL, 1)), RGBA(.4, .4, .4)));

    p->child(t_lb);
    p->child(t_in);

    t_in->set_value(t);
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
    auto t_lb = new Text("Threshold");
    auto t_in = new InputRange( Size(Fill, FitContent), TRANSPARENT );

    p->set_padding(20);
    p->child(new Text("Filtro de Robinson"));

    t_in->onchange([&, pdi](Input<float> &i) {
        t = *i.get_value();
        pdi->request_update();
    });

    p->child(new Element(Rect(Size(LAYOUT_FILL, 1)), RGBA(.4, .4, .4)));

    p->child(t_lb);
    p->child(t_in);

    t_in->set_value(t);
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
    p->set_padding(20);
    p->child(new Text("Erosão"));
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
    p->set_padding(20);
    p->child(new Text("Dilatação"));
    return p;
}

void BP::Opening::apply(PDI *pdi) const
{
    pdi->get_pipeline()->push_shader([&](GLuint tex, GLShit *g, MultiPassFBO *m){

        std::cout << "Abertura (" << "Type" << ", " << (k == MorphKernel::Cross ? "+" : "Sq" ) << ")" << std::endl;

        shader_morph_erosion->use();
        shader_morph_erosion->setFloat2("texelSize", 1./(float)m->width, 1./(float)m->height );
        shader_morph_erosion->setInt("kernel_type", (int)k);
        shader_morph_dilation->use();
        shader_morph_dilation->setFloat2("texelSize", 1./(float)m->width, 1./(float)m->height );
        shader_morph_dilation->setInt("kernel_type", (int)k);

        m->apply(tex); 
        return; 
    });
}

Element* BP::Opening::build (PDI *pdi) {
    auto p    = new Pile(Rect(0, 0, LAYOUT_FILL, LAYOUT_FIT_CONTENT), 10, TRANSPARENT );
    p->set_padding(20);
    p->child(new Text("Abertura"));
    return p;
}

void BP::Closing::apply(PDI *pdi) const
{
    pdi->get_pipeline()->push_shader([&](GLuint tex, GLShit *g, MultiPassFBO *m){

        std::cout << "Fechamento (" << "Type" << ", " << (k == MorphKernel::Cross ? "+" : "Sq" ) << ")" << std::endl;

        shader_morph_dilation->use();
        shader_morph_dilation->setFloat2("texelSize", 1./(float)m->width, 1./(float)m->height );
        shader_morph_dilation->setInt("kernel_type", (int)k);
        shader_morph_erosion->use();
        shader_morph_erosion->setFloat2("texelSize", 1./(float)m->width, 1./(float)m->height );
        shader_morph_erosion->setInt("kernel_type", (int)k);

        m->apply(tex); 
        return; 
    });
}

Element* BP::Closing::build (PDI *pdi) {
    auto p    = new Pile(Rect(0, 0, LAYOUT_FILL, LAYOUT_FIT_CONTENT), 10, TRANSPARENT );
    p->set_padding(20);
    p->child(new Text("Fechamento"));
    return p;
}
