#ifndef GLUI_IMAGE_LOADER_H
#define GLUI_IMAGE_LOADER_H

#include <GLUI/deps/stb_image.h>
#include <GLUI/deps/stb_image_write.h>

/**
 * When using GPU to calculate shit
 */
#include <GLUI/framebuffer.hpp>

#include <string>
#include <memory>
#include <filesystem>
class ImageHandler {
    
public:
    GLShitFBO m_fbo;

private: 

    std::string m_path = "static/container.jpg";
    Size m_img_size = Size(0);
    int m_ch_count = 0;
    unsigned char * m_data = nullptr;

    //  openGL stuff
    unsigned int m_texture = UINT_MAX;

    // when using gpu accelaration
    bool m_is_framebuffer = false;

public:
    ImageHandler(const std::string path): m_path(path) {}
    ImageHandler(): m_path("") {}


    bool is_valid_image_path( std::string *path ) {
        if ( ! std::filesystem::exists(*path) )  {
            return false;
        }

        if ( std::filesystem::is_directory(*path) )  {
            return false;
        }

        return true;
    }

    void set_path( std::string path ) {
        m_path = path;

        if ( m_data ) {
            free();    
            m_data = nullptr;
        }

        // request_texture_reload();
    }

    std::string get_path() const {
        return m_path;
    }

    bool load(  )  {

        // gpu stuff y'know
        // stbi_set_flip_vertically_on_load(true);

        unsigned char *data = stbi_load(
            m_path.c_str(), 
            (int *)&m_img_size.width, 
            (int*)&m_img_size.height, 
            &m_ch_count, 
            0);


        // m_data = std::make_shared<unsigned char *>( data );
        m_data = ( data );

        std::cout << "IMGLOADER::LOADED_IMG_OF w::"
                  << m_img_size.width
                  << "  h::"
                  << m_img_size.width
                  << std::endl;


        // i guess it makes more sense to be here. 
        request_texture_reload();

        return true;
    }

    bool save(const std::string &as) const {
        if (!m_data || m_img_size.width <= 0 || m_img_size.height <= 0 || m_ch_count <= 0) {
            return false;
        }
    
        std::string ext = as.substr(as.find_last_of('.') + 1);
    
        stbi_flip_vertically_on_write(true);

        bool success = false;
    
        if (ext == "png") {
            success = stbi_write_png(as.c_str(), m_img_size.width, m_img_size.height, m_ch_count, m_data, m_img_size.width * m_ch_count);
        } 
        else if (ext == "jpg" || ext == "jpeg") {
            success = stbi_write_jpg(as.c_str(), m_img_size.width, m_img_size.height, m_ch_count, m_data, 100); // 100 = max quality
        } 
        else {
            // atensao: format nao suportado
            return false; 
        }
    
        return success;
    }

    uint8_t* get_binary() {
        return m_data;
    }

    uint8_t get_channel_count() {
        return m_ch_count;
    }

    void set_binary( uint8_t *data ) {
        if ( m_data ) {
            free();
        }

        m_data = data;
    }

    void copy_to( ImageHandler *to ) {

        if (!to) return; 

        to->m_path = m_path;
        to->m_img_size = m_img_size;
        to->m_ch_count = m_ch_count;
        
        if ( to->is_loaded() ) {
            // Se tem uma imagem la tem que limpar ne tche
            to->free();
        }
        
        if ( m_data && m_img_size.width > 0 && m_img_size.height > 0 ) {
            size_t dataSize = m_img_size.width * m_img_size.height * m_ch_count;
            to->m_data = new unsigned char[dataSize];
            std::memcpy(to->m_data, m_data, dataSize);
        } 
        else {
            to->m_data = nullptr;
        }
    }

    Size* get_size()  {
        return &m_img_size;
    }

    void set_is_framebuffer( bool is ) {
        m_is_framebuffer = is;
    }

    bool is_framebuffer() {
        return m_is_framebuffer;
    }

    bool is_framebuffer_configured() {
        // maybe there will be more things to compare here
        if ( ! m_fbo.FBO ) {
            return false;
        }

        return true;
    }

    void reset_fbo() {
        free_fbo(&m_fbo);
        m_fbo = init_fbo( get_size() );
        set_fbo_buffers(&m_fbo.VAO, &m_fbo.VBO) ;
    }

    void assert_fbo() {
        if ( m_fbo.FBO ) {
            std::cout << "FBO is set" << std::endl;
            return;
        }

        std::cout << "FBO is NOT set" << std::endl;

        m_fbo = init_fbo( get_size() );
        set_fbo_buffers(&m_fbo.VAO, &m_fbo.VBO) ;
    }

    void read_generated_fbo() {

        std::cout << "Reading from FBO"  << std::endl;

        // if ( is_loaded() ) {
        //     free();
        //     std::cout << "Freed old"  << std::endl;
        // }

        m_fbo.read( m_data, &m_img_size );

        std::cout << "Successfully read."  << std::endl;
    }

    /**
     * This function only generates
     * ===========================
     */
    unsigned int generate_texture() {

        glGenTextures(1, &m_texture);
        glBindTexture(GL_TEXTURE_2D, m_texture);

        // set the texture wrapping/filtering options (on the currently bound texture object)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        if (m_data)
        {
            if ( m_ch_count == 4 ) {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_img_size.width, m_img_size.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_data);
            }
            else if ( m_ch_count == 3 ) {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_img_size.width, m_img_size.height, 0, GL_RGB, GL_UNSIGNED_BYTE, m_data);
            }
            glGenerateMipmap(GL_TEXTURE_2D);

            // a
            // unbind_texture();

            return true;
        }

        else
        {
            std::cout << "ERROR::IMAGE::TEXTURE  ::error:Failed to load texture" << std::endl;
            // unbind_texture();
            return false;
        }

    }

    void unbind_texture()
    {
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    bool has_texture() {
        return m_texture != UINT_MAX;
    }

    unsigned int get_texture() {
        return m_texture; 
    }

    void request_texture_reload () {
        m_texture = UINT_MAX;
    }

    unsigned int bind_texture() {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_texture);

        return m_texture;
    }

    bool is_loaded() {
        return m_data != nullptr;
    }

    void free() {
        stbi_image_free(m_data);
    }

    ~ImageHandler() {
        free();
    }

};


#endif