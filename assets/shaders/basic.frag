#version 430 core
out vec4 FragColor;
//layout(location = 0) uniform vec4 ourColor;
uniform vec4 ourColor;

void main()
{
    FragColor = ourColor;
}