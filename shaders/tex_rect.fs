#version 330 core

out vec4 FragColor;
  
// in vec3 ourColor;
in vec2 TexCoord;

uniform sampler2D ourTexture;

void main()
{
    // this is stupid... but since im stupid
    // it makes perfect sense;
    // gl origin is bottom-left, we need to flip it;
    // and here is easier :P
    //                              v
    vec2 tc = TexCoord;

    tc.y = 1. - tc.y ;

    FragColor = texture( ourTexture, tc);
}