#ifndef PDI_CORE_H
#define PDI_CORE_H

#include <GLUI/glui.hpp>
#include <PDI/math.hpp>
#include <functional>
#include <variant>
// #include <>

class PDI;

/**
 *  =============================================
 *               Funtion Blueprints
 *  =============================================
 */
struct TranslateBP {
    float x;
    float y; 
    
    void apply(PDI* p) const { std::cout << "TRANSLATE (" << x << ", " << y << ")" << std::endl; }
};

struct RotateBP {
    float angle;
    void apply(PDI* p) const { std::cout << "ROTATE (" << angle << ")" << std::endl; }
};

struct ScaleBP {
    float factor;
    void apply(PDI *pdi) const { std::cout << "SCALE (" << factor << ")" << std::endl; }
};

using Stage = std::variant<TranslateBP, RotateBP, ScaleBP>; 

void handle_operation(PDI* pdi, const Stage& op);

class PDIPipeline {
    private:
        std::vector<Stage> stages;

    public:
        PDIPipeline () {}

        uint8_t push( Stage s ) {
            stages.push_back(s);
        }

        void run ( PDI* p ) {
            for ( auto s : stages ) handle_operation(p, s);
        }
};

/**
 *  =============================================
 *               o que interessa
 *  =============================================
 */
class PDI {
private:
    GLUI *glui;

    ImageHandler *input;
    ImageHandler *output;

    PDIPipeline pipeline;

public:

    // transformations
    float m_scale_x = 1, 
            m_scale_y = 1;
    float m_translate_x = 0, m_translate_y = 0;
    float m_angle = 0;
    Axis  m_mirror_axis = None;

    PDI( GLUI*g ): glui(g) {
        input  = new ImageHandler();
        output = new ImageHandler();
    };

    GLUI* get_glui() {
        return glui;
    }

    ImageHandler * get_input()  { return input; }
    ImageHandler * get_output() { return output; }

    void layout();

    // file menu
    void open_image();
    void save_image();
    std::string about();
    void exit();

    //
    void translate();
    void rotate();
    void scale();
    void mirror();

    void transform();
    // void transform( int t, float angle );


    // testing functions
    void reset_output();
    void test_pipe() {
        pipeline.push( TranslateBP{ 1.7, 4.} );
        pipeline.push( ScaleBP{ 1.7 } );
        pipeline.push( RotateBP{ 1.7 } );
        std::cout << "pipe done" << std::endl;
        pipeline.run( this );
    }
    void test_math();

    ~PDI() {
        input->free();
        output->free();
    }

};


#endif