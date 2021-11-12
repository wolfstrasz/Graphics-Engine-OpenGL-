#version 330 core
/*
    Renders to FragColor as the normal lighted color
    AND
    Renders to BrightColor as the brightness (>1.0f) filter
*/
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

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
    //vec3 ambient = 0.0 * color;
    // lighting
    vec3 lighting = vec3(0.0);
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    for(int i = 0; i < POINT_LIGHT_COUNT; i++)
    {
        // diffuse
        vec3 lightDir = normalize(pointLights[i].position - fs_in.FragPos);  
        float diff = max(dot(lightDir, normal), 0.0);
        // vec3 result = pointLights[i].diffuse * diff * color ;
        vec3 result = pointLights[i].diffuse * diff * color + pointLights[i].ambient * color;      

        // attenuation (use quadratic as we have gamma correction)
        float distance = length(fs_in.FragPos - pointLights[i].position);
        result *= 1.0 / (distance * distance);
        lighting += result;
        
    }
    vec3 result = lighting;
   // vec3 result = ambient + lighting;
    FragColor = vec4(result, 1.0);

    // check whether result is higher than some threshold, if so, output as bloom threshold color
    float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > 1.0)
        BrightColor = vec4(FragColor.rgb, 1.0);
    else
        BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
}