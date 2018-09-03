#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} fs_in;

uniform sampler2D diffuseTexture;
uniform samplerCube depthMap;

uniform vec3 lightPos;
uniform vec3 viewPos;

uniform float far_plane;
uniform bool shadows;

uniform bool pcf;
uniform bool showDepthMap;
uniform float biasScale;
vec3 sampleOffsetDirections[20] = vec3[]
(
   vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1), 
   vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
   vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
   vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
   vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
);   

float ShadowCalculation(vec3 fragPos, float bias)
{
    float shadow;
    float closestDepth;
     // get vector between fragment position and light position
    vec3 fragToLight = fragPos - lightPos;
    // now get current linear depth as the length between the fragment and light position
    float currentDepth = length(fragToLight);

    if (!pcf)
    {
    // ise the fragment to light vector to sample from the depth map    
    closestDepth = texture(depthMap, fragToLight).r;
    // it is currently in linear range between [0,1], let's re-transform it back to original depth value
    closestDepth *= far_plane;  
    // test for shadows
    shadow = currentDepth -  bias > closestDepth ? 1.0 : 0.0;    
    }
    if (pcf)
    {
        shadow  = 0.0;
        float samples = 20.0;
        float viewDistance = length(viewPos - fragPos);
        float diskRadius = 0.025;
        float offset  = 0.1;
        for(int i = 0; i < samples; ++i)
        {
            closestDepth = texture(depthMap, fragToLight + sampleOffsetDirections[i] * diskRadius).r;
            closestDepth *= far_plane;   // Undo mapping [0;1]
            if(currentDepth - bias > closestDepth)
                shadow += 1.0;
        }
        //shadow /= (samples * samples * samples);
        shadow /= float(samples); 
    }

    // display closestDepth as debug (to visualize depth cubemap)
    if (showDepthMap)
    FragColor = vec4(vec3(closestDepth / far_plane), 1.0);   

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
    float maxVal = 0.75;
    float minVal = 0.0075;
    float bias = max(maxVal * (1.0 - dot(normal, lightDir)), minVal);
    bias += biasScale;
    // calculate shadow
    float shadow = shadows ? ShadowCalculation(fs_in.FragPos, bias) : 0.0;                      
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;    
    
    if (!showDepthMap)
    FragColor = vec4(lighting, 1.0);
    
}