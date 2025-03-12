#include <GLUI/glui.hpp>
#include <PDI/pdi.hpp>

#include <iostream>

#include <GLUI/benchmark.hpp>


int main()
{

    GLUI& glui = GLUI::create("Processamento Digital de Imagens");

    if ( ! glui.begin() ) {
        std::cerr << "Erro incializando GLUI." << std::endl;
        return -1;
    }

    PDI pdi(&glui);

    pdi.layout();

    glui.calc_elements();

    while( ! glui.should_close() )
    {
        glui.loop_start();

        Benchmark::capture();

        Benchmark::start();
        /**
         * 
         */
        glui.render();

        Benchmark::display(pdi.get_glui());

        glui.loop_end();

    }  


    glui.kill();

    return 0;
}
    
