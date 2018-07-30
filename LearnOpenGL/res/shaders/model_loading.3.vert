#version 450 core
// Entering
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

// Global
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 tiModel;
// Exit with interface 
out VERT_OUT
{
vec3 FragmentPos;
vec3 Normal;
vec2 TexCoords;
vec3 Tangent;
vec3 Bitangent;
} vert_out;


void main()
{
    // Get fragment position
    vert_out.FragmentPos = vec3(model * vec4(aPos, 1.0));
    // Get the fragment normal in world space
    vert_out.Normal = tiModel * aNormal; 
    // Forward Texture Coordinates 
    vert_out.TexCoords = aTexCoords;
    // Get clip position of vector
    gl_Position = projection * view * vec4(vert_out.FragmentPos, 1.0);

    vert_out.Tangent = aTangent;
    vert_out.Bitangent = aBitangent;
}