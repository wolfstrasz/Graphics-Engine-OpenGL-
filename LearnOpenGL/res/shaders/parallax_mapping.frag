#version 450 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
} fs_in;

uniform sampler2D diffuseMap;
uniform sampler2D normalMap;
uniform sampler2D depthMap;

uniform float heightScale;

vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir)
{
    // sample the height from Depth map at the current fragment
    float height =  texture(depthMap, texCoords).r;
    // calculate vector P (as XY component of ViewDir in tangent space)
    vec2 p = viewDir.xy;
    // to make the effect smoother, as when viewDir is largelt parallel to the surface z -> 0.0f
    // a.k.a we offset the texture coordinates at larger scale when we are at lower angles
    p /= viewDir.z;    // z is in [0:1] (tangent space)
    // scale p by the depth (we are using inverse heightmap) sample
    p *= height;
    // sometimes the height component has too strong effect so we use a scaling constant to lower it
    p *= heightScale;
    // return the proper textCoordinates at the calculated depth
    return texCoords - p;
}
void main()
{          
    // offset texture coordinates with Parallax Mapping
    vec3 viewDir   = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);
    vec2 texCoords = ParallaxMapping(fs_in.TexCoords,  viewDir);
    // discard fragments that get oversampled outside the range [0,1];
    if(texCoords.x > 1.0 || texCoords.y > 1.0 || texCoords.x < 0.0 || texCoords.y < 0.0)
        discard;
    // then sample textures with new texture coords
    vec3 color = texture(diffuseMap, texCoords).rgb;
    vec3 normal  = texture(normalMap, texCoords).rgb;
    normal = normalize(normal * 2.0 - 1.0);  // transform normal vector to range [-1,1] in tangent space

    // light's color
    vec3 lightColor = vec3(1.0f);
    // ambient
    vec3 ambient = 0.1 * color * lightColor;
    // diffuse
    vec3 lightDir = normalize(fs_in.TangentLightPos - fs_in.TangentFragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * color * lightColor;
    // specular
    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
    vec3 specular = vec3(0.2) * spec * lightColor;

    FragColor = vec4(ambient + diffuse + specular, 1.0);
}