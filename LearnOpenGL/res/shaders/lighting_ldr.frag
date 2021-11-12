#version 450 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} fs_in;


#define NR_POINT_LIGHTS 10
struct PointLight {    
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;  

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform int POINT_LIGHT_COUNT;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform sampler2D texture_diffuse[1];
uniform vec3 viewPos;

void main()
{           
    vec3 color = texture(texture_diffuse[0], fs_in.TexCoords).rgb;
    vec3 normal = normalize(fs_in.Normal);
    // ambient
    vec3 ambient = 0.0 * color;
    // lighting
    vec3 lighting = vec3(0.0);
    for(int i = 0; i < POINT_LIGHT_COUNT; i++)
    {
        // diffuse
        vec3 lightDir = normalize(pointLights[i].position - fs_in.FragPos);
        float diff = max(dot(lightDir, normal), 0.0);
        vec3 diffuse = pointLights[i].diffuse * diff * color;      
        vec3 result = diffuse;        
        // attenuation (use quadratic as we have gamma correction)
        float distance = length(fs_in.FragPos - pointLights[i].position);
        result *= 1.0 / (distance * distance);
        lighting += result;
                
    }
    FragColor = vec4(ambient + lighting, 1.0);
}