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

float vertices[] = {
	// positions			//colors			//texture coordinate		// point preview
	0.5f,  0.5f, 0.0f,		1.0f, 0.0f, 0.0f,	1.0f, 1.0f,					// [0] = top right, red , top right 
	0.5f, -0.5f, 0.0f,		0.0f, 1.0f, 0.0f,	1.0f, 0.0f,					// [1] = bottom right, green, bottom right
	-0.5f, -0.5f, 0.0f,		0.0f, 0.0f, 1.0f,	0.0f, 0.0f,					// [2] = bottom left, blue, bottom left
	-0.5f,  0.5f, 0.0f,		0.0f, 1.0f, 0.0f,	0.0f, 1.0f,					// [3] = top left , green, top left
	0.0f, 0.5f, 0.0f,		1.0f, 1.0f, 0.0f,	0.5f, 1.0f					// [4] = top mid, yellow, top mid
};
//unsigned int indices[] = { 
//	0, 1, 3,  // first triangle
//	1, 2, 3   // second triangle
//};
unsigned int indices[] = {
	1,2,4,
	2,3,0
};

#endif // !_CONTAINERS_H
