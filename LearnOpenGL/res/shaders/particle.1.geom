#version 450 core
layout (points) in;
//layout (line_Strip, max_vertices = 2) out;
layout (points, max_vertices = 1) out;

in VS_OUT{
    int vertID;
} gs_in[];

flat out int vertID;
void main() {    
    gl_Position = gl_in[0].gl_Position + vec4(-0.1, 0.0, 0.0, 0.0);
    gl_PointSize = gl_in[0].gl_PointSize; 
    vertID = gs_in[0].vertID;
    EmitVertex();
    EndPrimitive();
}    