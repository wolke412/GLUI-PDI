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
extern Shader* shader_median;
extern Shader* shader_convolution;

extern Shader* shader_sobel;

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
        bool enabled = true;
        virtual void apply(PDI *pdi)  const {}
        virtual Element* render(PDI *pdi) {
            return new Text("Renderizador padrão");
        }

        void enable() { enabled = true; }
        void disable() { enabled = false; }
        void toggle() { enabled = ! enabled; }
        bool is_enabled() const { return enabled; }
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
        float R;
        float G;
        float B;
    public:
        Greyscale(float f1, float f2, float f3): R(f1),  G(f2),  B(f3){}
        virtual void apply(PDI *pdi) const;
        virtual Element* render(PDI *pdi);
    };

    /**
     * 
     * 
     * 
     * 
     */
    enum LowPass {
        Median,
        Gaussian
    };

    class Filter : public Fn {
        LowPass c;
        int kernel_size;
    public:
        Filter(LowPass c, int ks): c(c), kernel_size(ks){}
        virtual void apply(PDI *pdi) const;
        virtual Element* render(PDI *pdi);
    };

    /**
     * 
     * 
     * 
     * 
     */
    enum EdgeDetection {};

    class Sobel : public Fn {
        float t;
    public:
        Sobel( float t ): t(t) {}
        virtual void apply(PDI *pdi) const;
        virtual Element* render(PDI *pdi);
    };

    class Sobel : public Fn {
        float t;
    public:
        Sobel( float t ): t(t) {}
        virtual void apply(PDI *pdi) const;
        virtual Element* render(PDI *pdi);
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