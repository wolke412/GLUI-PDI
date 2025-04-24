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


enum Spacing : int {
    Tiny       = 4,
    Small      = 8,
    SmallM     = 12,
    MediumS    = 16,
    Medium     = 20,
    MediumL    = 24,
    Large      = 28,
    LargeX     = 32,
};

enum FontSize : int {
    S_100      = 48, 
    S_200      = 44, 
    S_300      = 40, 
    S_400      = 36, 
    S_500      = 24, 
    S_600      = 20, 
    S_700      = 16, 
    S_800      = 12, 
    S_900      =  8, 
};

enum Layout : int {
    Fill       = -1,
    FitContent = -2
};

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

    Padding( uint8_t v, uint8_t h ) : 
        top(v), right(h), bottom(v), left(h) {}

};

struct Edges {
    uint8_t top, right, bottom, left;
};

struct Corners {
    uint8_t topright;
    uint8_t topleft;
    uint8_t bottomleft;
    uint8_t bottomright;
};

struct Border
{
    uint8_t top;
    uint8_t right;
    uint8_t bottom;
    uint8_t left;

    Corners radius;
    RGBA color;
    Border(uint8_t all, RGBA color) : top(all), right(all), bottom(all), left(all), color(color) {};
    Border(uint8_t t, uint8_t r, uint8_t b, uint8_t l, RGBA color) : top(t), right(r), bottom(b), left(l), color(color) {};

    bool exists() {
        return ( top + right + left + bottom ) > 0;
    }

    void set_radius( uint8_t r ) {
        radius.bottomleft  = r;
        radius.bottomright = r;
        radius.topleft     = r;
        radius.topright    = r;
    } 

    void set_radius( Corners r ) {
        radius = r;
    }

    RGBA get_color() { return color; }
    void set_color( RGBA c ) {
        color = c;
    }
};

enum Alignment {
    Start,
    Center,
    End,
    Even
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
    CHoverable   = 1 << 2 ,
    CToggable    = 1 << 3 ,
    CDraggable   = 1 << 4 ,
};


class Element;
typedef std::function<void(Element*)> draw_callback_t;

class Element {
public:
    Rect rect;

    RGBA bg_color = BLACK;
    RGBA fg_color = WHITE;

    Padding padding = Padding(0);
    Border border   = Border(0, BLACK);

    uint32_t id     = 0; 

    Element* m_parent = nullptr;

    draw_callback_t cb_before_draw = nullptr;

protected:

    // GLShit gl;
    uint32_t capabilities = 0;

    /**
     * 
     */
    Rect true_rect;

    LayoutMode lm;


    std::vector<Element*> children;

    bool hidden  = false;
    bool m_fixed = false;

public:
    explicit Element(RGBA c) :         id(Element::getid()), lm(LayoutMode::Auto), bg_color(c), rect(Rect()) {}
    explicit Element(Size s, RGBA c) : id(Element::getid()), lm(LayoutMode::Auto), rect(Rect(0, 0, s.width, s.height)), bg_color(c) {}
    explicit Element(Rect r, RGBA c) : id(Element::getid()), lm(LayoutMode::Fixed), rect(r), bg_color(c) {}
    explicit Element() :               id(Element::getid()), lm(LayoutMode::Auto), rect(Size(LAYOUT_FILL)) {
        std::cout <<  "CREATING FROM EMPTY CONSTRUCTOR" << std::endl;
    }

