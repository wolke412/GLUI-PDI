#ifndef TEXT_H
#define TEXT_H

#include "GLUI/layout.hpp"
#include <cstdint>
#include <map>
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <shader.hpp>

#include <GLUI/rect.hpp>
#include <GLUI/colors.hpp>

void DEBUG_init_text(); 

/// Holds all state information relevant to a character as loaded using FreeType
struct Character {
    unsigned int TextureID; // ID handle of the glyph texture
    glm::ivec2   Size;      // Size of glyph
    glm::ivec2   Bearing;   // Offset from baseline to left/top of glyph
    unsigned int Advance;   // Horizontal offset to advance to next glyph
};

typedef std::map<GLchar, Character> CharacterMap;

extern CharacterMap Characters;
extern unsigned int textVAO;
extern unsigned int textVBO;

#define IMPORT_CHAR_COUNT    ( 0xFF ) 
#define DEFAULT_FONT_SIZE    (   24 )

std::vector<uint32_t> decode_utf_8(const std::string& str);
std::string encode_utf8(uint32_t codepoint);


void TextLoadFont(const std::string& fontPath);

void InitTextRendering();
void RenderText(std::string text, Coord at, float scale, RGB color, Size *window);

int TextCalcRenderWidth( const std::string t, float scale);
int TextCalcRenderHeightSingleLine( const std::string t, float scale);

class Text : public Element {

    private :
        std::string m_Text; 
        float m_Scale        = 1;
        uint8_t m_Font_size = DEFAULT_FONT_SIZE;
        uint8_t m_line_height = DEFAULT_FONT_SIZE;
        RGB m_Fg_color      = WHITE;


    public:
        Text(const std::string text): m_Text(text), Element() {
            rect.width  = LAYOUT_FIT_CONTENT;
            rect.height = m_Font_size;
        }

        Text(const std::string text, RGB fg): m_Text(text), m_Fg_color(fg), Element() {}

        void draw( Size *window_size ) override {
            auto tr = get_true_rect();

            // draw_quad(*tr, RGB(.5, .2, .2), window_size);
            RenderText(m_Text, Coord(tr->x, tr->y), m_Scale, m_Fg_color, window_size);
        }

        void calc_fit_content_self(Rect *current, Size *window ) override
        {
            current->height = m_line_height;
            // current->height = TextCalcRenderHeightSingleLine( m_Text, m_Scale );
            current->width = TextCalcRenderWidth( m_Text, m_Scale );

            // std::cout << "Finished self calc w:" << current->width << std::endl;
        }
    
        std::string get_text() const {
            return m_Text;
        }

        void set_text( std::string t ) {
            m_Text = t;
        }

        void set_foreground_color(RGB color) {
            m_Fg_color= color;
        }

        void set_font_size( uint8_t fs ) {
            m_Font_size = fs;
            m_line_height = fs;
            m_Scale = (float)fs / (float)DEFAULT_FONT_SIZE;
        }
};


#endif
