#ifndef GLUI_BENCHMARK_H
#define GLUI_BENCHMARK_H

// GLUI does not depends on this
// it will be treated as a ADD'on
// #include <GLUI/glui.hpp>
class GLUI;



#include <chrono>
#include <malloc.h>

using namespace std::chrono;

enum Unit {
    Nano = 0,
    Micro, 
    Milli,

    Sec,
    Min,
};

class Benchmark
{
public:
    using time_t = std::chrono::_V2::system_clock::time_point; 

    static struct mallinfo m_info;
    static time_t m_start;
    static time_t m_stop;

    static time_t m_mark;

    static void start()
    {
        Benchmark::m_start = high_resolution_clock::now();
    }

    static void mark()
    {
        Benchmark::m_mark = high_resolution_clock::now();
    }

    static int since_mark( Unit u )
    {
        auto since =  high_resolution_clock::now() - m_mark;

        auto t = since.count();

        return to_unit(t, u);
    }

    static void capture()
    {
        Benchmark::m_stop = high_resolution_clock::now();
        Benchmark::m_info = mallinfo();
    }

    static uint64_t to_unit( int64_t t, Unit u  ) {
        switch (u) {
            case Min:   t /= 60;
            case Sec:   t /= 1000;
            case Milli: t /= 1000;
            case Micro: t /= 1000;
            case Nano:  
            default: return t;
        }
    }

    static void display(GLUI *glui);
};

#endif