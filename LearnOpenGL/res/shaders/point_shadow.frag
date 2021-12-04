#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} fs_in;

// general uniforms
uniform int LIGHT_COUNT;
uniform sampler2D texture_diffuse[1];
uniform samplerCube depthMap[2];
uniform vec3 lightPos[2];
uniform vec3 viewPos;
uniform float far_plane;

// control uniforms
uniform bool shadows;
uniform bool usePCF;
uniform bool showDepthMap;
uniform bool lamp;


uniform int index = 0;


// sampling vectors
vec3 sampleOffsetDirections[] = vec3[]
(
   vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1), 
   vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
   vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
   vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
   vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
);   

float ShadowCalculation(vec3 fragPos, int lightID)
{
    // vectors for light calculation
    vec3 normal = normalize(fs_in.Normal);
    // Get light direction
    vec3 lightDir = normalize(lightPos[lightID] - fs_in.FragPos);
     // get vector between fragment position and light position
    vec3 fragToLight = fragPos - lightPos[lightID];

    // calculate bias (steeper angles get higher bias)
    float maxBias = 0.75f;
    float minBias = 0.0075f;
    float bias = max(maxBias * (1.0f - dot(normal, lightDir)), minBias);


    // values to calculate
    float shadow = 0.0f;
    float closestDepth = 0.0f;
 
    // get current linear depth as the length between the fragment and light position
    float currentDepth = length(fragToLight);


    // shadow calculation
    if (!usePCF) // use hard shadows
    {
        // sample from the depth map
        closestDepth = texture(depthMap[lightID], fragToLight).r;
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
            closestDepth = texture(depthMap[lightID], fragToLight + sampleOffsetDirections[i] * sampleRegionRadius).r;
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
    {
            FragColor += vec4(vec3((closestDepth / far_plane)), 1.0);   
    }
      
    
    return shadow; // returns a value between 0.0 - 1.0f
}
vec3 CalculateLight (int lightID)
{
    vec3 lightColor = vec3(1.0f);
    vec3 normal = normalize(fs_in.Normal);
    vec3 lightDir = normalize(lightPos[lightID] - fs_in.FragPos);
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

    vec3 lighting = specular + diffuse;
    return lighting;
}


void main()
{   
    vec3 color = texture(texture_diffuse[0], fs_in.TexCoords).rgb;
    FragColor = vec4(0.0f);

    // ambient light
    vec3 ambient = 0.3f * color;

    // diffuse + specular light
    vec3 lightLight = vec3(0.0f);
    
    for (int i = 0; i < LIGHT_COUNT; ++ i)
    {
        // calculate shadow
        float shadow = shadows ? ShadowCalculation(fs_in.FragPos, i) : 0.0f;  
        lightLight +=  ((ambient + (1.0f - shadow) * CalculateLight(i)) * color );

    }

    lightLight = lightLight / LIGHT_COUNT;

    if (!showDepthMap)
        FragColor = vec4(lightLight, 1.0f);
    // if object is a lamp then draw in white colour
    if(lamp)
        FragColor = vec4(1.0f);
}