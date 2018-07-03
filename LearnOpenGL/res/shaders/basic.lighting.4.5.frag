#version 450 core
// Material Structure
struct Material {
    sampler2D diffuse;          // Diffuse map
    sampler2D specular;         // Specular map
    float shininess;
};

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

// Entering
in vec3 Normal;
in vec3 FragmentPos;
in vec2 TexCoords;
// Global
uniform Material material;
uniform Light light;
uniform vec3 lightColor;
uniform vec3 viewPos;
// Exit
out vec4 FragColor;

void main()
{   
    // In case the Normal is not normal :)
    vec3 norm = normalize(Normal);

    // Calculate Ambient Light                                                          // (0)
    //------------------------
    vec3 textureColor = vec3(texture(material.diffuse, TexCoords));
    vec3 ambientLight = light.ambient * textureColor;

    // Calculate Diffuse Light                                                          // (1)
    //------------------------
    vec3 lightDir = normalize(light.position - FragmentPos);
    // Get cos(angle of impact of the light to the vertex)
    float diffuseStrength = max(dot(norm,lightDir), 0.0);
    vec3 diffuseLight = light.diffuse * (diffuseStrength * textureColor);

    // Calculate Specular Light                                                         // (2)
    //-------------------------
    vec3 viewDir = normalize(viewPos - FragmentPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float specularStrength = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specularLight = light.specular * (specularStrength * vec3(texture(material.specular, TexCoords)));

    // Calculate result                                                                 // (3)
    //-----------------
    FragColor = vec4(ambientLight + diffuseLight + specularLight, 1.0);
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