#version 450 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

layout (location = 5) in mat4 instanceModelMatrix;
//5 instanceModelMatrix row 1
//6 instanceModelMatrix row 2
//7 instanceModelMatrix row 3
//8 instanceModelMatrix row 4
layout (location = 9) in mat3 tiInstanceModelMatrix;
// 9  tiInstanceModelMatrix row 1
// 10 tiInstanceModelMatrix row 2
// 11 tiInstanceModelMatrix row 3


layout(std140, binding = 0) uniform Matrices 
{ 
    mat4 projection;
    mat4 view;
};


uniform mat4 baseModel;
uniform mat3 tiBaseModel;

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
    vert_out.FragmentPos = vec3(( instanceModelMatrix * baseModel )* vec4(aPos, 1.0));
    // Get the fragment normal in world space
    vert_out.Normal = (tiInstanceModelMatrix * tiBaseModel) * aNormal; 
    // Forward Texture Coordinates 
    vert_out.TexCoords = aTexCoords;
    // Forward tangents
    vert_out.Tangent = aTangent;
    // Forward bitanges
    vert_out.Bitangent = aBitangent;

    // Get clip position of vector
    gl_Position = projection * view * vec4(vert_out.FragmentPos, 1.0);
}