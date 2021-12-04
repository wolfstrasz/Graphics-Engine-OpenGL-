#version 450 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} vs_out;

layout(std140, binding = 0) uniform CameraData 
{ 
    mat4 projection;
    mat4 view;
};

uniform mat4 model;
uniform mat3 tiModel;

void main()
{
    vs_out.FragPos = vec3(model * vec4(aPos, 1.0));   
    vs_out.TexCoords = aTexCoords;
        
    vs_out.Normal = normalize(tiModel * aNormal);
    
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}