#version 450 core

/*
--------------------------------------
 Shadow Mapping for directional lights
 -------------------------------------
 */
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
} fs_in;

uniform sampler2D diffuseTexture;
uniform sampler2D shadowMap;

uniform vec3 lightPos;
uniform vec3 viewPos;

float ShadowCalculation(vec4 fragPosLightSpace, float bias)
{
    float shadow;
    // perform perspective divide (not needed in orthographic projection but needed for point lights)
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // check whether current frag pos is in shadow
    if(projCoords.z > 1.0) shadow = 0.0;
    else
    {
        // Simple shadow edges
        //shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;

        // Softer shadow edges wiht Percentage-closer Filtering (PCF)
        vec2 texelSize = 0.5 / textureSize(shadowMap, 0);       // changing the size of the texel effects quality of shadows
        for(int x = -2; x <= 2; x++)                            // changing the samples made effects the quality of shadows
            for(int y = -2; y <= 2; y++)
            {
                float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
                shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
            }    
        shadow /= 25.0;
    } 
    

    return shadow;
}

void main()
{           
    vec3 color = texture(diffuseTexture, fs_in.TexCoords).rgb;
    vec3 normal = normalize(fs_in.Normal);
    vec3 lightColor = vec3(0.3);
    // ambient
    vec3 ambient = 0.3 * color;
    // diffuse
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor;
    // specular
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    vec3 specular = spec * lightColor;    
    
    // calculate bias (steeper angles get higher bias)
    float maxVal = 0.05;
    float bias = max(maxVal * (1.0 - dot(normal, lightDir)), 0.005);
    // calculate shadow
    float shadow = ShadowCalculation(fs_in.FragPosLightSpace, bias);                      
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;    
    
    FragColor = vec4(lighting, 1.0);
}