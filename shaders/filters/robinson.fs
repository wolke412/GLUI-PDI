#version 330 core


const float kernel_1[9] = float[9]( // north
    -1.0,  0.0,  1.0,
    -2.0,  0.0,  2.0,
    -1.0,  0.0,  1.0
);

const float kernel_2[9] = float[9]( // north-east
     0.0,  1.0,  2.0,
    -1.0,  0.0,  1.0,
    -2.0, -1.0,  0.0
);

const float kernel_3[9] = float[9]( // east
     1.0,  2.0,  1.0,
     0.0,  0.0,  0.0,
    -1.0, -2.0, -1.0
);

const float kernel_4[9] = float[9]( // south-east
     2.0,  1.0,  0.0,
     1.0,  0.0, -1.0,
     0.0, -1.0, -2.0
);

const float kernel_5[9] = float[9]( // south
     1.0,  0.0, -1.0,
     2.0,  0.0, -2.0,
     1.0,  0.0, -1.0
);

const float kernel_6[9] = float[9]( // south-west
     0.0, -1.0, -2.0,
     1.0,  0.0, -1.0,
     2.0,  1.0,  0.0
);

const float kernel_7[9] = float[9]( // west
    -1.0, -2.0, -1.0,
     0.0,  0.0,  0.0,
     1.0,  2.0,  1.0
);

const float kernel_8[9] = float[9]( // north-west
    -2.0, -1.0,  0.0,
    -1.0,  0.0,  1.0,
     0.0,  1.0,  2
);

out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D ourTexture;
uniform vec2 texelSize;
uniform float threshold;

void main() {

    vec2 uv = gl_FragCoord.xy * texelSize;

    const int KERNELSZ = 3;

    float g = 0.;
    float gs[8] = float[](0.,0.,0.,0.,0.,0.,0.,0.);

    for( int i = 0; i < KERNELSZ; i++ ) {
        for( int j = 0; j < KERNELSZ; j++ ) {

            vec2 offset = vec2(float(i-1), float(j-1)) * texelSize;

            float R = texture(ourTexture, uv + offset).r;

            gs[0] += R * kernel_1[i*3+j];
            gs[1] += R * kernel_2[i*3+j];
            gs[2] += R * kernel_3[i*3+j];
            gs[3] += R * kernel_4[i*3+j];
            gs[4] += R * kernel_5[i*3+j];
            gs[5] += R * kernel_6[i*3+j];
            gs[6] += R * kernel_7[i*3+j];
            gs[7] += R * kernel_8[i*3+j];

            // sobelx += R * kernel_x[i*3+j];
            // sobely += R * kernel_y[i*3+j];
        }
    }

    g =        gs[0];
    g = max(g, gs[1]);
    g = max(g, gs[2]);
    g = max(g, gs[3]);
    g = max(g, gs[4]);
    g = max(g, gs[5]);
    g = max(g, gs[6]);
    g = max(g, gs[7]);

    if ( g > threshold ) {
        g = 1.;
    }

    FragColor = vec4(vec3( g ), 1.);
}