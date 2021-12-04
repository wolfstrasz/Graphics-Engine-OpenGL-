#version 450 core
out vec4 FragColor;

uniform vec3 lamp_colour = vec3(1.0f);
void main()
{
    FragColor = vec4(lamp_colour, 1.0); // set alle 4 vector values to 1.0
}