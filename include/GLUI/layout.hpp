#ifndef LAYOUT_H
#define LAYOUT_H

#include <algorithm>
#include <cstdint>
#include <vector>
#include <functional>

#include "GLUI/colors.hpp"
#include "GLUI/rect.hpp"
#include "GLUI/draw.hpp"

#define LAYOUT_FILL         (-1)
#define LAYOUT_FIT_CONTENT  (-2)

enum LayoutMode {
    Fixed = 0,
    Auto
};


struct Padding {
    uint8_t top;
    uint8_t right;
    uint8_t bottom;
    uint8_t left;

    Padding( uint8_t all) : top(all), right(all), bottom(all), left(all) {}
    Padding( uint8_t t, uint8_t r, uint8_t b, uint8_t l) : 
        top(t), right(r), bottom(b), left(l) {}

    Padding( uint8_t v, uint8_t h) : 
        top(v), right(h), bottom(v), left(h) {}
};

struct ChildrenData {
    uint16_t max_width;
    uint16_t max_height;
    uint16_t total_width;
    uint16_t total_height;
};

enum ModifiableProperty {
    BgColor = 0,
    FgColor,
};

enum ElementCapabilities {
    CNone        = 0,
    CClickable   = 1 << 0,  
    CFocusable   = 1 << 1,  
    CHoverable   = 1 << 2 
};

class Element;
typedef std::function<void(Element*)> draw_callback_t;



class Element {
public:
    Rect rect;
    Padding padding = Padding(0);
    uint32_t id     = 0; 

    Element* m_parent = nullptr;

    draw_callback_t cb_before_draw = nullptr;

protected:
    uint32_t capabilities = 0;

    /**
     * 
     */
    Rect true_rect;

    LayoutMode lm;

    RGB bg_color = RGB(0,0,0);

    std::vector<Element*> children;

    bool hidden  = false;
    bool m_fixed = false;


public:
    Element() :              id(Element::getid()), lm(LayoutMode::Auto), rect(Size(LAYOUT_FILL)) {}
    Element(RGB c) :         id(Element::getid()), lm(LayoutMode::Auto), bg_color(c), rect(Rect()) {}
    Element(Size s, RGB c) : id(Element::getid()), lm(LayoutMode::Auto), rect(Rect(0, 0, s.width, s.height)), bg_color(c) {}
    Element(Rect r, RGB c) : id(Element::getid()), lm(LayoutMode::Fixed), rect(r), bg_color(c) {}

    static uint32_t getid() {
        static uint32_t c = 0;
        std::cout << "NEW ID GIVEN " << (c + 1) << std::endl;
        return ++c;
    }

    bool has_capability( ElementCapabilities c ) {
        return ( capabilities & c ) > 0;
    }

    void set_parent( Element *el ) {
        m_parent = el;
    }

    void child(Element* el)
    {
        children.push_back(el);

        el->set_parent(this);
    }
    void set_padding( Padding p ) {
        padding = p;
    }

    void set_background_color(RGB c) {
        bg_color = c;
    }
    void set_hover_background_color(RGB c) {
        bg_color = c;
    }
    
        
    std::vector<Element *> get_children() {
        return children;
    }

    void set_fixed(bool t) {
        m_fixed = t;
    }

    bool is_fixed() const {
        return m_fixed;
    }

    void toggle_hidden() {
        hidden = !hidden; 
    }
    void hide() {
        hidden = true; 
    }
    void show() {
        hidden = false;
    }

    bool is_hidden() const {
        return hidden;
    }

    RGB get_background_color() const {
        return bg_color;
    }


    /**
     * I dont like this shit very much...
     */
    void onbeforedraw(draw_callback_t cb) {
        cb_before_draw = cb;
    }

    /**
     * Ideally every one of them would be virtual
     */
    virtual void draw(Size *window)
    {

        if ( hidden ) {
            return;
        }

        // std::cout << id << "Drawn ID: " << id << " " << true_rect.width << ", " << true_rect.height << std::endl;
        draw_quad(true_rect, bg_color, window);

        draw_children(NULL, window);
    }

    void draw_children(Rect *parent, Size *window)
    {
        int child_count = children.size();
        // std::cout << "Drawing " << child_count << " children" << std::endl;

        for ( auto el : children)
        {
            if ( el->cb_before_draw ) {
                el->cb_before_draw(el);
            }

            el->draw(window);
        }
    }



