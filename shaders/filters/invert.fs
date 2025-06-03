#version 330 core

out vec4 FragColor;
  
in vec2 TexCoord;

uniform sampler2D ourTexture;

void main()
{
    vec2 tc = TexCoord;
    vec4 px = texture(ourTexture, tc);

    FragColor = vec4( 1. - px.xyz, 1.);
    return;
}