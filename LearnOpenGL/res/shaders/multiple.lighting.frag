#version 450 core
// STRUCTURE DEFINITIONS
// ---------------------
struct Material {
    sampler2D diffuse;          // Diffuse map
    sampler2D specular;         // Specular map
    sampler2D emission;         // Emission map
    float shininess;
};

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

#define NR_POINT_LIGHTS 4

// ENTERING
// --------
in vec3 FragmentPos;
in vec3 Normal;
in vec2 TexCoords;

// UNIFORMS
// --------
uniform vec3 viewPos;
uniform DirLight dirLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform SpotLight spotLight;
uniform Material material;

// EXITING
// -------
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
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragmentPos);

    // phase 1: Directional lighting
    vec3 result = CalcDirLight(dirLight, norm, viewDir);
    // phase 2: Point lights
    for(int i = 0; i < NR_POINT_LIGHTS; i++)
        result += CalcPointLight(pointLights[i], norm, FragmentPos, viewDir);    
    // phase 3: Spot light
    result += CalcSpotLight(spotLight, norm, FragmentPos, viewDir);    
    
    FragColor = vec4(result, 1.0);
}

// calculates the color when using a directional light.
// ----------------------------------------------------
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDirection)
{
    vec3 lightDirection = normalize(-light.direction);
    // Diffuse shading
    float diffuse = max(dot(normal, lightDirection), 0.0);
    // Specular shading
    vec3 reflectDirection = reflect (-lightDirection, normal);
    float specular = pow(max(dot(viewDirection,reflectDirection), 0.0), material.shininess);
    // Combine result
    vec3 ambientLight  = light.ambient * vec3(texture(material.diffuse,TexCoords));
    vec3 diffuseLight  = light.diffuse * diffuse * vec3(texture(material.diffuse, TexCoords));
    vec3 specularLight = light.specular * specular * vec3(texture(material.specular, TexCoords));
    return (ambientLight + diffuseLight + specularLight);
}

// calculates the color when using a point light.
// ----------------------------------------------
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDirection)
{
    vec3 lightDirection = normalize(light.position - fragPos);
    // Difuse shading
    float diffuse = max(dot(normal, lightDirection), 0.0);
    // Specular Shading
    vec3 reflectDirection = reflect(-lightDirection, normal);
    float specular = pow(max(dot(viewDirection,reflectDirection), 0.0), material.shininess);
    // Calculate Attenuation
    float distance    = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance)); 
    // Combine results
    vec3 ambientLight  = light.ambient  * vec3(texture(material.diffuse, TexCoords));
    vec3 diffuseLight  = light.diffuse  * diffuse * vec3(texture(material.diffuse, TexCoords));
    vec3 specularLight = light.specular * specular * vec3(texture(material.specular, TexCoords));
    ambientLight  *= attenuation;
    diffuseLight  *= attenuation;
    specularLight *= attenuation;   
    return (ambientLight + diffuseLight + specularLight);
}

// calculates the color when using a spot light.
// ---------------------------------------------
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // Diffuse shading
    float diffuse = max(dot(normal, lightDir), 0.0);
    // Specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float specular = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // Attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    // Combine results
    vec3 ambientLight = light.ambient * vec3(texture(material.diffuse, TexCoords));
    vec3 diffuseLight = light.diffuse * diffuse * vec3(texture(material.diffuse, TexCoords));
    vec3 specularLight = light.specular * specular * vec3(texture(material.specular, TexCoords));
    ambientLight  *= attenuation * intensity;
    diffuseLight  *= attenuation * intensity;
    specularLight *= attenuation * intensity;
    return (ambientLight + diffuseLight + specularLight);
}




