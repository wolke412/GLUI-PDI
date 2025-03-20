#ifndef PDI_MATH_H
#define PDI_MATH_H

#include <cstdint>
#include <iostream>

#include <cmath>
#include <cstring>

#define INVALID_MAT  mat{.data=nullptr,.r=0,.c=0}

enum Axis {
    X  = 1 << 0,
    Y  = 1 << 1,
    Z  = 1 << 2,
};

extern float __kernel_rot_x[9];
extern float __kernel_rot_y[9];
extern float __kernel_rot_z[9];

extern float __kernel_scale[9];
extern float __kernel_mirror[9];
extern float __kernel_translate[9];

struct mat {
    float* data;
    uint8_t r;
    uint8_t c;
    bool heap = false;
 
    mat mul ( mat* m2 );
    void mul_mut ( mat* m2 );
    void copy_to( mat *other );

    void debug(  ) {

        std::cout << "Mat " << (int)r << " by " << (int)c << "{"<< std::endl;

        for( int i = 0 ; i < r ; i++ ) {
            for (int j = 0; j < c; j++)

            {
                std::cout << "\t" << data[ i * c + j];
            }

            std::cout << std::endl;
        }

        std::cout << "}" << std::endl;
    }


    ~mat() {
        // std::cout << "DESCTRUCTING MAT" << std::endl;
        if ( heap ) {
            // std::cout << "Destroy mat ::HEAP" << std::endl;
            delete[] data;
        }
    }
};



mat mat1( float* values, uint8_t c);
// mat mat1( float* values, uint8_t c);
mat mat3( float* values, uint8_t c);

mat mat_mul( mat *m1, mat *m2 );
// void mat_mul_mut( mat *m1, mat *m2 );



void invert_affine_2x3(const mat* r, mat* r_inv);

void mat_mul_mut( mat *m1, mat *m2 );
void mat_mul_33_31( mat *ma1, mat *ma2, mat *res);

void cpbuffer( float* from, float* to, size_t sz );


mat identity_matrix( float* buf );

void rotation_matrix( float deg , float* buf, Axis e);
void scale_matrix( float sx , float sy, float* buf );
void mirror_matrix( Axis axis, float* buf );
void scale_matrix( float sx , float sy, float* buf );
void translate_matrix( float x , float y, float* buf );

void coord_matrix( float x, float y );


float rad( float deg )  ;
float sind( float deg );
float cosd( float deg );

#endif