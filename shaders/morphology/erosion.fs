#version 330 core

out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D ourTexture;
uniform vec2 texelSize;
uniform float val;
uniform int kernel_type;


void main () {

    vec2 uv = gl_FragCoord.xy * texelSize;

    // começa com valor central sempre 
    vec4 d = texture( ourTexture, uv );

    if (kernel_type == 0) {
        vec2 offsets[4] = vec2[4](
            vec2(0., 1.0),  
            vec2(-1.0, 0.0),   
            vec2(1.0, 0.0),  
            vec2(0.0, -1.0)    
        );

        for (int i = 0; i < 4; ++i) {
            vec2 offset = offsets[i] * texelSize;
            vec4 sample = texture(ourTexture, uv + offset);
            d = min(d, sample);  
        }
    } else {
        for (int x = -1; x <= 1; ++x) {
            for (int y = -1; y <= 1; ++y) {
                vec2 offset = vec2(x, y) * texelSize;
                vec4 sample = texture(ourTexture, uv + offset);
                d = min(d, sample);  
            }
        }
    }

    FragColor = vec4(vec3(d), 1.);

    return;
}