    ChildrenData calc_children_data( Size* window) {
        auto c = ChildrenData{
            .max_width=0,
            .max_height=0,
            .total_width=0,
            .total_height=0
        };

        for (auto el : children)
        {
            auto r = el->rect;

            if ( el->rect.width == LAYOUT_FIT_CONTENT || el->rect.height == LAYOUT_FIT_CONTENT ) {
                // std::cerr << "LAYOUT_FIT_CONTENT CANNOT HAVE NON-FIXED CHILDREN." << std::endl;
                el->calc_fit_content_self( &r, window);
            }

            if ( r.height > c.max_height) {
                c.max_height = r.height; 
            }
            if ( r.width > c.max_width) {
                c.max_width = r.width;
            }

            c.total_width += r.width; 
            c.total_height += r.height; 
        }

       return c; 
    }

    virtual void calc_fit_content_self(Rect *current, Size *window )
    {

        auto c = calc_children_data(window);

        if (rect.width == LAYOUT_FIT_CONTENT)
        {
            current->width = c.total_width + padding.left + padding.right;
        }

        if (rect.height == LAYOUT_FIT_CONTENT)
        {
            current->height = c.max_height + padding.top + padding.bottom;
        }
    }

    virtual void calc_children_true_rects( Rect * parent, Size * window ) {

        // std::cout << "TRUE_RECT::DEFAULT ccount: " << children.size() <<  std::endl;

        auto tr = get_true_rect();

        for ( auto el : children) {

            Rect copy = el->rect;

            copy.x += padding.left + tr->x;
            copy.y += padding.top  + tr->y;

            if (el->rect.width == LAYOUT_FILL)
            {
                copy.width = parent->width - copy.x - padding.right;
            }
            else
            {
                copy.width = el->rect.width;
            }

            if (el->rect.height == LAYOUT_FILL)
            {
                copy.height = parent->height - copy.y - padding.bottom;
            }
            else
            {
                copy.height = el->rect.height;
            }

            /**
             *
             */
            if (el->rect.height == LAYOUT_FIT_CONTENT || el->rect.width == LAYOUT_FIT_CONTENT)
            {
                el->calc_fit_content_self(&copy, window );
            }

            // std::cout << "Settings w to " << copy.width << "| h to " << copy.height << std::endl;

            el->set_true_rect(copy);

            el->calc_children_true_rects(el->get_true_rect(), window);
        }
    };

    void set_rect( Rect r ) {
        rect = r;
    }

    Rect* get_true_rect () {
        return &true_rect;
    }

    void set_true_rect( Rect tr ) {
        // std::cout << "ST::TR" <<
        true_rect = tr;
    }

    uint8_t count_fillw_children()
    {

        uint8_t i = 0;

        for (auto el : children)
        {
            if ( el->rect.width == LAYOUT_FILL)
            {
                i++;
            }
        }

        return i;
    }

    uint8_t count_fillh_children()
    {

        uint8_t i = 0;

        for (auto el : children)
        {
            if (el->rect.height == LAYOUT_FILL)
            {
                i++;
            }
        }

        return i;
    }

    uint16_t get_fixed_width_children_sum()
    {
        uint16_t i = 0;

        for (auto el : children)
        {
            if (el->rect.width != LAYOUT_FILL)
            {
                i += el->rect.width;
            }
        }

        return i;
    }

    uint16_t get_fixed_height_children_sum()
    {
        uint16_t i = 0;

        for (auto el : children)
        {
            if (el->rect.width != LAYOUT_FILL)
            {
                i += el->rect.height;
            }
        }

        return i;
    }

    virtual ~Element() {
        for (Element* child : children) {
            if (child) { 
                delete child;
            }
        }
        children.clear(); 
    };
};



/**
 *
 *
 */
class Row : public Element
{
public:
    uint8_t gap;

    Row(Size s, uint8_t gap, RGB c) : gap(gap), Element(s, c)
    {
        lm = LayoutMode::Auto;
    }

    Row(Rect r, uint8_t gap, RGB c) : gap(gap), Element(r, c)
    {
        lm = LayoutMode::Auto;
    };

