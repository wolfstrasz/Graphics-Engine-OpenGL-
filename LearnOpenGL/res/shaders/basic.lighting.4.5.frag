#version 450 core
// Entering
in vec3 Normal;
in vec3 FragmentPos;
// Global
uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;
// Exit
out vec4 FragColor;

void main()
{
    // Calculate Ambient Light
    float ambientStrenght = 0.1f;                                                   // (0)
    vec3 ambientLight = ambientStrenght * lightColor;
    // Calculate Diffuse Light
    vec3 norm = normalize(Normal);                                                  // (1)
    vec3 lightDir = normalize(lightPos - FragmentPos);
    float diffuseStrenght = max(dot(norm,lightDir), 0.0);
    vec3 diffuseLight = diffuseStrenght * lightColor;
    // Calculate Specular Light
    float specularStrength = 0.5;                                                   // (2)
    vec3 viewDir = normalize(viewPos - FragmentPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    int objectShininess = 256;
    float specularIntensity = pow(max(dot(viewDir, reflectDir), 0.0), objectShininess);
    vec3 specularLight = specularStrength * specularIntensity * lightColor;  
    // Calculate result
    vec3 result = (ambientLight + diffuseLight + specularLight) * objectColor;      // (3)  
    FragColor = vec4(result, 1.0);
}

/*----------------------------------------------------------------------------
	(0) :
	Ambient Light is the light that is always there. Reflected from one
     OBJECT to another and then another and so on. Examples: Moon light,
      shadows of buildings are not completely dark.
//----------------------------------------------------------------------------
	(1) :
    Diffuse Light is the directional light from the light source. Its 
     STRENGTH is dependent on ANGLE of IMPACT. Which is calculated by the
      dot product of the DIRECTION VECTOR (norm) of the light source to the
       OBJECT and the NORMAL vector of the OBJECT'S SURFACE.
//----------------------------------------------------------------------------
	(2) :
    Specular Light is the light reflected from the OBJECT. It is dependent
     on the SHININESS of the OBJECT. The STRENGHT is dependent on the OBJECT'S
      REFLECTIVENESS. The INTENSITITY is dependent on the ANGLE between the 
       VIEWER and the "Inverse of the reflection of the light's direction 
        vector reflected via the Object's normal vector axis"
//----------------------------------------------------------------------------
	(3) :
	The COLOR of an OBJECT is dependent of the PHONG lighting IMPACT size
     multiplied by the ORIGINAL COLOR. PHONG = AMBIENT + DIFFUSE + SPECULAR
      TRUE COLOR = (PHONG) * (ORIGINAL COLOR) 
*/