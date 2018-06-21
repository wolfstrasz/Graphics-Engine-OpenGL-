#pragma once
#ifndef _DATA_H
#define _DATA_H
/************************************************************************/
/*							CREATING TRIANGLES							*/
/************************************************************************/

// Define vertices:
unsigned int vertices_size = 4;
unsigned int triangle_count = 2;
unsigned int indices_size = triangle_count * 3;

//float vertices[] = {
//	// positions			//colors			//texture coordinate		// point preview
//	0.5f,  0.5f, 0.0f,		1.0f, 0.0f, 0.0f,	1.0f, 1.0f,					// [0] = top right, red , top right 
//	0.5f, -0.5f, 0.0f,		0.0f, 1.0f, 0.0f,	1.0f, 0.0f,					// [1] = bottom right, green, bottom right
//	-0.5f, -0.5f, 0.0f,		0.0f, 0.0f, 1.0f,	0.0f, 0.0f,					// [2] = bottom left, blue, bottom left
//	-0.5f,  0.5f, 0.0f,		0.0f, 1.0f, 0.0f,	0.0f, 1.0f,					// [3] = top left , green, top left
//	0.0f, 0.5f, 0.0f,		1.0f, 1.0f, 0.0f,	0.5f, 1.0f					// [4] = top mid, yellow, top mid
//};

//  COORDINATES				COLORS				TEXTURE POINTS
float vertices[] = {
	-0.5f, -0.5f, -0.5f,	0.0f, 0.0f, 0.0f,   0.0f, 0.0f,
	0.5f, -0.5f, -0.5f,		0.0f, 0.0f, 0.0f,   1.0f, 0.0f,
	0.5f,  0.5f, -0.5f,		0.0f, 0.0f, 0.0f,   1.0f, 1.0f,
	0.5f,  0.5f, -0.5f,		0.0f, 0.0f, 0.0f,   1.0f, 1.0f,
	-0.5f,  0.5f, -0.5f,	0.0f, 0.0f, 0.0f,   0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,	0.0f, 0.0f, 0.0f,   0.0f, 0.0f,

	-0.5f, -0.5f,  0.5f,	0.0f, 0.0f, 0.0f,   0.0f, 0.0f,
	0.5f, -0.5f,  0.5f,		0.0f, 0.0f, 0.0f,   1.0f, 0.0f,
	0.5f,  0.5f,  0.5f,		0.0f, 0.0f, 0.0f,   1.0f, 1.0f,
	0.5f,  0.5f,  0.5f,		0.0f, 0.0f, 0.0f,   1.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,	0.0f, 0.0f, 0.0f,   0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,	0.0f, 0.0f, 0.0f,   0.0f, 0.0f,

	-0.5f,  0.5f,  0.5f,	0.0f, 0.0f, 0.0f,   1.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,	0.0f, 0.0f, 0.0f,   1.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,	0.0f, 0.0f, 0.0f,   0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,	0.0f, 0.0f, 0.0f,   0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,	0.0f, 0.0f, 0.0f,   0.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,	0.0f, 0.0f, 0.0f,   1.0f, 0.0f,

	0.5f,  0.5f,  0.5f,		0.0f, 0.0f, 0.0f,   1.0f, 0.0f,
	0.5f,  0.5f, -0.5f,		0.0f, 0.0f, 0.0f,   1.0f, 1.0f,
	0.5f, -0.5f, -0.5f,		0.0f, 0.0f, 0.0f,   0.0f, 1.0f,
	0.5f, -0.5f, -0.5f,		0.0f, 0.0f, 0.0f,   0.0f, 1.0f,
	0.5f, -0.5f,  0.5f,		0.0f, 0.0f, 0.0f,   0.0f, 0.0f,
	0.5f,  0.5f,  0.5f,		0.0f, 0.0f, 0.0f,   1.0f, 0.0f,

	-0.5f, -0.5f, -0.5f,	0.0f, 0.0f, 0.0f,   0.0f, 1.0f,
	0.5f, -0.5f, -0.5f,		0.0f, 0.0f, 0.0f,   1.0f, 1.0f,
	0.5f, -0.5f,  0.5f,		0.0f, 0.0f, 0.0f,   1.0f, 0.0f,
	0.5f, -0.5f,  0.5f,		0.0f, 0.0f, 0.0f,   1.0f, 0.0f,
	-0.5f, -0.5f,  0.5f,	0.0f, 0.0f, 0.0f,   0.0f, 0.0f,
	-0.5f, -0.5f, -0.5f,	0.0f, 0.0f, 0.0f,   0.0f, 1.0f,

	-0.5f,  0.5f, -0.5f,	0.0f, 0.0f, 0.0f,   0.0f, 1.0f,
	0.5f,  0.5f, -0.5f,		0.0f, 0.0f, 0.0f,   1.0f, 1.0f,
	0.5f,  0.5f,  0.5f,		0.0f, 0.0f, 0.0f,   1.0f, 0.0f,
	0.5f,  0.5f,  0.5f,		0.0f, 0.0f, 0.0f,   1.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,	0.0f, 0.0f, 0.0f,   0.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,	0.0f, 0.0f, 0.0f,   0.0f, 1.0f
};
//unsigned int indices[] = { 
//	0, 1, 3,  // first triangle
//	1, 2, 3   // second triangle
//};

// Pepe triangle
//unsigned int indices[] = {
//	1,2,4,
//	2,3,0
//};

unsigned int indices[] = {
	0,1,2,
	3,4,5,

	6,7,8,
	9,10,11,

	12,13,14,
	15,16,17,

	18,19,20,
	21,22,23,

	24,25,26,
	27,28,29,

	30,31,32,
	33,34,35
};
glm::vec3 cubePositions[] = {
	glm::vec3(0.0f,  0.0f,  0.0f),
	glm::vec3(2.0f,  5.0f, -15.0f),
	glm::vec3(-1.5f, -2.2f, -2.5f),
	glm::vec3(-3.8f, -2.0f, -12.3f),
	glm::vec3(2.4f, -0.4f, -3.5f),
	glm::vec3(-1.7f,  3.0f, -7.5f),
	glm::vec3(1.3f, -2.0f, -2.5f),
	glm::vec3(1.5f,  2.0f, -2.5f),
	glm::vec3(1.5f,  0.2f, -1.5f),
	glm::vec3(-1.3f,  1.0f, -1.5f)
};

#endif // !_CONTAINERS_H
