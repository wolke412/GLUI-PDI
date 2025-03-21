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

    // FragColor = texture( ourTexture, tc );
    // return

    // invert translation to "immune" it against the next inverse function
    vec2 translationUV = vec2(-u_kernel[0][2], u_kernel[1][2]) / u_texSize; 

    mat3 kernel = inverse(u_kernel);

    vec2 centered = tc - .5;

    vec3 coord = kernel * vec3( centered, 1.0 );

    vec2 mappedCoord = coord.xy + .5 + translationUV;

    if ( mappedCoord.x < 0. || mappedCoord.x > 1. ||  mappedCoord.y < 0. || mappedCoord.y > 1. ) {
        discard;
    }

    FragColor = texture( ourTexture, mappedCoord );
}