    virtual void calc_children_true_rects(Rect *parent, Size *window) override
    {
        // std::cout << "CCTR ::ROW" << std::endl;

        // --- Calc auto fill variables
        auto fw_count = count_fillw_children();

        auto total_gap = (children.size() - 1) * gap;
        auto total_padding = padding.right + padding.left;

        auto fw_size = true_rect.width - total_gap - get_fixed_width_children_sum() - total_padding;
        auto fw_p_el = fw_count ? fw_size / fw_count : 0;
        // ---

        int i = 0;
        int offsetX = padding.right;

        for (Element *el : children)
        {
            Rect copy = true_rect;
            // copy.x += parent->x;
            // copy.y += parent->y;

            copy.y += padding.top;
            copy.x += i * gap + offsetX;

            if (el->rect.width == LAYOUT_FILL)
            {
                copy.width = fw_p_el;
            }
            else
            {
                copy.width = el->rect.width;
            }

            if (el->rect.height == LAYOUT_FILL)
            {
                copy.height = parent->height - padding.top - padding.bottom;
            }
            else
            {
                copy.height = el->rect.height;
            }

            /**
             *
             */
            if (el->rect.height == LAYOUT_FIT_CONTENT || el->rect.width == LAYOUT_FIT_CONTENT)
            {
                el->calc_fit_content_self(&copy, window);
            }

            el->set_true_rect(copy);

            el->calc_children_true_rects(&true_rect, window);
            // el->draw( &copy, window);

            offsetX += copy.width;
            i++;
        }
    }
};


/**
 *
 *
 */
class Pile : public Element
{
public:
    uint8_t gap;

    Pile(Size s, uint8_t gap, RGB c) : gap(gap), Element(s, c)
    {
        lm = LayoutMode::Auto;
    }

    Pile(Rect r, uint8_t gap, RGB c) : gap(gap), Element(r, c)
    {
        lm = LayoutMode::Auto;
    };

    virtual void calc_fit_content_self( Rect* current, Size *window ) override {
        auto c = calc_children_data(window);

        if ( rect.width == LAYOUT_FIT_CONTENT ) {
            current->width = c.max_width + padding.left + padding.right; 
        }

        if ( rect.height == LAYOUT_FIT_CONTENT ) {
            current->height = c.total_height + padding.top + padding.bottom;
        }
    }

    virtual void calc_children_true_rects(Rect *parent, Size *window) override
    {
        // std::cout << "TRUE_RECT::DEFAULT ccount: " << children.size() <<  std::endl;

        // --- Calc auto fill variables
        auto fh_count = count_fillh_children();

        std::cout << "PILE::Calcing: " << std::to_string(children.size()) << "children.." << std::endl;

        auto total_gap     = (children.size() - 1) * gap;
        auto total_padding = padding.top + padding.bottom;

        auto fh_size = true_rect.height - total_gap - get_fixed_height_children_sum() - total_padding;
        auto fw_p_el = fh_count ? fh_size / fh_count : 0;
        // ---

        int i = 0;
        int offsetY = padding.top; 

        for (auto el : children)
        {
            Rect copy = true_rect;

            copy.x += padding.left;
            copy.y += i * gap + offsetY;

            if (el->rect.height == LAYOUT_FILL)
            {
                copy.height = fw_p_el;
            }
            else
            {
                copy.height = el->rect.height;
            }

            if (el->rect.width == LAYOUT_FILL)
            {
                copy.width = true_rect.width - total_padding;
            }
            else
            {
                copy.width = el->rect.width;
            }

            /**
             *
             */
            if (el->rect.height == LAYOUT_FIT_CONTENT || el->rect.width == LAYOUT_FIT_CONTENT)
            {
                // std::cout << "Calcing FIT_CONTENT" << std::endl;
                el->calc_fit_content_self(&copy, window);
            }

            std::cout << "Settings PILE ::id "<< id << "->w to " << copy.width << "| h to " << copy.height << std::endl;

            el->set_true_rect(copy);

            el->calc_children_true_rects(&true_rect, window);
            // el->draw( &copy, window);

            offsetY += copy.height;
            i++;
        }
    }
};


struct Border
{

    uint8_t top;
    uint8_t right;
    uint8_t bottom;
    uint8_t left;

    RGB color;
    Border(uint8_t all, RGB color) : top(all), right(all), bottom(all), left(all), color(color) {};
    Border(uint8_t t, uint8_t r, uint8_t b, uint8_t l, RGB color) : top(t), right(r), bottom(b), left(l), color(color) {};
};

#endif
