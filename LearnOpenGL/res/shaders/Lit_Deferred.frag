#version 450 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;

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


#define NR_POINT_LIGHTS 32
#define NR_DIR_LIGHTS 32
#define NR_SPOT_LIGHTS 32

uniform int DIR_LIGHT_COUNT = 0;
uniform int POINT_LIGHT_COUNT = 0;
uniform int SPOT_LIGHT_COUNT = 0;

uniform vec3 viewPos;
uniform DirLight dirLights[NR_DIR_LIGHTS];
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform SpotLight spotLights[NR_SPOT_LIGHTS];

uniform float texture_shininess = 16.0f;
// FUNCTIONAL PROTOTYPES
// ---------------------
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec4 albedoSpec);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec4 albedoSpec);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec4 albedoSpec); 




void main()
{
    // retrieve data from gbuffer
    vec3 FragPos = texture(gPosition, TexCoords).rgb;
    vec3 Normal = texture(gNormal, TexCoords).rgb;
    vec4 AlbedoSpec = texture(gAlbedoSpec, TexCoords);

    vec3 viewDir = normalize(viewPos - FragPos);

    vec3 result = vec3(0.0f, 0.0f, 0.0f);
    // phase 1: Directional lighting
    for(int i =0; i< DIR_LIGHT_COUNT; i++)
        result +=CalcDirLight(dirLights[i], Normal, viewDir, AlbedoSpec);
    // phase 2: Point lights
    for(int i = 0; i < POINT_LIGHT_COUNT; i++)
        result += CalcPointLight(pointLights[i], Normal, FragPos, viewDir, AlbedoSpec);    
    // phase 3: Spot light
    for(int i = 0; i < SPOT_LIGHT_COUNT; i++)
        result += CalcSpotLight(spotLights[i], Normal, FragPos, viewDir, AlbedoSpec);    

    FragColor = vec4(result, 1.0f);
    // FragColor = vec4(AlbedoSpec.rgb, 1.0);
}


    // calculates the color when using a directional light.
// ----------------------------------------------------
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec4 albedoSpec)   
{
    // Get light direction
    vec3 lightDir = normalize(-light.direction);

    // Diffuse shading
    vec3 diffuse = max(dot(normal, lightDir), 0.0) * albedoSpec.rgb * light.diffuse;
    // Ambient shading
    vec3 ambient = light.ambient * albedoSpec.rgb;
    // Specular shading (Blinn-Phong)
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), texture_shininess);

    vec3 specular = light.specular * spec * albedoSpec.a; // albedoSpec.a holds the R value of the specular map;

     return (diffuse + ambient + specular);
}

// calculates the color when using a point light.
// ----------------------------------------------
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec4 albedoSpec)
{

    // Get light direction
    vec3 lightDir = normalize(light.position - fragPos);

     // Difuse shading
    vec3 diffuse = vec3(0.0f, 0.0f, 0.0f);
    diffuse = max(dot(normal, lightDir), 0.0) * albedoSpec.rgb * light.diffuse;

    // Ambient shading
    vec3 ambient = vec3(0.0f, 0.0f, 0.0f);
    //ambient = light.ambient * albedoSpec.rgb;

    // Specular shading
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), texture_shininess);
    vec3 specular = light.specular * spec * albedoSpec.a; // albedoSpec.a holds the R value of the specular map;

    // Calculate Attenuation
    float distance    = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance)); 

    // Combine results
    diffuse  *= attenuation;
    ambient  *= attenuation;
    specular *= attenuation;   
    return (diffuse + ambient + specular);
}

// calculates the color when using a spot light.
// ---------------------------------------------
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec4 albedoSpec)
{
    // Get light direction
    vec3 lightDir = normalize(light.position - fragPos);

    // Diffuse shading
    vec3 diffuse = max(dot(normal, lightDir), 0.0) * albedoSpec.rgb * light.diffuse;

    // Ambient shading
    vec3 ambient = light.ambient * albedoSpec.rgb;

    // Specular shading
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), texture_shininess);
    vec3 specular = light.specular * spec * albedoSpec.a; // albedoSpec.a holds the R value of the specular map;
    
    // Attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    

    // spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
  
    ambient  *= attenuation * intensity;
    diffuse  *= attenuation * intensity;
    specular *= attenuation * intensity;
    return (ambient + diffuse + specular);
}
