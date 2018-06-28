#version 450 core
// Entering
in vec3 Normal;
in vec3 FragmentPos;
// Global
uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
// Exit
out vec4 FragColor;

void main()
{
    // Calculate Ambient Light
    float ambientStrenght = 0.1f;
    vec3 ambientLight = ambientStrenght * lightColor;
    // Calculate Diffuse Light
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragmentPos);
    float diffuseImpact = max(dot(norm,lightDir), 0.0);
    vec3 diffuseLight = diffuseImpact * lightColor;

    // Calculate result
    vec3 result = (ambientLight + diffuseLight) * objectColor;
    FragColor = vec4(result, 1.0);
}