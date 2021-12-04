#version 450 core
in VERT_OUT
{
    vec3 FragmentPos;
    vec3 Normal;
    vec2 TexCoords;
    vec3 Tangent;
    vec3 Bitangent;
} frag_in;


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

uniform int DIR_LIGHT_COUNT;
uniform int POINT_LIGHT_COUNT;
uniform int SPOT_LIGHT_COUNT;

uniform DirLight dirLights[NR_DIR_LIGHTS];
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform SpotLight spotLights[NR_SPOT_LIGHTS];


#define NR_TEX_DIFF 10
#define NR_TEX_SPEC 10

uniform int TEX_DIFF_COUNT;
uniform int TEX_SPEC_COUNT;

uniform sampler2D texture_diffuse[NR_TEX_DIFF];
uniform sampler2D texture_specular[NR_TEX_SPEC];

uniform bool blinn;
uniform vec3 viewPos;
uniform float texture_shininess = 32.0f;

out vec4 FragColor;

// FUNCTIONAL PROTOTYPES
// ---------------------
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir); 

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
    // phase 1: Directional lighting
    for(int i =0; i< DIR_LIGHT_COUNT; i++)
        result +=CalcDirLight(dirLights[i], norm, viewDir);
    // phase 2: Point lights
    for(int i = 0; i < POINT_LIGHT_COUNT; i++)
        result += CalcPointLight(pointLights[i], norm, frag_in.FragmentPos, viewDir);    
    // phase 3: Spot light
    for(int i = 0; i < SPOT_LIGHT_COUNT; i++)
        result += CalcSpotLight(spotLights[i], norm, frag_in.FragmentPos, viewDir);    
    //FragColor = texture(texture_diffuse1, TexCoords);
    FragColor = vec4(result, 1.0);
}

// calculates the color when using a directional light.
// ----------------------------------------------------
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 ambientLight = vec3(0.0f, 0.0f, 0.0f);
    vec3 diffuseLight = vec3(0.0f, 0.0f, 0.0f);
    vec3 specularLight = vec3(0.0f, 0.0f, 0.0f);
    // Get light direction
    vec3 lightDir = normalize(-light.direction);
    // Diffuse shading
    float diffuse = max(dot(normal, lightDir), 0.0);

    // Specular shading
    float specular;
    if (blinn)  // Use blinn-phong (angle between halfway vector and normal)
    {
        vec3 halfwayDir = normalize(lightDir + viewDir);
        specular = pow(max(dot(normal, halfwayDir), 0.0), texture_shininess);
    }
    else        // Use phong (angle between reflection vector and view vector)
    {
        vec3 reflectDir = reflect(-lightDir, normal);
        specular = pow(max(dot(viewDir, reflectDir), 0.0), texture_shininess);
    }

    // Combine result
    for (int i = 0; i < TEX_DIFF_COUNT; i++)
    {
        ambientLight  += light.ambient * vec3(texture(texture_diffuse[i],frag_in.TexCoords));
        diffuseLight  += light.diffuse * diffuse * vec3(texture(texture_diffuse[i], frag_in.TexCoords));
    }
    for (int i = 0; i < TEX_SPEC_COUNT; i++)
        specularLight += light.specular * specular * vec3(texture(texture_specular[i], frag_in.TexCoords));
    return (ambientLight + diffuseLight + specularLight);
}

// calculates the color when using a point light.
// ----------------------------------------------
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    
    vec3 ambientLight = vec3(0.0f, 0.0f, 0.0f);
    vec3 diffuseLight = vec3(0.0f, 0.0f, 0.0f);
    vec3 specularLight = vec3(0.0f, 0.0f, 0.0f);
    // Get light direction
    vec3 lightDir = normalize(light.position - fragPos);

     // Difuse shading
    float diffuse = max(dot(normal, lightDir), 0.0);

    vec3 halfwayDir = normalize(lightDir + viewDir);
    float specular = pow(max(dot(normal, halfwayDir), 0.0), texture_shininess);
   
    // Calculate Attenuation
    float distance    = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance)); 
    // Combine results
    for (int i = 0; i < TEX_DIFF_COUNT; i++)
    {
        ambientLight  += light.ambient  * vec3(texture(texture_diffuse[i], frag_in.TexCoords));
        diffuseLight  += light.diffuse  * diffuse * vec3(texture(texture_diffuse[i], frag_in.TexCoords));
    }
    for (int i = 0; i < TEX_SPEC_COUNT; i++)
        specularLight += light.specular * specular * vec3(texture(texture_specular[i], frag_in.TexCoords));
    ambientLight  *= attenuation;
    diffuseLight  *= attenuation;
    specularLight *= attenuation;   
    return (ambientLight + diffuseLight + specularLight);
}

// calculates the color when using a spot light.
// ---------------------------------------------
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 ambientLight = vec3(0.0f, 0.0f, 0.0f);
    vec3 diffuseLight = vec3(0.0f, 0.0f, 0.0f);
    vec3 specularLight = vec3(0.0f, 0.0f, 0.0f);
    // Get light direction
    vec3 lightDir = normalize(light.position - fragPos);
    // Diffuse shading
    float diffuse = max(dot(normal, lightDir), 0.0);

    // Specular shading
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float specular = pow(max(dot(normal, halfwayDir), 0.0), texture_shininess);
    
    // Attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    // Combine results
    for (int i = 0; i < TEX_DIFF_COUNT; i++)
    {
        ambientLight += light.ambient * vec3(texture(texture_diffuse[i], frag_in.TexCoords));
        diffuseLight += light.diffuse * diffuse * vec3(texture(texture_diffuse[i], frag_in.TexCoords));
    }
    for (int i = 0; i < TEX_SPEC_COUNT; i++)
        specularLight += light.specular * specular * vec3(texture(texture_specular[i], frag_in.TexCoords));
    ambientLight  *= attenuation * intensity;
    diffuseLight  *= attenuation * intensity;
    specularLight *= attenuation * intensity;
    return (ambientLight + diffuseLight + specularLight);
}

// void main()
// {
//     FragColor = texture(texture_diffuse[0], frag_in.TexCoords);
// }