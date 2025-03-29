#ifndef PDI_BLUEPRINTS_H
#define PDI_BLUEPRINTS_H

#include <iostream>
#include <shader.hpp>
#include <GLUI/layout.hpp>
#include <GLUI/text.hpp>
#include <GLUI/multipassfbo.hpp>

class PDI;

extern Shader* shader_brightness;
extern Shader* shader_threshold;
extern Shader* shader_greyscale;

/**
 *  ============================================
 *              Function Blueprints
 *  ============================================
 */
namespace BP {
    typedef __UINT8_TYPE__ id_t;
    typedef struct{ void* bp; BP::id_t tid; } tuple;

    class Fn {
    public:
        virtual void apply(PDI *pdi)  const {}
        virtual Element* render(PDI *pdi) {
            return new Text("Renderizador padrão");
        }
    };

    struct Translate { float x; float y; };
    struct Rotate    { float angle;  };
    struct Scale     { float factor; };

    class Brightness : public Fn {
        float brightness;
        float contrast;
    public:
        Brightness(float b, float c) : brightness(b), contrast(c) {}
        virtual void apply(PDI *pdi) const;
        virtual Element* render(PDI *pdi);
    };

    class Threshold : public Fn  {
        float threshold;
    public:
        Threshold(float t): threshold(t){}
        virtual void apply(PDI *pdi) const;
        virtual Element* render(PDI *pdi);
    };

    class Greyscale : public Fn {
        float f1;
        float f2;
        float f3;
    public:
        Greyscale(float f1, float f2, float f3): f1(f1),  f2(f2),  f3(f3){}
        virtual void apply(PDI *pdi) const;
        // virtual Element* render(PDI *pdi);
    };


    /**
     * 
     *  hmmMmmMMMm
     *  I smell shitty code
     * 
     */
    void handle( PDI *p, Fn* f);
}

#endif