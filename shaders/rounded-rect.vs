#version 330
    
layout (location = 0) in vec2 aPos; // e.g., [-1,1] normalized quad positions
layout (location = 1) in vec2 aUV;  // pass UVs explicitly

out vec2 vUV;

uniform vec2 offset;
uniform vec2 size;
uniform vec2 resolution;

void main()
{
    // Convert to clip space [-1, 1] with offset and size in pixel space

    vec2 pos = offset + aPos * size;
    vec2 ndc = (pos / resolution) * 2.0 - 1.0;
    ndc.y = -ndc.y; 

    // gl_Position = vec4(ndc, 0.0, 1.0);
    gl_Position = vec4(aPos, 0.0, 1.0);
    vUV = aUV; 
}
