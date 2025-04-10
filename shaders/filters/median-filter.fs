#version 330 core

// Maximum number of samples for a 7x7 kernel.
const int MAX_KERNEL_SIZE = 49;
  
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D ourTexture;
uniform int kernel_size;
uniform vec2 texelSize;

float median(float values[MAX_KERNEL_SIZE], int count) {

    for (int i = 0; i <= count / 2; ++i) {
        float minVal = values[i];
        int minIndex = i;

        for (int j = i + 1; j < count; ++j) {
            float curr = values[j];
            float newMin = min(minVal, curr);
            float isNewMin = float(curr < minVal);

            minVal = newMin;
            minIndex = int(mix(float(minIndex), float(j), isNewMin));
        }

        // Swap values[i] and values[minIndex]
        float a = values[i];
        float b = values[minIndex];
        float doSwap = float(i != minIndex);
        values[i] = mix(a, b, doSwap);
        values[minIndex] = mix(b, a, doSwap);
    }

    int mid = count / 2;
    return (count % 2 == 1) ? values[mid]
                            : 0.5 * (values[mid - 1] + values[mid]);
}

void median_rgb(
    inout float r[MAX_KERNEL_SIZE],
    inout float g[MAX_KERNEL_SIZE],
    inout float b[MAX_KERNEL_SIZE],
    int count
) {
    for (int i = 0; i <= count / 2; ++i) {
        float minVal = r[i];
        int minIndex = i;

        for (int j = i + 1; j < count; ++j) {
            if (r[j] < minVal) {
                minVal = r[j];
                minIndex = j;
            }
        }

        // Swap all 3 channels at once
        float tmp_r = r[i];
        float tmp_g = g[i];
        float tmp_b = b[i];

        r[i] = r[minIndex];
        g[i] = g[minIndex];
        b[i] = b[minIndex];

        r[minIndex] = tmp_r;
        g[minIndex] = tmp_g;
        b[minIndex] = tmp_b;
    }
}



void main() {

    // FragColor = vec4(1.0, 0.3, 0.5, 1.);
    // return;

    vec2 uv = TexCoord;
    
    int halfSize = kernel_size / 2;
    // const int halfSize = 3;

    int count = kernel_size * kernel_size;
    
    float samples_r[MAX_KERNEL_SIZE];
    float samples_g[MAX_KERNEL_SIZE];
    float samples_b[MAX_KERNEL_SIZE];

    int index = 0;
    
    for (int y = -halfSize; y <= halfSize; ++y) {
        for (int x = -halfSize; x <= halfSize; ++x) {
            vec2 offset = vec2(float(x), float(y)) * texelSize;
            vec3 col = texture(ourTexture, uv + offset).rgb;

            samples_r[index] = col.r;
            samples_g[index] = col.g;
            samples_b[index] = col.b;
            ++index;
        }
    }

    // float median_r = median(samples_r, count);
    // float median_g = median(samples_g, count);
    // float median_b = median(samples_b, count);

    median_rgb( samples_r, samples_g, samples_b, count );

    FragColor = vec4( samples_r[count >> 1], samples_g[count >> 1], samples_b[ count >> 1], 1.0);
    
}