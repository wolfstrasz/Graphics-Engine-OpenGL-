#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} fs_in;

// general uniforms
uniform sampler2D diffuseTexture;
uniform samplerCube depthMap;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform float far_plane;
// control uniforms
uniform bool shadows;
uniform bool usePCF;
uniform bool showDepthMap;
uniform bool lamp;
// sampling vectors
vec3 sampleOffsetDirections[] = vec3[]
(
   vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1), 
   vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
   vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
   vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
   vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
);   

float ShadowCalculation(vec3 fragPos, float bias)
{
    float shadow = 0.0f;
    float closestDepth = 0.0f;
    // get vector between fragment position and light position
    vec3 fragToLight = fragPos - lightPos;
    // get current linear depth as the length between the fragment and light position
    float currentDepth = length(fragToLight);
    // shadow calculation
    if (!usePCF) // use hard shadows
    {
        // sample from the depth map
        closestDepth = texture(depthMap, fragToLight).r;
        // current range between [0,1], transform it back to original depth value
        closestDepth *= far_plane;  
        // test for shadows
        shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;    
    }
    if (usePCF) // use soft shadows
    {
        float samples = 20.0;
        float sampleRegionRadius = 0.025;
        for(int i = 0; i < samples; ++i)
        {
            // sample from the depth map
            closestDepth = texture(depthMap, fragToLight + sampleOffsetDirections[i] * sampleRegionRadius).r;
            // Undo mapping [0,1]
            closestDepth *= far_plane;
            // sum of shadow tests of all samples
            shadow += currentDepth - bias > closestDepth ? 1.0 : 0.0;
        }
        // get mean of samples
        shadow /= float(samples); 
    }
    // display closestDepth as debug (to visualize depth cubemap)
    if (showDepthMap)
        FragColor = vec4(vec3(closestDepth / far_plane), 1.0);   
    
    return shadow;
}

void main()
{   
    vec3 lightColor = vec3(1.0f);
    vec3 color = texture(diffuseTexture, fs_in.TexCoords).rgb;
    // vectors for light calculation
    vec3 normal = normalize(fs_in.Normal);
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    // ambient light
    vec3 ambient = 0.3f * color;
    // diffuse light
    float diff = max(dot(lightDir, normal), 0.0f);
    vec3 diffuse = diff * lightColor;
    // specular light
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = 0.0f;
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    spec = pow(max(dot(normal, halfwayDir), 0.0f), 64.0f);
    vec3 specular = spec * lightColor;

    // calculate bias (steeper angles get higher bias)
    float maxBias = 0.75f;
    float minBias = 0.0075f;
    float bias = max(maxBias * (1.0f - dot(normal, lightDir)), minBias);
    // calculate shadow
    float shadow = shadows ? ShadowCalculation(fs_in.FragPos, bias) : 0.0f;  
    // calculate light                    
    vec3 lighting = (ambient + (1.0f - shadow) * (diffuse + specular)) * color;    
    
    if (!showDepthMap)
        FragColor = vec4(lighting, 1.0f);
    // if object is a lamp then draw in white colour
    if(lamp)
        FragColor = vec4(1.0f);
}