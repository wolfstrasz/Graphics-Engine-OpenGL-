#version 450 core
// Entering
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
// Global
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
// Exit
out vec3 Normal;
out vec3 FragmentPos;
out vec2 TexCoords;
void main()
{
	// Get clip position of vector
	gl_Position = projection * view * model * vec4(aPos, 1.0);
	// Get fragment position										// (0)
	FragmentPos = vec3(model * vec4(aPos,1.0));
	
	// Create Normal matrix from Model matrix						// (1)
	mat3 normalMatrix = mat3(transpose(inverse(model))); 			// (2)
	// Get the fragment normal in world space:
	Normal = normalMatrix * aNormal;
	// Forward Texture Coordinates
	TexCoords = aTexCoords;
}
/*----------------------------------------------------------------------------
	(0) :
	In order to calculate the direction vector of the light to
	 the object we need the object's position vector in WORLD SPACE
	  Done by multiplying the vector by the model matrix of the world
//----------------------------------------------------------------------------
	(1) :
	Translation and non-Uniform scaling matrices can severely affect the
	 NORMAL vector of a model when doing TRANSFORMATION from LOCAL to WORLD
	  space, thus we need a NORMAL MATRIX that is the "transpose of the inverse
	   of the upper-left corner of the model matrix"
	    More info: http://www.lighthouse3d.com "normal matrix"
//----------------------------------------------------------------------------
	(2) :
	INVERSE(ofMatrix) - is a heavy operation so it is better to
						 calculate the normalMatrix in the CPU (main
						  thread) and then send it to the shader via UNIFORM
*/