#version 330 core

in vec2 TexCoord;      // Texture coordinates passed from vertex shader
out vec4 FragColor;    // Final output color

uniform sampler2D texInput;  // Input image texture
uniform mat3 mtx;          // 3x3 transformation matrix
uniform vec2 center;       // Center for transformation

void main()
{
    FragColor = vec4(1.0);
    return;
    // Apply the transformation matrix to the texture coordinates
    vec3 transformed = mtx * vec3(TexCoord.x - center.x, TexCoord.y - center.y, 1.0);
    
    // Add back the center to move the transformed coordinates into the original space
    vec2 newPos = transformed.xy + center;

    // Sample the texture at the new coordinates
    vec4 pixelColor = texture(texInput, newPos);
    
    // Output the color (you can do additional computations if needed)
    FragColor = pixelColor;
}