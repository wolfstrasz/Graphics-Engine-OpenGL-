#version 450 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D sampleBuffer;
uniform float exposure = 1.0f;
uniform float gamma = 2.2f;

void main()
{             
    vec3 hdrColor = texture(sampleBuffer, TexCoords).rgb;
  
    // exposure
    vec3 result = vec3(1.0) - exp(-hdrColor * exposure);
    // also gamma correct while we're at it       
    result = pow(result, vec3(1.0 / gamma));
    FragColor = vec4(result, 1.0);
   
}