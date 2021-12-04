#version 450 core
// Entering
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

// Global
layout(std140, binding = 0) uniform Matrices 
{ 
    mat4 projection;
    mat4 view;
};

uniform mat4 model;
uniform mat3 tiModel;

out VERT_OUT
{
    vec3 FragmentPos;
    vec3 Normal;
    vec2 TexCoords;
} vert_out;


void main()
{
    vert_out.FragmentPos = vec3(model * vec4(aPos, 1.0));
    vert_out.Normal = tiModel * aNormal; 
    vert_out.TexCoords = aTexCoords;
    // Get clip position of vector
    gl_Position = projection * view *  vec4(vert_out.FragmentPos, 1.0);

}