#version 330 core

out vec4 FragColor;
  
in vec2 TexCoord;

uniform sampler2D ourTexture;
uniform vec3 weights;

void main()
{
    vec2 tc = TexCoord;
    vec4 px = texture(ourTexture, tc);
    vec3 pondered = px.xyz * weights;
    float s = pondered.x + pondered.y + pondered.z;
    float m = s / 3.;

    FragColor = vec4( vec3(m), 1.0 );
}