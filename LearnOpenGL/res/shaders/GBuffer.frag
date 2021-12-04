#version 450 core
layout (location = 0) out vec3 gPosition;
// layout (location = 0) out vec4 gPosition; // using vec4 will allow to give alpha vlaue of 1.0f and work under glEnable(GL_BLEND)

layout (location = 1) out vec3 gNormal;
// layout (location = 1) out vec4 gNormal; // Same here

layout (location = 2) out vec4 gAlbedoSpec; // Here cannot use Blending because the ALPHA channel is blended.

in VERT_OUT
{
    vec3 FragmentPos;
    vec3 Normal;
    vec2 TexCoords;

} frag_in;

#define NR_TEX_DIFF 10
#define NR_TEX_SPEC 10
uniform int TEX_DIFF_COUNT;
uniform int TEX_SPEC_COUNT;
uniform sampler2D texture_diffuse[NR_TEX_DIFF];
uniform sampler2D texture_specular[NR_TEX_SPEC];

void main()
{    
    gPosition = frag_in.FragmentPos;
    // gPosition = vec4(frag_in.FragmentPos, 1.0f);
    gNormal = normalize(frag_in.Normal);
    // gNormal = vec4(normalize(frag_in.Normal), 1.0f);

    gAlbedoSpec.rgb = texture(texture_diffuse[0], frag_in.TexCoords).rgb;
    gAlbedoSpec.a = texture(texture_specular[0], frag_in.TexCoords).r;
}  