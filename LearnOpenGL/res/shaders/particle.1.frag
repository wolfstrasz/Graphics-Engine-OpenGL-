#version 450 core


out vec4 FragColor;

uniform vec3 colour;
uniform int pointsCount;
flat in int vertID; // flat because data from vert to frag has a 2:1 mapping
float flashLightCutOffRadius = 190.0f;
float screenHeight = 600.0f;
float screenWidth = 800.0f;
void main()
{    
    float deAlpha = float(vertID) / float(pointsCount);

    vec3 myColour = colour;
    
    float xSquared = (gl_FragCoord.x - screenWidth/2) * (gl_FragCoord.x - screenWidth/2);
    float ySquared = (gl_FragCoord.y - screenHeight/2) * (gl_FragCoord.y - screenHeight/2);
    float radius = sqrt( xSquared + ySquared);
    if (radius < flashLightCutOffRadius)
        myColour = vec3 (1.0f, 0.0f, 0.0f);
    FragColor = vec4(myColour, 1.0f * (1.0f - deAlpha));

}