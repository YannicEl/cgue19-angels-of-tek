#version 430 core

in VertexData {
    vec3 position_world;
    vec3 normal_world;
    vec2 uv;
} vert;

uniform vec3 diffuseColor;
uniform vec3 cameraWorldPosition;
uniform vec3 materialCoefficients; // x = ambient, y = diffuse, z = specular 
uniform float specularAlpha;
uniform sampler2D colorTexture;

out vec4 FragColor;

void main()
{
    vec3 texColor = texture(colorTexture, vert.uv).rgb;
    FragColor = vec4(texColor * materialCoefficients.x, 1);
    //FragColor = vec4(diffuseColor, 1.0);
}