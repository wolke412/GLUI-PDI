#include <GLUI/text.hpp>

CharacterMap Characters;

unsigned int textVAO;
unsigned int textVBO;

Shader* shader_text = nullptr; // Global pointer to shader


/**
 * 
 */
void DEBUG_init_text(){

  TextLoadFont("fonts/Poppins-Regular.ttf");

  InitTextRendering();

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}


/**
 * Loads a font texture into memory;
 * /!\ Only one font can be loaded at a time.
 */
void TextLoadFont(const std::string& fontPath)
{

    FT_Library ft;
    if (FT_Init_FreeType(&ft)) {
        std::cout << "FREE_TYPE::ERROR:INIT Could not init FreeType Library\n";
        return;
    }

    FT_Face face;
    if (FT_New_Face(ft, fontPath.c_str(), 0, &face)) {
        std::cout << "FREETYPE::ERROR:LOAD Failed to load font\n";
        return;
    }

    std::cout << "Family: " << face->family_name << "\n";
    std::cout << "Style: " << face->style_name << "\n";
    std::cout << "Num Glyphs: " << face->num_glyphs << "\n";

    FT_Set_Pixel_Sizes(face, 0, DEFAULT_FONT_SIZE);  

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); 


    for (unsigned char c = 0; c < IMPORT_CHAR_COUNT; c++) {  

        if ( FT_Load_Char(face, c, FT_LOAD_RENDER) ) {
            std::cout << "FREE_TYPE::WARNING::GLYPH Failed to load glyph " << c << "\n";
            continue;
        }

        // std::cout << "Glyph '" << c << "' loaded successfully! "
        //           << "Bitmap Width: " << face->glyph->bitmap.width 
        //           << " Height: " << face->glyph->bitmap.rows << "\n";

        unsigned int texture;

        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
        );

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        Character character = {
            texture,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            static_cast<unsigned int>(face->glyph->advance.x)
        };

        Characters.insert(std::pair<char, Character>(c, character));
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    
    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    std::cout << "Font Loaded" << std::endl;
}

//
//
// ------------------
int TextCalcRenderWidth( const std::string t, float scale) {

    std::vector<uint32_t> codepoints = decode_utf_8(t);

    int s = 0;

    for (uint32_t cp : codepoints) 
    {
        if (Characters.find(cp) == Characters.end()) {
            std::cout << "Warning: Glyph not found for Unicode " << cp << std::endl;
            continue;
        }

        Character ch = Characters[cp];

        // float w = ch.Size.x * scale * 0;
        float spacingx = (ch.Advance >> 6) * scale;

        s += (int) spacingx;
    }

    return s;
}

//
// ------------------
int TextCalcRenderHeightSingleLine( const std::string t, float scale) {

    std::vector<uint32_t> codepoints = decode_utf_8(t);

    int gt = 0;

    for (uint32_t cp : codepoints) 
    {
        if (Characters.find(cp) == Characters.end()) {
            std::cout << "Warning: Glyph not found for Unicode " << cp << std::endl;
            continue;
        }

        Character ch = Characters[cp];

        // float spacingy = (ch.Bearing.y);
        float spacingy = (ch.Size.y);


        if ( spacingy > gt ) {
            gt = spacingy;
        }
    }

    return gt * scale;
    // return (gt + DEFAULT_FONT_SIZE) * scale;
}


//
// Render line of text
// -------------------
void RenderText(std::string text, Coord at, float scale, RGB color, Size *window)
{

    if ( ! shader_text ) {
        std::cerr << "Error: Text shader not initialized!" << std::endl;
        return;
    }

    // This makes 0,0 bottom left
    glm::mat4 projection = glm::ortho(0.0f, (float)window->width, 0.0f, (float)window->height);

    at.y = window->height - at.y - DEFAULT_FONT_SIZE * scale;

    
    // Shader shader("shaders/text.vs", "shaders/text.fs");

    shader_text->use();
    shader_text->setMat4("projection", projection);
    shader_text->setFloat3("textColor", color.R, color.G, color.B);
    // glUniform3f(glGetUniformLocation(shader.ID, "textColor"), color.x, color.y, color.z);
    
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(textVAO);

    // Decode UTF-8 into Unicode code points
    std::vector<uint32_t> codepoints = decode_utf_8(text);

    // Iterate through Unicode code points
    for (uint32_t cp : codepoints) 
    {
        // Check if character exists in the map
        if (Characters.find(cp) == Characters.end()) {
            std::cout << "Warning: Glyph not found for Unicode " << cp << std::endl;
            continue;
        }

        Character ch = Characters[cp];

        float xpos = at.x + ch.Bearing.x * scale;

        float bryan = (DEFAULT_FONT_SIZE/5 * scale);  // this is the magic offset -> I call it bryan.

        float ypos = (at.y - (ch.Size.y - ch.Bearing.y) * scale ) + bryan;

        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;

        // Vertex positions in pixel space
        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            
            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 0.0f }
        };

        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        glBindBuffer(GL_ARRAY_BUFFER, textVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDrawArrays(GL_TRIANGLES, 0, 6);


        // Make sure the advance is divided by 64 to get the correct units since FreeType gives it in 1/64th of a pixel. 
        at.x += (ch.Advance >> 6) * scale;

        // std::cout << ch.TextureID << "] " <<at.x << ", " << at.y << std::endl;
    }

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);

    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        std::cerr << "OpenGL Error: " << err << std::endl;
    }
}


