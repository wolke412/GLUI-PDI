#ifndef PDI_CORE_H 
#define PDI_CORE_H

/**
 *  ============================================================ 
 *    THIS DEFINES WETHER TO USE GPU OR CPU TO CALCULATE IMAGE
 *  ============================================================ 
 */
#define USE_GPU         ( 1 )

#pragma once

#include <GLUI/glui.hpp>
#include <PDI/math.hpp>
#include <functional>
#include <variant>
#include <PDI/bppipeline.hpp>

#if USE_GPU 
#include <GLUI/framebuffer.hpp>
#include <GLUI/multipassfbo.hpp>
#endif

class PDI;


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
    Element* render_pipeline;

    /**
     * ============================================================
     *   Transformations
     * ============================================================
     */
    float m_scale_x = 1, 
          m_scale_y = 1;
    float m_translate_x = 0, 
          m_translate_y = 0;
    float m_angle = 0;
    Axis  m_mirror_axis = None;
    /**
     * ============================================================
     */

    PDI( GLUI*g ): glui(g) {
        input  = new ImageHandler();
        output = new ImageHandler();

        load_shaders();
    };

    GLUI* get_glui() {
        return glui;
    }

    PDIPipeline* get_pipeline() {
        return &pipeline;
    }

    ImageHandler * get_input()  { return input; }
    ImageHandler * get_output() { return output; }

    void update_pipeline();

    void push_stage( Stage s );
    void remove_stage( size_t at );
    void swap_positions( size_t i, size_t j );

    void load_shaders();

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


    mat get_transform_kernel();
    void reset_transform();

    void transform();
    void update();


    // testing functions
    void reset_output();
    void test_math();

    void layout_pipeline_components();
    void layout();
    void setup_hotkeys();

    ~PDI() {
        input->free();
        output->free();
    }

};


#endif
