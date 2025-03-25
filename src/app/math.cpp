#include <PDI/math.hpp>


/**
 * this global ensure the copied memory won't be clearead at runtime
 */
float __kernel_rot_x[9] = {
    1, 0, 0,
    0, 0, 0,
    0, 0 ,0
};
float __kernel_rot_y[9] = {
    0, 0, 0,
    0, 1, 0,
    0, 0 ,0
};
float __kernel_rot_z[9] = {
    0, 0, 0,
    0, 0, 0,
    0, 0 ,1
};
float __kernel_scale[9] = {
    0, 0, 0,
    0, 0, 0,
    0, 0, 1
};
float __kernel_mirror[9] = {
    1, 0, 0,
    0, 1, 0,
    0, 0, 1
};
float __kernel_translate[9] = {
    1, 0, 0,
    0, 1, 0,
    0, 0, 1
};


float rad( float deg) {
    return deg * M_PI / 180;
}

float sind( float deg ){
    return std::sin( rad(deg) );
}

float cosd( float deg ) {
    return std::cos( rad(deg) );
}

/**
 * 
 * ========================================
 *               MATRICES
 * ========================================
 * 
 */


void __mat_mul( mat *m1, mat *m2, float *buf);
void mat_mul_mut( mat *m1, mat *m2 );

// ========================================
void mat_mul_33_31( mat *ma1, mat *ma2, mat *res) {
    auto m1 = ma1->data;
    auto m2 = ma2->data;

    res->data[0] = m1[0] * m2[0] + m1[1] * m2[1] + m1[2] * m2[2];
    res->data[1] = m1[3] * m2[0] + m1[4] * m2[1] + m1[5] * m2[2];
    res->data[2] = m1[6] * m2[0] + m1[7] * m2[1] + m1[8] * m2[2];
}

mat mat::mul(  mat *other )  {
    return mat_mul(this, other);
}

void mat::mul_mut ( mat* m2 ) {
    // return mat_mul_mut( this, m2 );
}

void mat::copy_to( mat *other ) {
    cpbuffer( other->data, data, other->r * other->c );
    for ( auto i = 0; i < other->r * other->c; i++) {
        other->data[i] = data[i];
    } 
}

/**
 * basically memcpy2() :P
 */
void cpbuffer( float* from, float* to, size_t sz ) {
    for ( auto i = 0; i < sz; i++) {
        to[i] = from[i];
    } 
}

void mat_mul_mut( mat *m1, mat *m2 ) {
    float buf[ 16 ];

    __mat_mul( m1, m2, buf );

    std::memcpy(m2->data, buf, m2->r * m2->c * sizeof(float));
}

void __mat_mul( mat *m1, mat *m2, float *buf) {

    if ( m1->c != m2->r ) {
        return;
    }

    for ( auto i = 0; i < m2->r; i++ ) {

        for ( auto j = 0; j < m2->c; j++ ) {

            buf[ i * m2->c + j ] = 0;

            for ( auto k = 0; k < m1->c; k++ ) {

                buf[ i * m2->c + j ] += m1->data[ i * m1->c + k ] * m2->data[ k * m2->c + j ];
                
            }
        }

    }

    return;

    // if we were to alter the original, but i don't think this is 
    // a good choice 
    for ( auto i = 0; i < m2->r * m2->c; i++) {
        m2->data[i] = buf[i];
    } 
}



mat mat_mul( mat *m1, mat *m2 ) {

    float* buf = new float[ m1->r * m2->c ]();

    __mat_mul( m1, m2, buf );

    return mat{
        .data=buf,
        .r=m2->r,
        .c=m2->c,
        .heap = true
    };
}


void invert_affine_2x3(const mat* r, mat* r_inv) {
    float M00 = r->data[0], M01 = r->data[1], M02 = r->data[2];
    float M10 = r->data[3], M11 = r->data[4], M12 = r->data[5];

    float det = M00 * M11 - M01 * M10;

    // se o determinante estiver muito proximo de zero;
    if (fabsf(det) < 1e-8f) {
        std::cerr << "Singular matrix!" << std::endl;
        return;
    }

    float inv_det = 1.0f / det;

    // Inverse 2x2
    r_inv->data[0] =  M11 * inv_det;
    r_inv->data[1] = -M01 * inv_det;
    r_inv->data[3] = -M10 * inv_det;
    r_inv->data[4] =  M00 * inv_det;

    r_inv->data[2] = -(r_inv->data[0] * M02 + r_inv->data[1] * M12);
    r_inv->data[5] = -(r_inv->data[3] * M02 + r_inv->data[4] * M12);
}



mat mat1( float* values, uint8_t c) {

    return mat {
        .data = values,  
        .r = 1,
        .c = c
    };
} 

mat mat3( float* values, uint8_t c) {

    return mat {
        .data = values,  
        .r = 3,
        .c = c
    };
}

void scale_matrix(float sx, float sy, float *buf)
{
        auto sz = 9;
        // auto sz = sizeof(__kernel_rot_z) / sizeof( float );
        cpbuffer(__kernel_scale, buf, sz);

        buf[0] = sx;
        buf[4] = sy;
        buf[8] = 1.;
}

void rotation_matrix( float deg, float* buf, Axis type ) {

    auto s = sind( deg );
    auto c = cosd( deg );

    auto sz = sizeof(__kernel_rot_z) / sizeof( float );

    switch (type)
    {
    case X:
    {
        cpbuffer(__kernel_rot_x, buf, sz);
        buf[3] = c;
        buf[4] = -s;

        buf[6] = s;
        buf[7] = c;
        break;
    }

    case Y:
    {
        cpbuffer(__kernel_rot_y, buf, sz);
        buf[0] = c;
        buf[2] = s;

        buf[6] = -s;
        buf[8] = c;
        break;
    }
    case Z:
    {
        cpbuffer(__kernel_rot_z, buf, sz);
        buf[0] = c;
        buf[1] = -s;

        buf[3] = s;
        buf[4] = c;

        break;
    }
    }
}

void translate_matrix(float x, float y, float *buf)
{
        auto sz = 9;
        cpbuffer(__kernel_translate, buf, sz);

        buf[2] = x;
        buf[5] = y;
}

void mirror_matrix( Axis ax, float *buf)
{
    constexpr auto sz = 9;

    cpbuffer(__kernel_mirror, buf, sz);

    if (ax & Axis::X)
    {
        buf[0] = -1;
    }

    if (ax & Axis::Y)
    {
        buf[4] = -1;
    }
}

/**
 * its a 3x3
 */
mat identity_matrix ( float* buf )  {

    mat kernel = mat3( buf, 3 );

    buf[0] = 1.;
    buf[4] = 1.;
    buf[8] = 1.;

    return kernel;
}