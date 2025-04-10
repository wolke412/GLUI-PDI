#version 330 core

// maximum number of samples for a 7x7 kernel.
const int MAX_KERNEL_SIZE = 49;
const float kernel_x[3*3] = float[](
    1., 0., -1.,
    2., 0., -2.,
    1., 0., -1.
);

const float kernel_y[3*3] = float[](
     1.,  2.,  1.,
     0.,  0.,  0.,
    -1., -2., -1.
);
  
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D ourTexture;
uniform vec2 texelSize;
uniform float threshold;

void main() {

    // FragColor = vec4(1.);
    // return;

    vec2 uv = gl_FragCoord.xy * texelSize;

    const int KERNELSZ = 3;

    float sobelx = 0.;
    float sobely = 0.;

    for( int i = 0; i < KERNELSZ; i++ ) {
        for( int j = 0; j < KERNELSZ; j++ ) {
            vec2 offset = vec2(float(i-1), float(j-1)) * texelSize;
            float R = texture(ourTexture, uv + offset).r;

            sobelx += R * kernel_x[i*3+j];
            sobely += R * kernel_y[i*3+j];
        }
    }

    float sob = sqrt(pow(sobelx, 2) + pow(sobelx, 2));

    if ( sob > threshold ) {
        sob = 1.;
    }else {
        sob = 0.;
    }
    FragColor = vec4(vec3( sob ), 1.);
}