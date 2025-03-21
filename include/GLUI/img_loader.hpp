#ifndef GLUI_IMAGE_LOADER_H
#define GLUI_IMAGE_LOADER_H

#include <GLUI/deps/stb_image.h>
#include <GLUI/deps/stb_image_write.h>

#include <GLUI/rect.hpp>

#include <string>
#include <memory>
#include <filesystem>

struct GLShit {
    unsigned int VBO;
    unsigned int VAO;
};

struct GLShitFBO {
    GLuint VBO;
    GLuint VAO;
    GLuint FBO;
    GLuint texture;
    GLuint RBO;

    void read( uint8_t* nout, Size* sz ) {
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);
        glReadBuffer(GL_COLOR_ATTACHMENT0);
        glReadPixels(0, 0, sz->width, sz->height, GL_RGB, GL_UNSIGNED_BYTE, nout);
        glBindFramebuffer(GL_FRAMEBUFFER, 0); // Unbind the framebuffer
    };
};

class ImageHandler {
    
public:
    GLShitFBO m_fbo;

private: 
    std::string m_path = "static/container.jpg";
    Size m_img_size = Size(0);
    int m_ch_count = 0;

    unsigned char * m_data = nullptr;

    unsigned int m_texture = UINT_MAX;

    bool m_is_kernel_shader = false;
    glm::mat3 m_kernel; 

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

        request_texture_reload();
    }

    bool load(  )  {
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

        return true;
    }

    bool save(const std::string &as) const {
        if (!m_data || m_img_size.width <= 0 || m_img_size.height <= 0 || m_ch_count <= 0) {
            return false;
        }
    
        std::string ext = as.substr(as.find_last_of('.') + 1);
    
        bool success = false;
    
        if (ext == "png") {
            success = stbi_write_png(as.c_str(), m_img_size.width, m_img_size.height, m_ch_count, m_data, m_img_size.width * m_ch_count);
        } 
        else if (ext == "jpg" || ext == "jpeg") {
            success = stbi_write_jpg(as.c_str(), m_img_size.width, m_img_size.height, m_ch_count, m_data, 100); // 100 = max quality
        } 
        else {
            return false; // Unsupported format
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
    
    void set_is_kernel_shader( bool is_k ) {
        m_is_kernel_shader = is_k;
    }

    bool is_kernel_shader() {
        return m_is_kernel_shader;
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

    glm::mat3 get_transformation_kernel() const {
        return m_kernel;
    }

    void set_kernel( glm::mat3 k ) {
        std::cout << "setting ku"  << std::endl;
        m_kernel = k;
        std::cout << " ku set"  << std::endl;
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
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_img_size.width, m_img_size.height, 0, GL_RGB, GL_UNSIGNED_BYTE, m_data);
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