#include <GLUI/draw.hpp>


GLShit RECT;
GLShit TEX_RECT;


Shader *rect_shader          = nullptr;
Shader *tex_rect_shader      = nullptr;
Shader *rounded_rect_shader  = nullptr;

// -----
Shader *kernel_compute_shader   = nullptr;

//-------

void printVector(float* vec, int totalSize, int rowSize) {
    for (int i = 0; i < totalSize; ++i) {
        std::cout << *(vec + i) << " ";   
        if ((i + 1) % rowSize == 0) {
            std::cout << std::endl;
        }
    }
    
    if (totalSize % rowSize != 0) {
        std::cout << std::endl;
    }
}

//-------


void init_quad( GLShit *gl );
void init_tex_quad( GLuint *VAO, GLuint *VBO );

void initialize_drawing() {
    rect_shader         = new Shader("shaders/rect.vs", "shaders/rect.fs");
    tex_rect_shader     = new Shader("shaders/tex_rect.vs", "shaders/tex_rect.fs");
    rounded_rect_shader = new Shader("shaders/rounded-rect.vs", "shaders/rounded-rect.fs");

    kernel_compute_shader  = new Shader("shaders/tex_rect.vs", "shaders/tex_compute_kernel.fs");
    
    init_quad( &RECT );
}





GLShitFBO init_fbo(  Rect *r, Size* window, ImageHandler *i )  {
    
    // framebuffer configuration
    // -------------------------

    unsigned int framebuffer;
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    // create a color attachment texture
    unsigned int textureColorbuffer;
    glGenTextures(1, &textureColorbuffer);
    glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, r->width, r->height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);
    // create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
    unsigned int rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, r->width, r->height); // use a single renderbuffer object for both a depth AND stencil buffer.
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); // now actually attach it

    // now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    auto g = GLShitFBO{
        .FBO= framebuffer,
        .texture=textureColorbuffer,
        .RBO= rbo
    };

    init_tex_quad(&g.VAO, &g.VBO);

    return g;
}

