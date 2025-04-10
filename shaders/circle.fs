#version 330 core

in vec2 UV;  // Must be in range [0,1]
out vec4 FragColor;

uniform vec4 color; // (r, g, b, a)
uniform vec4 rect; 

void main() {

    vec2 centered = UV * 2.0 - 1.0;

    float dist = length(centered);

    float alpha = smoothstep(0.8, 1., dist);

    // FragColor = vec4( color.rgb, 1. - step( 1., dist ));
    FragColor = vec4( color.rgb, 1.-alpha);
}