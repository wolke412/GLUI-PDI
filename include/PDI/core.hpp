#ifndef PDI_CORE_H
#define PDI_CORE_H

#include <GLUI/glui.hpp>

class PDI {
    private:
        GLUI* glui;

        ImageHandler* input;
        ImageHandler* output;

        std::vector<int> pipeline;
    public:

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

    void open_image();
    void save_image();
    std::string about();
    void exit();

    void reset_output();

    void apply();

    ~PDI() {
        input->free();
        output->free();
    }

};


#endif