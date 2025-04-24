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

        // isso aqui é para propositos de teste apenas.
        // glFinish();

        Benchmark::capture();

        Benchmark::display(pdi.get_glui());

        glui.loop_end();
    }  

    glui.kill();

    return 0;
}
    
