#version 330 core

out vec4 FragColor;
  
in vec2 TexCoord;

uniform sampler2D ourTexture;
uniform float threshold;

void main()
{

    vec2 tc = TexCoord;
    vec4 px = texture(ourTexture, tc);

    vec3 tpx = clamp(px.xyz + step(threshold, px.xyz ), 0., 1.) ; 

    FragColor = vec4( tpx, 1.0 );
}