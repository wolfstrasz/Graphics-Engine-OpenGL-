#version 330 core
out vec4 FragColor;
#define NR_TEX_DIFF 10
in vec2 TexCoords;

uniform sampler2D texture_diffuse[NR_TEX_DIFF];

void main()
{
    FragColor = texture(texture_diffuse[0], TexCoords);
}