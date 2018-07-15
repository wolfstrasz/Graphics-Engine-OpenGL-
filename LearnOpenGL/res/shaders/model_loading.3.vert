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
// Exit
out vec3 FragmentPos;
out vec3 Normal;
out vec2 TexCoords;
out vec3 Tangent;
out vec3 Bitangent;

void main()
{
    // Get fragment position
    FragmentPos = vec3(model * vec4(aPos, 1.0));
    // Get the fragment normal in world space
    Normal = tiModel * aNormal; 
    // Forward Texture Coordinates 
    TexCoords = aTexCoords;
    // Get clip position of vector
    gl_Position = projection * view * vec4(FragmentPos, 1.0);

    Tangent = aTangent;
    Bitangent = aBitangent;
}