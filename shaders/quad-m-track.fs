#version 330 core

uniform vec2 mousePos;   
uniform vec2 quadPos;    
uniform vec2 quadSize;   
out vec4 FragColor;

void main() {
    vec2 fragPos = gl_FragCoord.xy; 

    vec2 inQuad = step(quadPos, fragPos) * step(fragPos, quadPos + quadSize);
    float insideQuad = inQuad.x * inQuad.y;

    float distanceToMouse = length(fragPos - mousePos);
    
    float radius = 20.0;
    float alpha = smoothstep(radius, radius - 3.0, distanceToMouse);

    FragColor = vec4(vec3(1.0), alpha * insideQuad);
}
