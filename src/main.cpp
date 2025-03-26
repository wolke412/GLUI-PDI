#include "GLUI/draw.hpp"
#include <GLUI/glui.hpp>
#include <PDI/pdi.hpp>

#include <iostream>

#include <GLUI/benchmark.hpp>


int main()
{

    GLUI& glui = GLUI::create("Processamento Digital de Imagens");

    if ( ! glui.begin( Theme::BG_SHADE_100 ) ) {
        std::cerr << "Erro incializando GLUI." << std::endl;
        return -1;
    }

    PDI pdi(&glui);

    pdi.layout();

    glui.calc_elements();

    pdi.get_input()->set_path("static/512.jpg");
    pdi.get_input()->load();
    pdi.get_input()->copy_to(pdi.get_output());

    auto o = pdi.get_output();
    
#if USE_GPU == 1
    // use gpu
    o->set_is_framebuffer(true);
    o->generate_texture();
#endif

    auto win = glui.get_window_size();
    auto r = Rect(*pdi.get_input()->get_size() );

    glui.get_hotkeys()->registerhk('R', GLFW_MOD_SHIFT | GLFW_MOD_CONTROL, [&](){
        pdi.m_angle -= 5;
        pdi.transform();
    });
    glui.get_hotkeys()->registerhk('R', GLFW_MOD_CONTROL, [&](){
        pdi.m_angle += 5;
        pdi.transform();
    });
    glui.get_hotkeys()->registerhk(GLFW_KEY_UP, GLFW_MOD_CONTROL, [&](){
        pdi.m_translate_y += 5;
        pdi.transform();
    });
    glui.get_hotkeys()->registerhk(GLFW_KEY_DOWN, GLFW_MOD_CONTROL , [&](){
        pdi.m_translate_y -= 5;
        pdi.transform();
    });
    glui.get_hotkeys()->registerhk(GLFW_KEY_LEFT, GLFW_MOD_CONTROL , [&](){
        pdi.m_translate_x -= 5;
        pdi.update();
    });
    glui.get_hotkeys()->registerhk(GLFW_KEY_RIGHT, GLFW_MOD_CONTROL , [&](){
        pdi.m_translate_x += 5;
        pdi.transform();
    });
    glui.get_hotkeys()->registerhk(GLFW_KEY_M, GLFW_MOD_CONTROL , [&](){
        pdi.m_mirror_axis = (Axis) ( ( (int)pdi.m_mirror_axis + 1 ) % 4);
        pdi.transform();

    });
    glui.get_hotkeys()->registerhk('=', GLFW_MOD_CONTROL , [&](){
        pdi.m_scale_x += 0.05; 
        pdi.m_scale_y += 0.05; 

        pdi.transform();
    });
    glui.get_hotkeys()->registerhk('-', GLFW_MOD_CONTROL , [&](){
        pdi.m_scale_x -= 0.05; 
        pdi.m_scale_y -= 0.05; 

        if ( pdi.m_scale_x <= 0.01 ) {
            pdi.m_scale_x = 0.01;
            pdi.m_scale_y = 0.01;
        }

        pdi.transform();
    });
    

    // FILE BINDS
    // ============================================================ 
    glui.get_hotkeys()->registerhk(GLFW_KEY_S, GLFW_MOD_CONTROL , [&](){

        std::cout << "Getting bytes" << std::endl;

        auto o = pdi.get_output(); 

        o->read_generated_fbo();
    
        std::cout << "After read"  << std::endl;

        auto bin = o->get_binary();

        o->save( "static/save.jpg" );
    });

    Benchmark::monitor_gpu();

    while( ! glui.should_close() )
    {
        Benchmark::start();

        glui.loop_start();

        /**
         * 
         */
        glui.render();

        // Rect r( 60, 60, 160, 60 );
        // auto win = glui.get_window_size();
        // draw_rounded_quad(&r, RGB( .3, .32, .57 ) ,  {20, 20, 40, 40}, &win );

        // glFinish();

        Benchmark::capture();

        Benchmark::display(pdi.get_glui());

        glui.loop_end();
    }  

    glui.kill();

    return 0;
}
    
