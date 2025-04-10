#include <PDI/blueprints.hpp>
#include <PDI/core.hpp>
#include <vector>


// variant is a full of shit solution

// using Stage = std::variant<
//     // TranslateBP, RotateBP, ScaleBP, 
//     BrightnessBP, GreyscaleBP, ThresholdBP
// >; 

// void handle_operation(PDI* pdi, const Stage& op);

// using Stage = BP::tuple;

using Stage = BP::Fn*;

/**
 * ============================================================
 *      THIS IS THE CALL FOR HANDLING PIPELINE *DATA*!!!!
 * ============================================================
 */
class PDIPipeline
{
private:
    std::vector<Stage> stages;

#if USE_GPU
    std::vector<FnShader> shaders;
#endif

public:
    PDIPipeline() {}

    void push(Stage s)
    {
        stages.push_back(s);
        // std::cout << "Total size : " << stages.size() << std::endl;
    }

    int size() { return stages.size(); }
    int active_size() {
        int c = 0;
        for( auto s : stages ) {
            if (s->is_enabled()) c++;
        }
        return c; 
    }

    bool swap( size_t from, size_t to ) {
        if (from >= stages.size() || to >= stages.size())
        {
            return false;
        }

        auto temp = stages[from];
        stages[from] = stages[to];
    }

    void remove( int idx ) {
        stages.erase( stages.begin() + idx );
    }

    void clear() { 
        for( auto s : stages ) {
            delete s;
        }

        stages.clear();
    }

    std::vector<Stage> get_stages() {
        return stages;
    }

    void run(PDI *p)
    {
        for (auto s : stages) {
            if ( s->is_enabled() ) {
                BP::handle(p, s);
                int count = 49;
            }
        }
    }

#if USE_GPU
    void push_shader(FnShader s){ shaders.push_back(s); }
    std::vector<FnShader> *get_shaders()
    {
        return &shaders;
    }
    void flush_shaders()
    {
        shaders.clear();
    }
#endif
};