    static uint32_t getid() {
        static uint32_t c = 0;
        // std::cout << "NEW ID GIVEN " << (c + 1) << std::endl;
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

    void set_border ( Border b ) {
        border = b;
    }

    Border* get_border () {
        return &border;
    }

    void set_background_color(RGBA c) {
        bg_color = c;
    }

    void set_foreground_color(RGBA c) {
        fg_color = c;
    }

    std::vector<Element *> get_children() {
        return children;
    }

    void drop_children() {
        for (Element* child : children) {
            // delete child;
        }
        children.clear();
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
    void toggle_hidden(bool stt) {
        hidden = stt;
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

    RGBA get_background_color() const {
        return bg_color;
    }

    Size get_reserved_size() const {
        static Size sz = Size(0, 0);

        sz.width  = padding.left + padding.right + border.left + border.right;
        sz.height = padding.bottom + padding.top + border.top + border.bottom;

        return sz;
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

        draw_quad(true_rect, bg_color, window);

        draw_border( window );

        draw_children(NULL, window);
    }

    void draw_border( Size *window ) {
        auto tr = true_rect;

        auto top = Rect( tr.x, tr.y, tr.width, border.top );
        auto bot = Rect( tr.x, tr.y+tr.height - border.bottom, tr.width, border.bottom);
        auto rig = Rect( tr.x, tr.y, border.left, tr.height );
        auto lef = Rect( tr.x + tr.width - border.left, tr.y, border.right, tr.height );

        draw_quad(&top, border.color, window);
        draw_quad(&bot, border.color, window);
        draw_quad(&rig, border.color, window);
        draw_quad(&lef, border.color, window);
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

    /**
     * TODO: this SHOULD grab true_rect, it must remais agnostic of child methods
     * wherever the true_rect is being calculated it must remain there.
     */
    ChildrenData calc_children_data( Size* window ) {
        auto c = ChildrenData{
            .max_width=0,
            .max_height=0,
            .total_width=0,
            .total_height=0
        };

        for (auto el : children)
        {
            auto r = el->get_true_rect();

            if ( el->rect.width == Layout::FitContent || el->rect.height == Layout::FitContent) {
                el->calc_fit_content_self(r, window);
            }

            if ( r->height > c.max_height) {
                c.max_height = r->height; 
            }

            if ( r->width > c.max_width) {
                c.max_width = r->width;
            }

            c.total_width  += r->width; 
            c.total_height += r->height; 

            // r->debug();
            // std::cout << "TW = " << c.total_width  << std::endl;
            // std::cout << "TH = " << c.total_height << std::endl;
        }

       return c; 
    }

    virtual void calc_fit_content_self( Rect* current, Size *window )
    {
        auto c = calc_children_data(window);
        auto r = get_reserved_size();
        
        if (rect.width == LAYOUT_FIT_CONTENT)
        {
            current->width = c.total_width + r.width;
        }

        if (rect.height == LAYOUT_FIT_CONTENT)
        {
            current->height = c.max_height + r.height;
        }
    }

    virtual void calc_children_true_rects( Rect * parent, Size * window ) {

        auto tr      = get_true_rect();
        auto content = get_content_rect();

        for ( auto el : children ) {
            Rect copy = el->rect;
        
            float left_offset   = padding.left + border.left;
            float right_offset  = padding.right + border.right;
            float top_offset    = padding.top + border.top;
            float bottom_offset = padding.bottom + border.bottom;
        
            copy.x += left_offset + tr->x;
            copy.y += top_offset  + tr->y;
        
            if (el->rect.width == LAYOUT_FILL){
                copy.width = tr->width - left_offset - right_offset;
            }
        
            if (el->rect.height == LAYOUT_FILL){
                copy.height = tr->height - top_offset - bottom_offset;
            }

            if (el->rect.height == LAYOUT_FIT_CONTENT || el->rect.width == LAYOUT_FIT_CONTENT)
            {
                el->calc_fit_content_self(&copy, window );
            }

            // std::cout << "Settings w to " << copy.width << "| h to " << copy.height << std::endl;

            el->set_true_rect(copy);

            el->calc_children_true_rects( &copy, window);
        }
    };

    void set_rect( Rect r ) {
        rect = r;
    }
    void set_position( Coord c ) {
        rect.x = c.x;
        rect.y = c.y;
    }
    void set_size( Size s ) {
        rect.width  = s.width;
        rect.height = s.height;
    }

    Rect* get_true_rect () {
        return &true_rect;
    }

    Rect* get_rect () {
        return &rect;
    }

    void set_true_rect( Rect tr ) {
        // std::cout << "ST::TR" <<
        // std::cout << "TR ::id" << id << ": (" << tr.width << ", " << tr.height << ")" << std::endl;
        true_rect = tr;
    }

    Rect get_content_rect() {
        auto r = get_reserved_size();
        return Rect (
            0, 0,
            true_rect.width - r.width, true_rect.height - r.height 
        );
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
            if (el->rect.height != LAYOUT_FILL)
            {
                i += el->true_rect.height;
            }

            // TODO: this has a stupid bug
            // if ( el->rect.height == Layout::FitContent ) {
            //    i += el->true_rect.height; 
            // }
        }

        return i;
    }

    Coord to_local( Coord c ) {
        auto p = get_true_rect()->get_pos();
        return c.difference(&p);
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

    Row(Size s, uint8_t gap, RGBA c) : gap(gap), Element(s, c) { lm = LayoutMode::Auto; }
    Row(Rect r, uint8_t gap, RGBA c) : gap(gap), Element(r, c) { lm = LayoutMode::Auto; };

    virtual void calc_fit_content_self( Rect* current, Size *window ) override {
        auto c = calc_children_data(window);
        auto reserved = get_reserved_size();

        if ( rect.width == LAYOUT_FIT_CONTENT ) {
            auto total_gap = (children.size() - 1) * gap;
            current->width = c.total_width + reserved.width + total_gap;
        }

        if ( rect.height == LAYOUT_FIT_CONTENT ) {
            current->height = c.max_height + reserved.height;
        }
    }


    virtual void calc_children_true_rects(Rect *parent, Size *window) override
    {

        auto reserved = get_reserved_size();

        // --- Calc auto fill variables
        auto fw_count = count_fillw_children();

        auto total_gap = (children.size() - 1) * gap;
        auto total_padding = reserved.width;

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

            copy.y += padding.top + border.top;
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
                copy.height = parent->height - reserved.height;
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

    // this might cause template bloating with used too largely
    template<typename... Elements>
    Pile(Elements*... elems) {
        (child(elems), ...); // folds appending children
    }

    Pile(Size s, uint8_t gap, RGBA c) : gap(gap), Element(s, c){}
    Pile(Rect r, uint8_t gap, RGBA c) : gap(gap), Element(r, c){}

    virtual void calc_fit_content_self( Rect* current, Size *window ) override {

        auto c        = calc_children_data(window);
        auto reserved = get_reserved_size();

        if ( rect.width == LAYOUT_FIT_CONTENT ) {
            current->width = c.max_width + reserved.width;
        }

        if ( rect.height == LAYOUT_FIT_CONTENT ) {
            auto total_gap = (children.size() - 1) * gap;
            current->height = c.total_height + reserved.height + total_gap;
        }
    }

    virtual void calc_children_true_rects(Rect *parent, Size *window) override
    {
        auto reserved = get_reserved_size();

        // --- Calc auto fill variables
        auto fh_count = count_fillh_children();

        auto total_gap     = (children.size() - 1) * gap;
        auto total_padding = reserved.height;

        auto fh_size = true_rect.height - total_gap - get_fixed_height_children_sum() - total_padding;
        auto fw_p_el = fh_count ? fh_size / fh_count : 0;
        // ---

        int i = 0;
        int offsetY = padding.top + border.top; 

        for (auto el : children)
        {
            Rect copy = true_rect;

            copy.x += padding.left + border.left;
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
                copy.width = true_rect.width - reserved.width;
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
                el->calc_fit_content_self(&copy, window);
            }

            el->set_true_rect(copy);
            el->calc_children_true_rects(&copy, window);

            offsetY += copy.height;
            i++;
        }
    }
};



#endif
