#include <GLUI/benchmark.hpp>
#include "GLUI/glui.hpp"  

struct mallinfo Benchmark::m_info;
Benchmark::time_t Benchmark::m_start;
Benchmark::time_t Benchmark::m_stop;
Benchmark::time_t Benchmark::m_mark;

bool     Benchmark::m_monitor_gpu ;
uint64_t Benchmark::m_gpu_time; 
GLuint   Benchmark::m_gl_query;


void Benchmark::display(GLUI *glui)
{
    auto duration = duration_cast<microseconds>(Benchmark::m_stop - Benchmark::m_start);
    auto ms = duration.count() / 1000.0;
    long fps = 1000.0 / ms;

    auto window_size = glui->get_window_size();

    RenderText(std::to_string(static_cast<int>(fps)) + " FPS", Coord(0, 0), .5, RGBA(0, .8, .45), &window_size);
    RenderText(std::to_string(m_info.uordblks / 1024.0 / 1024.0) + " MB", Coord(60, 0), .5, RGBA(.8, .4, .45), &window_size);
    // RenderText(std::to_string( Benchmark::to_unit(m_gpu_time, Micro) ) + " us", Coord(220, 0), .5, RGBA(.6, .6, .25), &window_size);
}
