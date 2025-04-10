/**
  THIS SHADER IS SUPPOSED TO EVALUATE MULTIPLE CONVOLUTION
  
*/
#version 330 core

// maximum number of samples for a 7x7 kernel.
const int MAX_KERNEL_SIZE = 49;
const float GAUSSIAN[25] = float[](
    1.,  4.,  7.,  4., 1., 
    4., 16., 26., 16., 4., 
    7., 26., 41., 26., 7.,
    4., 16., 26., 16., 4.,
    1.,  4.,  7.,  4., 1.
);
  
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D ourTexture;
// uniform int kernel_size;
uniform vec2 texelSize;

float gaussian(in float values[MAX_KERNEL_SIZE], int count) {
    float soma = 0.;
    for (int i = 0; i < count; i++) {
        soma += values[i] * GAUSSIAN[i];
    }
    return soma / 273.;
}

void main() {

    vec2 uv = gl_FragCoord.xy * texelSize;

    int kernel_size = 5; 
    int half_kernel = kernel_size / 2;

    int count = kernel_size * kernel_size;
    
    float samples_r[MAX_KERNEL_SIZE];
    float samples_g[MAX_KERNEL_SIZE];
    float samples_b[MAX_KERNEL_SIZE];

    int index = 0;
    
    
    for (int y = -half_kernel; y <= half_kernel; y++) {
        for (int x = -half_kernel; x <= half_kernel; x++) {
            vec2 offset = vec2(float(x), float(y)) * texelSize;
            samples_r[index] = texture(ourTexture, uv + offset).r;
            samples_g[index] = texture(ourTexture, uv + offset).g;
            samples_b[index] = texture(ourTexture, uv + offset).b;
            index++;
        }
    }
    
    float gauss_r = gaussian(samples_r, count);
    float gauss_g = gaussian(samples_g, count);
    float gauss_b = gaussian(samples_b, count);

    FragColor = vec4(gauss_r, gauss_g, gauss_b, 1.0);
}