void init_tex_quad( GLuint *VAO, GLuint *VBO ){


    glGenVertexArrays(1, VAO);
    glGenBuffers(1, VBO);

    glBindVertexArray(*VAO);
    glBindBuffer(GL_ARRAY_BUFFER, *VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 2 * 4 * 2, nullptr, GL_DYNAMIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void init_quad( GLShit *gl ){

    glGenVertexArrays(1, &gl->VAO);

    glGenBuffers(1, &gl->VBO);

    glBindVertexArray(gl->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, gl->VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 2 * 4, nullptr, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0); 

    //unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}


// Normalize  to NDC
void normalize(Rect * rect, Size *window, float* vertices){

    float ndcX = (2.0f * rect->x) / window->width - 1.0f;
    float ndcY = 1.0f - (2.0f * rect->y) / window->height;
    float ndcW = (2.0f * rect->width) / window->width;
    float ndcH = (2.0f * rect->height) / window->height;

    // (two triangles)
    float tempVertices[] = {
        ndcX,         ndcY,          // Top-left
        ndcX + ndcW,  ndcY,          // Top-right
        ndcX + ndcW,  ndcY - ndcH,   // Bottom-right
        ndcX,         ndcY - ndcH    // Bottom-left
    };

    for (int i = 0; i < 8; i++) vertices[i] = tempVertices[i];
}

void normalize_rect(Rect * rect, Size *window ){

    float ndcX = (2.0f * rect->x) / window->width - 1.0f;
    float ndcY = 1.0f - (2.0f * rect->y) / window->height;
    float ndcW = (2.0f * rect->width) / window->width;
    float ndcH = (2.0f * rect->height) / window->height;

    rect->width = ndcW;
    rect->height = ndcH;
    rect->x = ndcX;
    rect->y = ndcY;
}

float normalize_to_range( float v, float min, float max ){
    return (v - min) / ( max - min );
}

void apply_tex(float* vertices, float* nvertices) {
    float texCoordsTemp[] = {
        0.0f, 1.0f,  // Top-left
        1.0f, 1.0f,  // Top-right
        1.0f, 0.0f,  // Bottom-right
        0.0f, 0.0f   // Bottom-left
    };

    int numVertices = 4; 

    for (int i = 0; i < numVertices; i++) {
        nvertices[i * 4 + 0] = vertices[i * 2 + 0]; // X
        nvertices[i * 4 + 1] = vertices[i * 2 + 1]; // Y

        nvertices[i * 4 + 2] = texCoordsTemp[i * 2 + 0]; // S
        nvertices[i * 4 + 3] = texCoordsTemp[i * 2 + 1]; // T
    }
}


void draw_quad( Rect *r, RGB c , Size* window) {

    float vertices[8];

    normalize(r, window, vertices);

    if ( rect_shader == nullptr ) {
        std::cout << "ERROR::UNINITIALIZED_SHADER ::" << "rect_shader" << std::endl;
        return;
    }

    glBindVertexArray(RECT.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, RECT.VBO);

    // Update buffer with new vertex data
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

    // Set button color
    rect_shader->use();
    rect_shader->setFloat3("buttonColor", c.R, c.G, c.B);

    // Draw the quad as two triangles
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    // glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void draw_rounded_quad( Rect *r, RGB c, glm::vec4 corners, Size* window) {

    float vertices[8];

    normalize(r, window, vertices);

    if ( rect_shader == nullptr ) {
        std::cout << "ERROR::UNINITIALIZED_SHADER ::" << "rounded_rect_shader" << std::endl;
        return;
    }

    glBindVertexArray(RECT.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, RECT.VBO);

    // Update buffer with new vertex data
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

    rounded_rect_shader->use();
    rounded_rect_shader->setFloat2("resolution", window->width, window->height);
    rounded_rect_shader->setFloat3("buttonColor", c.R, c.G, c.B);
    rounded_rect_shader->setFloat4("corners", corners);
    rounded_rect_shader->setFloat2("size", r->width, r->height );
    rounded_rect_shader->setFloat2("offset", r->x, window->height - r->height - r->y );

    // Draw the quad as two triangles
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void draw_quad( Rect r, RGB c , Size* window) {
    draw_quad(&r, c, window);
}




void draw_tex_quad( Rect *r, ImageHandler *img, Size* window) {

    float jvertices[8]; 
    float vertices[16]; // 8 for coord + 8 for tex

    // Normalizes vertexes from window space to GL space
    normalize(r, window, jvertices);

    apply_tex(jvertices, vertices);

    /**
     * TODO:
     * This must be calc and stored by the
     * Element itself; not calc'ed every frame.
     * ==============================
     */
    unsigned int VAO, VBO;

    glad_glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    /**
     * ===============================
     */

    if ( ! img->has_texture() ) {
        img->generate_texture();
    }

    img->bind_texture();

    tex_rect_shader->use();

    // Draw the quad as two triangles
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    // Cleanup
    glDeleteVertexArrays(1, &VAO);

    glDeleteBuffers(1, &VBO);

}

void draw_compute_tex_quad( GLShitFBO* g, glm::mat3 kernel, Rect *r, ImageHandler *img, Size* win) {

    float jvertices[8]; 
    float vertices[16]; // 8 for coord + 8 for tex

    Size fbosize(r->width, r->height);

    // Normalizes vertexes from window space to GL space
    auto force0 = Rect(Size(r->width, r->height));
    normalize(&force0, &fbosize, jvertices);
    apply_tex(jvertices, vertices);

    // render
    // ------
    // bind to framebuffer and draw scene as we normally would to color texture
    glBindFramebuffer(GL_FRAMEBUFFER, g->FBO );
    glViewport( 0, 0, r->width, r->height );
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST); // enable depth testing (is disabled for rendering screen-space quad)

    // unsigned int VAO, VBO;
    // glGenVertexArrays(1, &VAO);
    // glGenBuffers(1, &VBO);

    // glBindVertexArray(VAO);
    // glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // // position attribute
    // glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    // glEnableVertexAttribArray(0);
    // glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    // glEnableVertexAttribArray(1);

    glBindVertexArray(g->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, g->VBO);

    // Update buffer with new vertex data
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

    if ( ! img->has_texture() ) {
        img->generate_texture();
    }

    img->bind_texture();

    auto sz = img->get_size();

    kernel_compute_shader->use();
    kernel_compute_shader->setMat3(   "u_kernel", kernel );
    kernel_compute_shader->setFloat2( "u_texSize", r->width, r->height );

    /**
     * ===============================
     */
    if ( ! img->has_texture() ) {
        img->generate_texture();
    }

    img->bind_texture();

    // Draw the quad as two triangles
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4); 

    /**
     * ===============================
     *  Bind your offscreen FBO as the source for reading.
     */
    int dest_x0 = r->x;
    int dest_y0 = win->height - r->y - r->height; // convert top-left to bottom-left
    int dest_x1 = r->x + r->width;
    int dest_y1 = win->height - r->y;

    glBindFramebuffer(GL_READ_FRAMEBUFFER, g->FBO);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glViewport( 0, 0, win->width, win->height );

    glBlitFramebuffer(
        0, 0, r->width, r->height, // source FBO region
        dest_x0, dest_y0, dest_x1, dest_y1,
        GL_COLOR_BUFFER_BIT, GL_NEAREST
    );

    glBindFramebuffer(GL_FRAMEBUFFER, 0); // Unbind the framebuffer
    glDisable(GL_DEPTH_TEST);

    // Cleanup
    // glDeleteVertexArrays(1, &VAO);
    // glDeleteBuffers(1, &VBO);
}

