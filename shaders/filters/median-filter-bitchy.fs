#version 330 core

const int MAX_KERNEL_SIZE = 49;
const int KERNEL_SIZE = 7; 

out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D ourTexture;
uniform int kernel_size;
uniform vec2 texelSize;

// ——— Compile‑time constants ———
const int KSIZE = 7;                // your 7×7 kernel
const int COUNT = KSIZE * KSIZE;    // 49 samples
const int MID   = COUNT >> 1;       // 24

float median(in float v[COUNT]) {
    for (int i = 0; i < COUNT - 1; ++i) {
        for (int j = i + 1; j < COUNT; ++j) {
            float a = v[i];
            float b = v[j];
            v[i] = min(a, b);
            v[j] = max(a, b);
        }
    }
    return v[MID];
}

void main() {

    vec2 uv = gl_FragCoord.xy * texelSize;
    
    // int halfSize = kernel_size / 2;
    int halfSize = KSIZE / 2;
    
    float samples_r[MAX_KERNEL_SIZE];
    float samples_g[MAX_KERNEL_SIZE];
    float samples_b[MAX_KERNEL_SIZE];

    int index = 0;

   for (int y = -halfSize; y <= halfSize; y++) {
        for (int x = -halfSize; x <= halfSize; x++) {
            vec2 offset = vec2(float(x), float(y)) * texelSize;
            vec2 sample = clamp(uv + offset, vec2(0.), vec2(1.));
            samples_r[index] = texture(ourTexture, sample).r;
            samples_g[index] = texture(ourTexture, sample).g;
            samples_b[index] = texture(ourTexture, sample).b;
            index++;
        }
    }
    
    float median_r = median(samples_r);
    float median_g = median(samples_g);
    float median_b = median(samples_b);

    FragColor = vec4(median_r, median_g, median_b, 1.0);
    
}