void InitTextRendering()
{
    glGenVertexArrays(1, &textVAO);
    glGenBuffers(1, &textVBO);

    glBindVertexArray(textVAO);
    glBindBuffer(GL_ARRAY_BUFFER, textVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, nullptr, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    shader_text = new Shader("shaders/text.vs", "shaders/text.fs"); // Allocate shader once
}

// Function to decode a UTF-8 string into Unicode code points.
std::vector<uint32_t> decode_utf_8(const std::string& str) {
    std::vector<uint32_t> codepoints;
    size_t i = 0;
    while (i < str.size()) {

        unsigned char c = str[i];
        uint32_t codepoint = 0;

        int numBytes = 0;

        if ((c & 0x80) == 0) {  
            codepoint = c;
            numBytes = 1;
        } else if ((c & 0xE0) == 0xC0) {  
            codepoint = c & 0x1F;
            numBytes = 2;
        } else if ((c & 0xF0) == 0xE0) {  
            codepoint = c & 0x0F;
            numBytes = 3;
        } else if ((c & 0xF8) == 0xF0) {  
            codepoint = c & 0x07;
            numBytes = 4;
        } else {
            throw std::runtime_error("Invalid UTF-8 encoding detected.");
        }

        if (i + numBytes > str.size()) {
            throw std::runtime_error("Truncated UTF-8 sequence.");
        }

        for (int j = 1; j < numBytes; j++) {
            unsigned char next = str[i + j];

            if ((next & 0xC0) != 0x80) {
                throw std::runtime_error("Invalid UTF-8 continuation byte.");
            }

            codepoint = (codepoint << 6) | (next & 0x3F);
        }

        codepoints.push_back(codepoint);
        i += numBytes;
    }

    return codepoints;
}


std::string encode_utf8(uint32_t codepoint) {
    std::string result;

    if (codepoint <= 0x7F) {
        // 1 byte (0xxxxxxx)
        result.push_back(static_cast<char>(codepoint));
    } 
    else if (codepoint <= 0x7FF) {
        // 2 bytes (110xxxxx 10xxxxxx)
        result.push_back(static_cast<char>((codepoint >> 6) | 0xC0));         // 110xxxxx
        result.push_back(static_cast<char>((codepoint & 0x3F) | 0x80));      // 10xxxxxx
    } 
    else if (codepoint <= 0xFFFF) {
        // 3 bytes (1110xxxx 10xxxxxx 10xxxxxx)
        result.push_back(static_cast<char>((codepoint >> 12) | 0xE0));       // 1110xxxx
        result.push_back(static_cast<char>(((codepoint >> 6) & 0x3F) | 0x80)); // 10xxxxxx
        result.push_back(static_cast<char>((codepoint & 0x3F) | 0x80));       // 10xxxxxx
    } 
    else if (codepoint <= 0x10FFFF) {
        // 4 bytes (11110xxx 10xxxxxx 10xxxxxx 10xxxxxx)
        result.push_back(static_cast<char>((codepoint >> 18) | 0xF0));       // 11110xxx
        result.push_back(static_cast<char>(((codepoint >> 12) & 0x3F) | 0x80)); // 10xxxxxx
        result.push_back(static_cast<char>(((codepoint >> 6) & 0x3F) | 0x80));  // 10xxxxxx
        result.push_back(static_cast<char>((codepoint & 0x3F) | 0x80));       // 10xxxxxx
    } 
    else {
        throw std::invalid_argument("Invalid Unicode code point.");
    }

    return result;
}