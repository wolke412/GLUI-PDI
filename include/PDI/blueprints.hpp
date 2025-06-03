#ifndef PDI_BLUEPRINTS_H
#define PDI_BLUEPRINTS_H

#include <iostream>
#include <shader.hpp>
#include <GLUI/layout.hpp>
#include <GLUI/text.hpp>
#include <GLUI/multipassfbo.hpp>

class PDI;

extern Shader *shader_invert;
extern Shader *shader_brightness;
extern Shader *shader_threshold;
extern Shader *shader_greyscale;
extern Shader *shader_median;
extern Shader *shader_convolution;

extern Shader *shader_sobel;
extern Shader *shader_robinson;

extern Shader *shader_morph_dilation;
extern Shader *shader_morph_erosion;

extern Shader *shader_thinner_holt;

/**
 *  ============================================
 *              Function Blueprints
 *  ============================================
 */
namespace BP
{
    typedef __UINT8_TYPE__ id_t;
    typedef struct
    {
        void *bp;
        BP::id_t tid;
    } tuple;

    class Fn
    {
    private:
        float pass_time = 0.0;
        Element *built = nullptr;

    public:
        bool enabled = true;
        float get_pass_time() const { return pass_time; }
        void set_pass_time(float pt) { pass_time = pt; }
        void set_built(Element *b) { built = b; }
        virtual void apply(PDI *pdi) const {}
        void request_rerender()
        {
            delete built;
            built = nullptr;
        }
        virtual std::string get_title() const { return "Sem nome"; }

        Element *render(PDI *pdi)
        {
            if (built == nullptr)
            {
                set_built(build(pdi));
            }
            return built;
        }
        virtual Element *build(PDI *pdi)
        {
            auto t = new Text("Renderizador padrão");
            // set_built(t);
            return t;
        }

        void enable() { enabled = true; }
        void disable() { enabled = false; }
        void toggle() { enabled = !enabled; }
        bool is_enabled() const { return enabled; }
    };

    struct Translate
    {
        float x;
        float y;
    };
    struct Rotate
    {
        float angle;
    };
    struct Scale
    {
        float factor;
    };

    class Invert : public Fn
    {
    public:
        Invert() {}
        virtual std::string get_title() const { return "Inverter"; }
        virtual void apply(PDI *pdi) const;
        virtual Element *build(PDI *pdi);
    };

    class Brightness : public Fn
    {
        float brightness;
        float contrast;

    public:
        Brightness(float b, float c) : brightness(b), contrast(c) {}
        virtual std::string get_title() const { return "Brilho & Contraste"; }
        virtual void apply(PDI *pdi) const;
        virtual Element *build(PDI *pdi);
    };

    class Threshold : public Fn
    {
        float threshold;

    public:
        Threshold(float t) : threshold(t) {}
        virtual std::string get_title() const { return "Limiar"; }
        virtual void apply(PDI *pdi) const;
        virtual Element *build(PDI *pdi);
    };

    class Greyscale : public Fn
    {
        float R;
        float G;
        float B;

    public:
        Greyscale(float f1, float f2, float f3) : R(f1), G(f2), B(f3) {}
        virtual std::string get_title() const { return "Filtro de Cinza"; }
        virtual void apply(PDI *pdi) const;
        virtual Element *build(PDI *pdi);
    };

    /**
     *
     *
     *
     *
     */
    enum LowPass
    {
        Median,
        Gaussian
    };

    class Filter : public Fn
    {
        LowPass c;
        int kernel_size;

    public:
        Filter(LowPass c, int ks) : c(c), kernel_size(ks) {}
        virtual std::string get_title() const
        {
            return c == Median
                       ? "Filtro::Mediana"
                       : "Filtro::Gaussiano";
        }
        virtual void apply(PDI *pdi) const;
        virtual Element *build(PDI *pdi);
    };

    /**
     *
     *
     *
     *
     */
    enum EdgeDetection
    {
    };

    class Sobel : public Fn
    {
        float t;

    public:
        Sobel(float t) : t(t) {}
        virtual std::string get_title() const { return "Sobel"; }
        virtual void apply(PDI *pdi) const;
        virtual Element *build(PDI *pdi);
    };

    class Robinson : public Fn
    {
        float t;

    public:
        Robinson(float t) : t(t) {}
        virtual std::string get_title() const { return "Robinson"; }
        virtual void apply(PDI *pdi) const;
        virtual Element *build(PDI *pdi);
    };

    /**
     *  ============================================================
     *      MORPHOLOGY
     *  ============================================================
     */

    enum MorphKernel
    {
        Cross = 0,
        Square = 1
    };

    class Erosion : public Fn
    {
        MorphKernel k;
        float v = 0.05;

    public:
        Erosion(MorphKernel ke) : k(ke) {}
        virtual std::string get_title() const { return "Erosão"; }
        virtual void apply(PDI *pdi) const;
        virtual Element *build(PDI *pdi);
    };

    class Dilation : public Fn
    {
        MorphKernel k;
        float v = 0.05;

    public:
        Dilation(MorphKernel ke) : k(ke) {}
        virtual std::string get_title() const { return "Dilatação"; }
        virtual void apply(PDI *pdi) const;
        virtual Element *build(PDI *pdi);
    };

    /**
     * TODO: This does not work as expected::
     * Maybe change erosion and dilation to single shader and make
     * this an colsing a composed operation.
     */
    class Opening : public Fn
    {
        MorphKernel k;
        float v = 0.05;

    public:
        Opening(MorphKernel ke) : k(ke) {}
        virtual std::string get_title() const { return "Abertura"; }
        virtual void apply(PDI *pdi) const;
        virtual Element *build(PDI *pdi);
    };

    /**
     * TODO: Descripted in the class above ;)
     */
    class Closing : public Fn
    {
        MorphKernel k;
        float v = 0.05;

    public:
        Closing(MorphKernel ke) : k(ke) {}
        virtual std::string get_title() const { return "Fechamento"; }
        virtual void apply(PDI *pdi) const;
        virtual Element* build(PDI *pdi);
    };

    class ThinningHolt : public Fn
    {
    public:
        int iter = 1;
        ThinningHolt(int t ) : iter(t){ }
        virtual std::string get_title() const { return "Afinamento (Holt)"; }
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
