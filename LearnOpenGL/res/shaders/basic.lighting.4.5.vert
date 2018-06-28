#version 450 core
// Entering
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
// Global
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
// Exit
out vec3 Normal;
out vec3 FragmentPos;

void main()
{
	gl_Position = projection * view * model * vec4(aPos, 1.0);
	// Get fragment position
	FragmentPos = vec3(model * vec4(aPos,1.0));
	
	// Create Normal matrix from Model matrix
	mat3 normalMatrix = mat3(transpose(inverse(model)));
	// Get the fragment normal in world space:
	Normal = normalMatrix * aNormal;
}