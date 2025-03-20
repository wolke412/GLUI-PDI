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
    // use gpu
    o->set_is_kernel_shader(true);
    o->set_kernel( glm::mat3(1.0) );
    o->generate_texture();

    auto win = glui.get_window_size();
    auto r = Rect(*pdi.get_input()->get_size() );

    glui.get_hotkeys()->registerHotkey('R', GLFW_MOD_SHIFT | GLFW_MOD_CONTROL, [&](){
        pdi.m_angle -= 5;
        pdi.transform();
    });
    glui.get_hotkeys()->registerHotkey('R', GLFW_MOD_CONTROL, [&](){
        pdi.m_angle += 5;
        pdi.transform();
    });
    glui.get_hotkeys()->registerHotkey(GLFW_KEY_UP, GLFW_MOD_CONTROL, [&](){
        pdi.m_translate_y += 5;
        pdi.transform();
    });
    glui.get_hotkeys()->registerHotkey(GLFW_KEY_DOWN, GLFW_MOD_CONTROL , [&](){
        pdi.m_translate_y -= 5;
        pdi.transform();
    });
    glui.get_hotkeys()->registerHotkey(GLFW_KEY_LEFT, GLFW_MOD_CONTROL , [&](){
        pdi.m_translate_x -= 5;
        pdi.transform();
    });
    glui.get_hotkeys()->registerHotkey(GLFW_KEY_RIGHT, GLFW_MOD_CONTROL , [&](){
        pdi.m_translate_x += 5;
        pdi.transform();
    });

    glui.get_hotkeys()->registerHotkey(GLFW_KEY_M, GLFW_MOD_CONTROL , [&](){
        pdi.m_mirror_axis = (Axis) ( ( (int)pdi.m_mirror_axis + 1 ) % 4);
        pdi.transform();
    });


    GLuint query;
    GLuint64 gpu_time;
    glGenQueries(1, &query);
    while( ! glui.should_close() )
    {
        Benchmark::start();
        glBeginQuery(GL_TIME_ELAPSED, query);

        glui.loop_start();

        Benchmark::capture();

        /**
         * 
         */
        glui.render();


        RenderText( "GPU time: " + std::to_string( Benchmark::to_unit( gpu_time, Unit::Micro ) ) , Coord(0, 60), .5, RGB(0, .8, .45), &win);

        Benchmark::display(pdi.get_glui());

        glFinish();

        glui.loop_end();

        glEndQuery(GL_TIME_ELAPSED);
        glGetQueryObjectui64v(query, GL_QUERY_RESULT, &gpu_time);

        // Rect r( 100, 200, 400, 400 );
        // for( int i = 0; i < 1000000; i++) {
        //     draw_quad( &r, WHITE, &win );
        // }


    }  


    glui.kill();

    return 0;
}
    
