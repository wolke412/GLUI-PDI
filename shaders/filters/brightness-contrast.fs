#version 330 core

out vec4 FragColor;
  
in vec2 TexCoord;

uniform sampler2D ourTexture;
uniform float brightness;
uniform float contrast;


float calc( float px, float B, float C ) {
    return C * px + B; 
}

vec4 apply( vec4 px, float B, float C ) {

    px.x = calc( px.x, B, C);
    px.y = calc( px.y, B, C);
    px.z = calc( px.z, B, C);

    return px;
}

void main()
{
    vec2 tc = TexCoord;
    vec4 px = texture(ourTexture, tc);

    vec4 tpx = apply( px, brightness, contrast );

    FragColor = tpx; 
}