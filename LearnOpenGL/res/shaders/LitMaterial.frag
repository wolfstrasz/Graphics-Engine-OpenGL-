#version 450 core
// STRUCTURE DEFINITIONS
// ---------------------
struct DirLight {
    vec3 direction;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
}; 

struct PointLight {    
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;  

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;
  
    float constant;
    float linear;
    float quadratic;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;       
};

#define NR_POINT_LIGHTS 10
#define NR_DIR_LIGHTS 10
#define NR_SPOT_LIGHTS 10
#define NR_TEX_DIFF 10
#define NR_TEX_SPEC 10

// ENTERING via Interface
// --------
in VERT_OUT
{
    vec3 FragmentPos;
    vec3 Normal;
    vec2 TexCoords;
    vec3 Tangent;
    vec3 Bitangent;
} frag_in;

struct Material
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 emissive;
    float shininess;

};

struct FragTextures {
    vec3 diffuse;
    vec3 specular;
};

uniform Material material = { vec3(1.0f), vec3(1.0f), vec3(1.0f), vec3(1.0f), 0.25f };

// UNIFORM SIZE COUNTERS
uniform int DIR_LIGHT_COUNT = 0;
uniform int POINT_LIGHT_COUNT = 0;
uniform int SPOT_LIGHT_COUNT = 0;
uniform int TEX_DIFF_COUNT = 0;
uniform int TEX_SPEC_COUNT = 0;

// UNIFORMS
// --------
uniform bool blinn;
uniform vec3 viewPos;
uniform DirLight dirLights[NR_DIR_LIGHTS];
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform SpotLight spotLights[NR_SPOT_LIGHTS];
uniform sampler2D texture_diffuse[NR_TEX_DIFF];
uniform sampler2D texture_specular[NR_TEX_SPEC];
uniform float specularExponent = 1024; // http://devernay.free.fr/cours/opengl/materials.html all material shininess must be boosted by 128.0f
// EXITING
// -------
out vec4 FragColor;

// FUNCTIONAL PROTOTYPES
// ---------------------
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, FragTextures fragTextures);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 viewDir, FragTextures fragTextures);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 viewDir, FragTextures fragTextures); 
vec3 GetDiffuseSample();
vec3 GetSpecularSample();
// MAIN CALCULATIONS
// -----------------
void main()
{
    
    // Properties
    vec3 norm = normalize(frag_in.Normal);
    // If back face then invert normal;
    if (!gl_FrontFacing) 
        norm = -norm;
    vec3 viewDir = normalize(viewPos - frag_in.FragmentPos);
    vec3 result = vec3(0.0f, 0.0f, 0.0f);

    FragTextures fragTextures;
    fragTextures.diffuse = GetDiffuseSample();
    fragTextures.specular = GetSpecularSample();

    // phase 1: Directional lighting
    for(int i =0; i< DIR_LIGHT_COUNT; i++)
        result +=CalcDirLight(dirLights[i], norm, viewDir, fragTextures);
    // phase 2: Point lights
    for(int i = 0; i < POINT_LIGHT_COUNT; i++)
        result += CalcPointLight(pointLights[i], norm, viewDir, fragTextures);    
    // phase 3: Spot light
    for(int i = 0; i < SPOT_LIGHT_COUNT; i++)
        result += CalcSpotLight(spotLights[i], norm, viewDir, fragTextures);    

    result += material.emissive;

    FragColor = vec4(result, 1.0);
}

vec3 GetDiffuseSample()
{
    vec3 tex = vec3 (0.0f, 0.0f, 0.0f);

    for (int i = 0; i < TEX_DIFF_COUNT; ++i)
    {
        tex += vec3(texture(texture_diffuse[i],frag_in.TexCoords));
    }

    return tex;
}

vec3 GetSpecularSample()
{
    vec3 tex = vec3 (0.0f);

    for (int i = 0; i < TEX_SPEC_COUNT; ++i)
    {
        tex += vec3(texture(texture_specular[i],frag_in.TexCoords));
    }

    return tex;
}

// calculates the color when using a directional light.
// ----------------------------------------------------
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir,  FragTextures fragTextures)
{
    // Get light direction
    vec3 lightDir = normalize(-light.direction);

    // Diffuse shading
    float diffuse = max(dot(normal, lightDir), 0.0);

    // Specular shading (blinn-phong)
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float specular = pow(max(dot(normal, halfwayDir), 0.0), specularExponent * material.shininess);
    
     // Combine results
    vec3 ambientLight  = light.ambient * material.ambient;
    vec3 diffuseLight  = light.diffuse * diffuse * fragTextures.diffuse * material.diffuse ;
    vec3 specularLight = light.specular * specular * fragTextures.specular * material.specular;

    return (ambientLight + diffuseLight + specularLight);
}

// calculates the color when using a point light.
// ----------------------------------------------
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 viewDir,  FragTextures fragTextures)
{
    vec3 FragToLightVector = light.position - frag_in.FragmentPos;

    // Get light direction
    vec3 lightDir = normalize(FragToLightVector);

     // Difuse shading
    float diffuse = max(dot(normal, lightDir), 0.0);

    // Specular shading
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float specular = pow(max(dot(normal, halfwayDir), 0.0), specularExponent * material.shininess);
  
    // Calculate Attenuation
    float distance    = length(FragToLightVector);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance)); 

    // Combine results
    vec3 ambientLight  = light.ambient  * material.ambient;
    vec3 diffuseLight  = light.diffuse  * diffuse * fragTextures.diffuse * material.diffuse;
    vec3 specularLight = light.specular * specular * fragTextures.specular * material.specular;

    return (ambientLight + diffuseLight + specularLight) * attenuation;
}

// calculates the color when using a spot light.
// ---------------------------------------------
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 viewDir,  FragTextures fragTextures)
{
    vec3 FragToLightVector = light.position - frag_in.FragmentPos;
    // Get light direction
    vec3 lightDir = normalize(FragToLightVector);
    // Diffuse shading
    float diffuse = max(dot(normal, lightDir), 0.0);

    // Specular shading
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float specular = pow(max(dot(normal, halfwayDir), 0.0), specularExponent * material.shininess);
    
    
    // Attenuation
    float distance = length(FragToLightVector);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    

    // spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    // Combine results
    vec3 ambientLight  = light.ambient  * material.ambient;
    vec3 diffuseLight  = light.diffuse  * diffuse * fragTextures.diffuse * material.diffuse;
    vec3 specularLight = light.specular * specular * fragTextures.specular * material.specular;

    return (ambientLight + diffuseLight + specularLight) * attenuation * intensity;
}