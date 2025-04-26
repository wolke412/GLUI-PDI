#include "GLUI/draw.hpp"
#include <GLUI/glui.hpp>
#include <PDI/pdi.hpp>

#include <iostream>

#include <GLUI/benchmark.hpp>

#include <GLUI/components/Modal.hpp>

int main()
{

    GLUI& glui = GLUI::create("Processamento Digital de Imagens");

    if ( ! glui.begin( (Theme::BG_SHADE_100).to_rgb() ) ) {
        std::cerr << "Erro incializando GLUI." << std::endl;
        return -1;
    }

    PDI pdi(&glui);

    pdi.layout();
    pdi.setup_hotkeys();

    glui.calc_elements();

    pdi.get_input()->set_path("static/noisy-lena.jpeg");
    pdi.get_input()->load();
    pdi.get_input()->copy_to(pdi.get_output());

    auto o = pdi.get_output();

    pdi.request_update(); 

#if USE_GPU == 1
    pdi.get_output()->set_is_framebuffer(true);
#endif

    Benchmark::monitor_gpu();

    while( ! glui.should_close() )
    {
        Benchmark::start();

        glui.loop_start();

        /**
         * updates pic if needed;
         */
        pdi.update();

        /**
         * 
         */
        glui.render();

        // Rect r ( 100, 300, 50, 100);
        // Rect r2( 100, 440, 200, 200);
        // Rect r3( 500, 300, 100, 400);

        // Border b ( 1, WHITE );
        // Border b2( 10, 10, 10, 0, WHITE );
        // Border b3( 0, 0, 0, 0, WHITE );
        // b.set_radius(10);
        // b2.set_radius({40, 40, 0, 0});
        // b3.set_radius({0, 10, 20, 30});

        // auto w = glui.get_window_size();

        // draw_rounded_quad( &r ,  WHITE, b.radius,  b.edges() , &w );
        // draw_rounded_quad( &r2,  WHITE, b2.radius, b2.edges(), &w );
        // draw_rounded_quad( &r3,  WHITE, b3.radius, b3.edges(), &w );


        // isso aqui é para propositos de teste apenas.
        // glFinish();

        Benchmark::capture();

        Benchmark::display(pdi.get_glui());

        glui.loop_end();
    }  

    glui.kill();

    return 0;
}
    
