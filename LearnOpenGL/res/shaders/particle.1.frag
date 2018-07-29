#version 450 core


out vec4 FragColor;

uniform vec3 colour;
uniform int pointsCount;
flat in int vertID; // flat because data from vert to frag has a 2:1 mapping
void main()
{    
    float deAlpha = float(vertID) / float(pointsCount);

    FragColor = vec4(colour, 1.0f * (1.0f - deAlpha));
}