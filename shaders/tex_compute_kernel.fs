#version 330 core

out vec4 FragColor;
  
// in vec3 ourColor;
in vec2 TexCoord;

uniform vec2 u_texSize;
uniform sampler2D ourTexture;
uniform mat3 u_kernel;

void main()
{
    // this is stupid... but since im stupid
    // it makes perfect sense;
    // gl origin is bottom-left, we need to flip it;
    // and here is easier :P
    //                              v
    vec2 tc = TexCoord;
    tc.y = 1. - tc.y;
    
    vec2 centered = tc - 0.5;
    vec2 mappedCoord;

    const bool USE_INVERSE_MAPPING = true;
    if ( USE_INVERSE_MAPPING ) {

        // invert translation to "immune" it against the next inverse function
        vec2 translationUV = vec2(-u_kernel[0][2], u_kernel[1][2]) / u_texSize; 

        mat3 kernel = inverse(u_kernel);

        vec3 coord = kernel * vec3( centered, 1.0 );

        mappedCoord = coord.xy + .5 + translationUV;

    } else {
        vec3 coord = u_kernel * vec3(centered, 1.0);

        vec2 mappedCoord = coord.xy + 0.5;
    }

    if (mappedCoord.x < 0.0 || mappedCoord.x > 1.0 || mappedCoord.y < 0.0 || mappedCoord.y > 1.0) {
        discard;
    }

    FragColor = texture( ourTexture, mappedCoord );
}