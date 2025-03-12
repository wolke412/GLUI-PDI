#include <GLUI/draw.hpp>

Shader *rect_shader  = nullptr;
Shader *tex_rect_shader  = nullptr;

void initialize_drawing() {
    rect_shader      = new Shader("shaders/rect.vs", "shaders/rect.fs");
    tex_rect_shader  = new Shader("shaders/tex_rect.vs", "shaders/tex_rect.fs");
}

// Normalize  to NDC
void normalize(Rect * rect, Size *window, float* vertices){

    float ndcX = (2.0f * rect->x) / window->width - 1.0f;
    float ndcY = 1.0f - (2.0f * rect->y) / window->height;
    float ndcW = (2.0f * rect->width) / window->width;
    float ndcH = (2.0f * rect->height) / window->height;

    // (two triangles)
    float tempVertices[] = {
        ndcX,         ndcY,         // Top-left
        ndcX + ndcW, ndcY,         // Top-right
        ndcX + ndcW, ndcY - ndcH,  // Bottom-right
        ndcX,         ndcY - ndcH  // Bottom-left
    };
     for (int i = 0; i < 8; i++) vertices[i] = tempVertices[i];
}

void apply_tex(float* vertices, float* nvertices) {
    float texCoordsTemp[] = {
        0.0f, 1.0f,  // Top-left
        1.0f, 1.0f,  // Top-right
        1.0f, 0.0f,  // Bottom-right
        0.0f, 0.0f   // Bottom-left
    };

    int numVertices = 4; // We assume a quad

    for (int i = 0; i < numVertices; i++) {
        // Copy position (x, y)
        nvertices[i * 4 + 0] = vertices[i * 2 + 0]; // X
        nvertices[i * 4 + 1] = vertices[i * 2 + 1]; // Y

        // Copy correct texture coordinate
        nvertices[i * 4 + 2] = texCoordsTemp[i * 2 + 0]; // S
        nvertices[i * 4 + 3] = texCoordsTemp[i * 2 + 1]; // T
    }

    // Debug output
    // std::cout << "nvertices: ";
    // for (int i = 0; i < numVertices * 4; i+=4) {
        // std::cout << nvertices[i] << ", ";
        // std::cout << nvertices[i+1] << ", ";
        // std::cout << nvertices[i+2] << ", ";
        // std::cout << nvertices[i+3] << ", ";
        // std::cout << std::endl;
    // }
}


void draw_quad( Rect *r, RGB c , Size* window) {

    float vertices[8];

    // Normalizes vertexes from window space to GL space
    normalize(r, window, vertices);

    // OpenGL setup
    unsigned int VAO, VBO;

    glad_glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0); 

    if ( rect_shader == nullptr ) {
        std::cout << "ERROR::UNINITIALIZED_SHADER ::" << "rect_shader" << std::endl;
        return;
    }

    // Set button color
    rect_shader->use();
    rect_shader->setFloat3("buttonColor", c.R, c.G, c.B);

    // Draw the quad as two triangles
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    // Cleanup
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
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

void draw_quad( Rect r, RGB c , Size* window) {
    draw_quad(&r, c, window);
}

