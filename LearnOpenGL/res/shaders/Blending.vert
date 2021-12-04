#version 450 core
in vec3 aPos;
in vec2 aTexCoords;

out vec2 TexCoords;

layout(std140, binding = 0) uniform Matrices 
{ 
    mat4 projection;
    mat4 view;
};

uniform mat4 model;


void main()
{
    TexCoords = aTexCoords;
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}