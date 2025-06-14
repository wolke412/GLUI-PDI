#include <PDI/core.hpp>
#include <PDI/pdi.hpp>
#include <GLUI/multipassfbo.hpp>

#include <immintrin.h>  // For AVX2 intrinsics
#include <variant>



Shader* shader_invert      = nullptr;
Shader* shader_brightness  = nullptr;
Shader* shader_threshold   = nullptr;
Shader* shader_greyscale   = nullptr;
Shader* shader_median = nullptr;
Shader* shader_convolution = nullptr;
//
Shader* shader_sobel    = nullptr;
Shader* shader_robinson = nullptr;

Shader* shader_morph_dilation = nullptr;
Shader* shader_morph_erosion  = nullptr;

Shader* shader_thinner_holt = nullptr;


void PDI::load_shaders() {
    shader_invert      = new Shader ("shaders/tex_rect.vs", "shaders/filters/invert.fs");
    shader_brightness  = new Shader ("shaders/tex_rect.vs", "shaders/filters/brightness-contrast.fs");
    shader_greyscale   = new Shader ("shaders/tex_rect.vs", "shaders/filters/greyscale.fs");
    shader_threshold   = new Shader ("shaders/tex_rect.vs", "shaders/filters/threshold.fs");
    shader_median      = new Shader ("shaders/tex_rect.vs", "shaders/filters/median-filter.fs");
    shader_convolution = new Shader ("shaders/tex_rect.vs", "shaders/filters/convolution.fs");
    //
    shader_sobel    = new Shader ("shaders/tex_rect.vs", "shaders/filters/sobel.fs");
    shader_robinson = new Shader ("shaders/tex_rect.vs", "shaders/filters/robinson.fs");
    //
    shader_morph_dilation = new Shader ("shaders/tex_rect.vs", "shaders/morphology/dilation.fs");
    shader_morph_erosion  = new Shader ("shaders/tex_rect.vs", "shaders/morphology/erosion.fs");
    //
    shader_thinner_holt   = new Shader ("shaders/tex_rect.vs", "shaders/morphology/thinner-holt.fs");
}

void PDI::layout() {
    Components::layout(this);
    Components::generate_pipeline_components(this);
}

/**
 * ============================================================
 *      CONFIGURA OS ATALHOS DA APLICAÇÃO
 * ============================================================
 */
void PDI::setup_hotkeys() {

    auto glui = get_glui();
    auto hk   = glui->get_hotkeys(); // get hotkeys handler
    
    // INTERNALS BINDS
    // ----------------------------------------
    // Binds relacionadas ao funcionamento do sistema
    // ============================================================ 

    // F5 -> Reload shaders
    hk->registerhk(GLFW_KEY_F5, 0, [&](){
        load_shaders();
    });

    // FILE BINDS
    // ============================================================ 

    // CTRL + S   -> Salva a Imagem
    hk->registerhk(GLFW_KEY_S, GLFW_MOD_CONTROL , [&](){

        std::cout << "Getting bytes" << std::endl;

        auto o = get_output(); 

        o->read_generated_fbo();
    
        std::cout << "After read"  << std::endl;

        auto bin = o->get_binary();

        std::string ext  = o->get_channel_count() == 3 ? "jpg" : "png";
        std::string path = "static/save";
        path.append(".");
        path.append(ext);


        o->save( path );
    });


    //  BINDS DE TRANSFORMAÇÃO 
    // ============================================================ 
        
    // CTRL + 0  -> RESETA TRANSFORMAÇÕES
    hk->registerhk('0', GLFW_MOD_CONTROL, [&](){
        reset_transform();
        request_update();
    });

    // CTRL + R         -> Rotação sentido horário 
    hk->registerhk('R', GLFW_MOD_CONTROL, [&](){
        m_angle -= 5;
        request_update();
    });

    // CTRL + SHIFT + R -> Rotação sentido anti-horário 
    hk->registerhk('R', GLFW_MOD_SHIFT | GLFW_MOD_CONTROL, [&](){
        m_angle += 5;
        request_update();
    });

    /**
     * CTRL + (setinhas)  -> Translação. Step = 5 pixels
     */
    hk->registerhk(GLFW_KEY_UP, GLFW_MOD_CONTROL, [&](){
        m_translate_y += 5;
        request_update();
    });
    hk->registerhk(GLFW_KEY_DOWN, GLFW_MOD_CONTROL , [&](){
        m_translate_y -= 5;
        request_update();
    });
    hk->registerhk(GLFW_KEY_LEFT, GLFW_MOD_CONTROL , [&](){
        m_translate_x -= 5;
        request_update();
    });
    hk->registerhk(GLFW_KEY_RIGHT, GLFW_MOD_CONTROL , [&](){
        m_translate_x += 5;
        request_update();
    });

    /**
     * CTRL + M  -> Espelhamento: X -> Y -> X e Y -> Nenhunm.
     */
    hk->registerhk(GLFW_KEY_M, GLFW_MOD_CONTROL , [&](){
        m_mirror_axis = (Axis) ( ( (int)m_mirror_axis + 1 ) % 4);
        request_update();

    });

    /**
     * CTRL + '='  -> Zoom In
     */
    hk->registerhk('=', GLFW_MOD_CONTROL , [&](){
        m_scale_x += 0.05; 
        m_scale_y += 0.05; 

        request_update();
    });
    /**
     * CTRL + '='  -> Zoom Out
     */
    hk->registerhk('-', GLFW_MOD_CONTROL , [&](){
        m_scale_x -= 0.05; 
        m_scale_y -= 0.05; 

        if ( m_scale_x <= 0.01 ) {
            m_scale_x = 0.01;
            m_scale_y = 0.01;
        }

        request_update();
    });
}

