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
extern Shader* shader_robinson;

extern Shader* shader_morph_dilation;
extern Shader* shader_morph_erosion ;

/**
 *  ============================================
 *              Function Blueprints
 *  ============================================
 */
namespace BP {
    typedef __UINT8_TYPE__ id_t;
    typedef struct{ void* bp; BP::id_t tid; } tuple;

    class Fn {
    private: 
        Element* built = nullptr;
    public:
        bool enabled = true;
        void set_built( Element* b ) {  built = b; }
        virtual void apply(PDI *pdi)  const {}
        void request_rerender(  ) { delete built; built = nullptr; }

        Element* render( PDI* pdi ) {
            if ( built == nullptr ) {
                set_built( build(pdi) );
            }
            return built;
        }
        virtual Element* build(PDI *pdi) {
            auto t = new Text("Renderizador padrão");
            // set_built(t);
            return t;
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
        virtual Element* build(PDI *pdi);
    };

    class Threshold : public Fn  {
        float threshold;
    public:
        Threshold(float t): threshold(t){}
        virtual void apply(PDI *pdi) const;
        virtual Element* build(PDI *pdi);
    };

    class Greyscale : public Fn {
        float R;
        float G;
        float B;
    public:
        Greyscale(float f1, float f2, float f3): R(f1),  G(f2),  B(f3){}
        virtual void apply(PDI *pdi) const;
        virtual Element* build(PDI *pdi);
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
        virtual Element* build(PDI *pdi);
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
        virtual Element* build(PDI *pdi);
    };

    class Robinson : public Fn {
        float t;
    public:
        Robinson( float t ): t(t) {}
        virtual void apply(PDI *pdi) const;
        virtual Element* build(PDI *pdi);
    };


    /**
     *  ============================================================ 
     *      MORPHOLOGY
     *  ============================================================ 
     */

    enum MorphKernel {
        Cross = 0,
        Square = 1
    };

    class Erosion : public Fn {
        MorphKernel k;
        float v = 0.05;
    public:
        Erosion( MorphKernel ke): k(ke) {}
        virtual void apply(PDI *pdi) const;
        virtual Element* build(PDI *pdi);
    };

    class Dilation : public Fn {
        MorphKernel k;
        float v = 0.05;
    public:
        Dilation( MorphKernel ke): k(ke) {}
        virtual void apply(PDI *pdi) const;
        virtual Element* build(PDI *pdi);
    };

    class Opening : public Fn {
        MorphKernel k;
        float v = 0.05;
    public:
        Opening( MorphKernel ke): k(ke) {}
        virtual void apply(PDI *pdi) const;
        virtual Element* build(PDI *pdi);
    };


    class Closing : public Fn {
        MorphKernel k;
        float v = 0.05;
    public:
        Closing( MorphKernel ke): k(ke) {}
        virtual void apply(PDI *pdi) const;
        virtual Element* build(PDI *pdi);
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