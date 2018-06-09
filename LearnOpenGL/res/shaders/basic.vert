#version 450 core
layout (location = 0) in vec3 aPos;

/************************************************************************/
/*							BASIC VERTEX SHADER							*/
/************************************************************************/
void main()
{
    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
}