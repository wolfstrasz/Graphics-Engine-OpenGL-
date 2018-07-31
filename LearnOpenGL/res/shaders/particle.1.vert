#version 450 core
layout (location = 0) in vec3 aPos;
layout(std140, binding = 0) uniform Matrices 
{ 
    mat4 projection;
    mat4 view;
};
uniform mat4 model;
//uniform mat4 projection;
//uniform mat4 view;
out VS_OUT {
    int vertID;
} vs_out;
void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);    
    gl_PointSize = 5.0f;
    vs_out.vertID = gl_VertexID;
}  