void PDI::layout_pipeline_components() {
    Components::generate_pipeline_components(this);
}

void PDI::request_update() {
    m_should_update = true;
}

void PDI::update() {
    if ( ! m_should_update ) return;

    m_should_update = false;

    /**
     * TODO: Make transform its own "PipelineStage"
     */
    transform();

    /**
     * Executar o restante das operações do PIPELINE
     */
#if USE_GPU
    auto winsz = glui->get_window_size();
    auto imgsz = output->get_size();

    if ( pipeline.size() || pipeline.active_size() ) {
        pipeline.run(this);

        MultiPassFBO multipassFBO(imgsz->width, imgsz->height);
        multipassFBO.process(output->m_fbo.texture, *(pipeline.get_shaders()), &winsz, output->m_fbo.FBO);

        // limpa a file execução de shaders gerados. Na próxima iteração ela é gerada novamente...
        // Pois significa que houve uma alteração.
        pipeline.flush_shaders();
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    GLenum err = glGetError();

    if (err != GL_NO_ERROR)
    {
        std::cout << "OpenGL Error after apply(): " << err << std::endl;
    }
#endif
}

void PDI::update_pipeline() {
    layout_pipeline_components(); 
    request_update();
}

/**
 * Reseta os campos de transformação 
 */
void PDI::reset_transform() {
   m_scale_x = 1.; 
   m_scale_y = 1.; 
   m_translate_y = 0; 
   m_translate_x = 0; 
   m_angle = 0.;
   m_mirror_axis = Axis::None;
}

/**
 * ============================================================ 
 *   Transformations
 *   ---------------------------------------- 
 *   This function applies geometric transformations to the 
 *   image bytes using a kernel.
 *
 *   can use both GPU or CPU
 * ============================================================ 
 */
void PDI::transform() {

    // reset output
    input->copy_to( output );

    float buf[9] = {0};
    mat kernel   = identity_matrix(buf);

    // --- MIRROR ---
    if ( m_mirror_axis != Axis::None ) {
        float buf_m[9] = {0};
        mirror_matrix( m_mirror_axis, buf_m );
        mat mirror = mat3( buf_m, 3 );
        mat_mul_mut( &mirror, &kernel );
    }

    // --- SCALE ---
    if ( m_scale_x != 0 || m_scale_y != 0 ) {
        float buf_s[9] = {0};
        scale_matrix( m_scale_x, m_scale_y, buf_s );
        mat scale = mat3( buf_s, 3 );
        mat_mul_mut( &scale, &kernel );
    }

    // --- ROTATE ---
    if ( m_angle != 0 ) {
        float buf_r[9] = {0};
        rotation_matrix( m_angle, buf_r, Axis::Z );
        mat rot = mat3( buf_r, 3 );
        mat_mul_mut( &rot, &kernel );
    }

    // --- TRANSLATE ---
    if ( m_translate_x != 0 || m_translate_y != 0 ) {
        float buf_t[9] = {0};
        translate_matrix( m_translate_x, m_translate_y, buf_t );
        mat trans = mat3( buf_t, 3 );
        mat_mul_mut( &trans, &kernel );
    }

    // kernel.debug(); 
    
    auto in  = input->get_binary();
    auto out = output->get_binary();
    
    Benchmark::mark();

#if USE_GPU == 1 

    //  GPU computing
    // =====================================
    output->set_is_framebuffer(true);
    output->assert_fbo();

    auto win = get_glui()->get_window_size();

    compute_tex_quad( &output->m_fbo, glm::make_mat3x3( kernel.data ), output, &win );


#else
    //  CPU computing
    // =====================================
    //
    auto sz = input->get_size();
    int w = sz->width;
    int h = sz->height;
    auto ch = input->get_channel_count();

    // allocate output bufffer
    uint8_t* nout = (uint8_t *)calloc( w*h*ch, sizeof(uint8_t)); 
    // __simd_kernel_multiplication( &kernel, in, nout, sz, ch);
    // __isimd_kernel_multiplication( &kernel, in, nout, sz, ch);
     __dflt_kernel_multiplication( &kernel, in, nout, sz, ch);
    
    // this can be known using the same shader inside ImageHandler class; 
    output->set_is_framebuffer(false);

    output->set_binary(nout);
    output->request_texture_reload();
#endif 

    return;
}

void fill_holes(uint8_t* img, int w, int h, uint8_t ch) {
    std::vector<uint8_t> temp(img, img + w * h * ch);  // copy original image

    // essa função é mega burra; ela assume que um '0' é um buraco
    // TODO: implementar uma mascara; quando um pixel for setado corretamente
    // atualiza mascara, depois itera a imagem avaliando a validade atraves da
    // mascara
    int filled = 0;
    for (int i = 1; i < h - 1; i++) {
        for (int j = 1; j < w - 1; j++) {

            auto idx = (i * w + j) * ch;

            int chs = 0;
            for (int c = 0; c < ch; c++)
            {
                chs += img[idx + c]; 
            }

            bool is_hole = chs == 0;

            if (is_hole) {

                // interpolation
                for (int c = 0; c < ch; c++) {
                    for (int c = 0; c < ch; c++) {
                        int sum = 0;
                        int count = 0;
                    
                        // up
                        int val = temp[((i - 1) * w + j) * ch + c];
                        if (val != 0) { sum += val; count++; }
                    
                        // down
                        val = temp[((i + 1) * w + j) * ch + c];
                        if (val != 0) { sum += val; count++; }
                    
                        // left
                        val = temp[(i * w + j - 1) * ch + c];
                        if (val != 0) { sum += val; count++; }
                    
                        // right
                        val = temp[(i * w + j + 1) * ch + c];
                        if (val != 0) { sum += val; count++; }

                        img[idx + c] = count == 0 ? 0 : sum / count;
                    }
                }

                filled++;
            }
        }
    }
    std::cout << "Filled " << filled <<  " holes." << std::endl;
}


/**
 * this is not used for now
 */
void bilinear_interpolate(uint8_t* in, int w, int h, float x, float y, uint8_t* out, uint8_t ch) {
    int x0 = (int)floorf(x);
    int y0 = (int)floorf(y);
    int x1 = x0 + 1;
    int y1 = y0 + 1;

    float dx = x - x0;
    float dy = y - y0;

    x0 = (x0 < 0) ? 0 : (x0 >= w ? w - 1 : x0);
    y0 = (y0 < 0) ? 0 : (y0 >= h ? h - 1 : y0);
    x1 = (x1 < 0) ? 0 : (x1 >= w ? w - 1 : x1);
    y1 = (y1 < 0) ? 0 : (y1 >= h ? h - 1 : y1);

    for (int c = 0; c < ch; c++) {
        uint8_t p00 = in[(y0 * w + x0) * ch + c];
        uint8_t p10 = in[(y0 * w + x1) * ch + c];
        uint8_t p01 = in[(y1 * w + x0) * ch + c];
        uint8_t p11 = in[(y1 * w + x1) * ch + c];

        // interpolate hor
        float i0 = p00 + dx * (p10 - p00);
        float i1 = p01 + dx * (p11 - p01);

        // interpolate ver
        float value = i0 + dy * (i1 - i0);

        out[c] = (uint8_t)(value + 0.5f);
    }
}

/**
 * this is black magic
 * 
 * usei esse tipo de função pra otimizar na fase de testes em CPU;
 */
void __simd_kernel_multiplication( mat* r, uint8_t* in, uint8_t* nout, Size* sz, uint8_t ch ) {

    auto w = sz->width;
    auto h = sz->height;
    float cx = w / 2.0f;
    float cy = h / 2.0f;

    float f[3], _r[3];

    mat c = mat{ .data =  f, .r=3, .c=1 };
    mat m = mat{ .data = _r, .r=3, .c=1 };

    const float M00 = r->data[0], 
                M01 = r->data[1], 
                M02 = r->data[2];
    const float M10 = r->data[3], 
                M11 = r->data[4], 
                M12 = r->data[5];
    // z is perfectably ignorable :))
    // const float M20 = r->data[6], M21 = r->data[7], M22 = r->data[8];

    // Load matrix into SIMD
    __m256 M00_vec = _mm256_set1_ps(M00);
    __m256 M01_vec = _mm256_set1_ps(M01);
    __m256 M02_vec = _mm256_set1_ps(M02);
    __m256 M10_vec = _mm256_set1_ps(M10);
    __m256 M11_vec = _mm256_set1_ps(M11);
    __m256 M12_vec = _mm256_set1_ps(M12);

    // load constants
    __m256 half_vec = _mm256_set1_ps(0.5f);
    __m256 one_vec  = _mm256_set1_ps(1.0f);
    __m256 cx_vec   = _mm256_set1_ps(cx);
    __m256 cy_vec   = _mm256_set1_ps(cy);

    Benchmark::mark();

    for (int i = 0; i < h; i++) {

        __m256 f1_vec = _mm256_set1_ps(i + 0.5f - cy);

        for (int j = 0; j < w; j += 8) {

            // é little endian ta ligado
            __m256 j_vec1 = _mm256_set_ps( j+7,  j+6,  j+5,  j+4,  j+3,  j+2,  j+1, j+0);
        
            // f0 = (j + 0.5) - cx
            __m256 f0_1 = _mm256_sub_ps(_mm256_add_ps(j_vec1, half_vec), cx_vec);
        
            // compute transformed x, y coordinates
            __m256 x_vec1 = _mm256_add_ps(_mm256_add_ps(_mm256_mul_ps(M00_vec, f0_1), _mm256_mul_ps(M01_vec, f1_vec)), M02_vec);
            __m256 y_vec1 = _mm256_add_ps(_mm256_add_ps(_mm256_mul_ps(M10_vec, f0_1), _mm256_mul_ps(M11_vec, f1_vec)), M12_vec);
        
            // translate back
            x_vec1 = _mm256_add_ps(x_vec1, cx_vec);
            y_vec1 = _mm256_add_ps(y_vec1, cy_vec);
        
            // convert to integer 
            __m256i xd_vec1 = _mm256_cvtps_epi32(x_vec1);
            __m256i yd_vec1 = _mm256_cvtps_epi32(y_vec1);
        
            // store results
            int xd[8], 
                yd[8];

            _mm256_storeu_si256((__m256i *)(xd    ), xd_vec1);
            _mm256_storeu_si256((__m256i *)(yd    ), yd_vec1);
        
            for (int k = 0; k < 8; ++k) {
                int curr_j = j + k;
                int curr_i = i;
                int xd_val = xd[k], 
                    yd_val = yd[k];
        
                if (xd_val < 0 || xd_val >= w || yd_val < 0 || yd_val >= h){
                    continue;
                }
        
                // Compute 1D indices for input and output
                int i_at = ((h - 1 - curr_i) * w + curr_j) * ch;
                int o_at = ((h - 1 - yd_val) * w + xd_val) * ch;
        
                // Copy RGB channels separately
                nout[o_at + 0] = in[i_at + 0];  // Red
                nout[o_at + 1] = in[i_at + 1];  // Green
                nout[o_at + 2] = in[i_at + 2];  // Blue
            }
        }
    }

    std::cout <<  "SIMD: " << Benchmark::since_mark(Unit::Micro) << " us" << std::endl;
}



void __dflt_kernel_multiplication( mat* r, uint8_t* in, uint8_t* nout, Size* sz, uint8_t ch ) {

    auto w = sz->width;
    auto h = sz->height;
    float cx = w / 2.0f;
    float cy = h / 2.0f;

    float f[3], _r[3];

    mat c = mat{ .data = f, .r=3, .c=1 };
    mat m = mat{ .data = _r, .r=3, .c=1 };

    int xd = 0;
    int yd = 0;

    float x, y = 0;

    Benchmark::mark();

    for (int i = 0; i < h; i++)
    {
        for (int j = 0; j < w; j++)
        {
            // takes "middle of the pixel" into consideration
            f[0] = j + 0.5 - cx;
            f[1] = i + 0.5 - cy;
            f[2] = 1.0;

            c.data = f;

            mat_mul_33_31( r, &c, &m );

            float x = m.data[0];
            float y = m.data[1];

            x += cx;
            y += cy;

            xd = (int)(x);
            yd = (int)(y);

            // Discarding removes the weird edges that can happen if we dont discard 
            if ( xd < 0 || xd > w-1 || yd < 0  || yd > h-1 ) {
                continue;
            }

            // favor não ler o codigo abaixo

            auto i_at = (h-1-i)  * w + j;
            auto o_at = (h-1-yd) * w + xd;

            // map pixel 
            i_at *= ch; // RGB... RGBA ...
            o_at *= ch; // RGB... RGBA ...

            for (int C = 0; C < ch; C++)
            {
                nout[o_at + C] = in[i_at + C];
            }
        }
    }

    auto ms = Benchmark::since_mark(Unit::Milli);
    std::cout <<  "DONE. in " << ms << " ms" << std::endl;

    fill_holes( nout, w, h, ch );

    ms = Benchmark::since_mark(Unit::Milli);
    std::cout <<  "HOLES FILLED. in " << ms << " ms" << std::endl;
}



void __isimd_kernel_multiplication( mat* r, uint8_t* in, uint8_t* nout, Size* sz, uint8_t ch ) {

        float buf[9] = {0};
        mat r_inv= mat{.data=buf, .r=3, .c=3,};
        invert_affine_2x3(r, &r_inv);

        const int w = sz->width;
        const int h = sz->height;
        const float cx = w / 2.0f;
        const float cy = h / 2.0f;
    
        const float M00 = r_inv.data[0], M01 = r_inv.data[1], M02 = r_inv.data[2];
        const float M10 = r_inv.data[3], M11 = r_inv.data[4], M12 = r_inv.data[5];
    
        __m256 M00_vec = _mm256_set1_ps(M00);
        __m256 M01_vec = _mm256_set1_ps(M01);
        __m256 M02_vec = _mm256_set1_ps(M02);
        __m256 M10_vec = _mm256_set1_ps(M10);
        __m256 M11_vec = _mm256_set1_ps(M11);
        __m256 M12_vec = _mm256_set1_ps(M12);
    
        __m256 half_vec = _mm256_set1_ps(0.5f);
        __m256 cx_vec = _mm256_set1_ps(cx);
        __m256 cy_vec = _mm256_set1_ps(cy);
    
        Benchmark::mark();
    
        for (int yd = 0; yd < h; yd++) {
            __m256 y_vec = _mm256_set1_ps(yd + 0.5f - cy);
    
            for (int xd = 0; xd < w; xd += 8) {
                __m256 xd_vec = _mm256_set_ps(xd + 7, xd + 6, xd + 5, xd + 4, xd + 3, xd + 2, xd + 1, xd + 0);
                xd_vec = _mm256_add_ps(xd_vec, half_vec);
                xd_vec = _mm256_sub_ps(xd_vec, cx_vec);
    
                // Forward matrix to get source coords
                __m256 src_x = _mm256_add_ps(_mm256_add_ps(_mm256_mul_ps(M00_vec, xd_vec), _mm256_mul_ps(M01_vec, y_vec)), M02_vec);
                __m256 src_y = _mm256_add_ps(_mm256_add_ps(_mm256_mul_ps(M10_vec, xd_vec), _mm256_mul_ps(M11_vec, y_vec)), M12_vec);
    
                src_x = _mm256_add_ps(src_x, cx_vec);
                src_y = _mm256_add_ps(src_y, cy_vec);
    
                __m256 x_rounded = _mm256_round_ps(src_x, _MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC);
                __m256 y_rounded = _mm256_round_ps(src_y, _MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC);
    
                __m256i xs = _mm256_cvtps_epi32(x_rounded);
                __m256i ys = _mm256_cvtps_epi32(y_rounded);
    
                int x_s[8], y_s[8];
                _mm256_storeu_si256((__m256i*)x_s, xs);
                _mm256_storeu_si256((__m256i*)y_s, ys);
    
                for (int k = 0; k < 8; ++k) {
                    int x_src = x_s[k];
                    int y_src = y_s[k];
    
                    if (x_src < 0 || x_src >= w || y_src < 0 || y_src >= h) {
                        continue;
                    }
    
                    int i_at = ((h - 1 - y_src) * w + x_src) * ch;
                    int o_at = ((h - 1 - yd) * w + (xd + k)) * ch;
    
                    for (int c = 0; c < ch; c++) {
                        nout[o_at + c] = in[i_at + c];
                    }
                }
            }
        }
    
        std::cout << "SIMD INV: " << Benchmark::since_mark(Unit::Micro) << " us" << std::endl;
    
}


/**
 * this is a routine used to calculate 
 * 
 */
void PDI::test_math () {

    std::cout << "TESTING PDI MATH" << std::endl;

    float m[] = {
        1, 2, 1,
        0, 0, 1,
        1, 1, 0
    };

    float mc[] = {
        3, 6, 
        1, 2, 
        0, 0 
    };

    auto m3   = mat3( m  , 3);
    auto m1   = mat3( mc , 2);

    auto mx = mat_mul( &m3, &m1 );
    // mx.debug();
    // m3.debug();
    // m1.debug();

    // transform( 2, 270 );

}





/**
 * ===================================
 *         FUNÇÕES APOSENTADAS
 * ===================================
 */

void PDI::rotate() {

    std::cout << std::endl;
    std::cout << "================================="<< std::endl;
    std::cout << "PDI::TRANSFORM::ROTATE ::deg =" << m_angle << std::endl;
    std::cout << "================================="<< std::endl;

    float buf[9] = {0};
    rotation_matrix( m_angle, buf, Axis::Z );
    mat r = mat3( buf, 3);

    auto in  = output->get_binary();
    auto out = output->get_binary();
    
    Benchmark::mark();

    auto sz = input->get_size();
    int w = sz->width;
    int h = sz->height;
    auto ch = input->get_channel_count();

    auto nout = (uint8_t *)calloc( w*h*ch, sizeof(uint8_t)); //
    // posix_memalign((void **)&nout, 32, w*h*ch * sizeof(uint8_t));

    __simd_kernel_multiplication( &r, in, nout, sz, ch);

    output->set_binary(nout);
    output->request_texture_reload();

    // free( nout );

    return;
}

void PDI::scale() {

    std::cout << std::endl;
    std::cout << "================================="<< std::endl;
    std::cout << "PDI::TRANSFORM::SCALE ::fact =" << m_scale_x << ":" << m_scale_y << std::endl;
    std::cout << "================================="<< std::endl;

    float buf[9] = {0};
    scale_matrix( m_scale_x, m_scale_y, buf );
    mat r = mat3( buf, 3);

    auto in  = output->get_binary();
    auto out = output->get_binary();
    
    Benchmark::mark();

    auto sz = input->get_size();
    int w = sz->width;
    int h = sz->height;
    auto ch = input->get_channel_count();

    auto nout = (uint8_t *)calloc( w*h*ch, sizeof(uint8_t)); //

    __dflt_kernel_multiplication( &r, in, nout, sz, ch);

    output->set_binary(nout);
    output->request_texture_reload();
    
    return;
}
