#include <GLUI/benchmark.hpp>
#include "GLUI/glui.hpp"  

struct mallinfo Benchmark::m_info;
Benchmark::time_t Benchmark::m_start;
Benchmark::time_t Benchmark::m_stop;
Benchmark::time_t Benchmark::m_mark;

void Benchmark::display(GLUI *glui)
{
    auto duration = duration_cast<microseconds>(Benchmark::m_stop - Benchmark::m_start);
    auto ms = duration.count() / 1000.0;
    long fps = 1000.0 / ms;

    auto window_size = glui->get_window_size();

    RenderText(std::to_string(static_cast<int>(fps)) + " FPS", Coord(0, 0), .5, RGB(0, .8, .45), &window_size);
    RenderText(std::to_string(m_info.uordblks / 1024.0 / 1024.0) + " MB", Coord(60, 0), .5, RGB(.8, .4, .45), &window_size);
}