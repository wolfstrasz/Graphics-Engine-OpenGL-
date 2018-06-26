#pragma once
#ifndef _VERTICES_H
#define _VERTICES_H

// CUBE with no color nor textures
float vertices[] = {
	-0.5f, -0.5f, -0.5f,
	0.5f, -0.5f, -0.5f,
	0.5f,  0.5f, -0.5f,
	0.5f,  0.5f, -0.5f,
	-0.5f,  0.5f, -0.5f,
	-0.5f, -0.5f, -0.5f,

	-0.5f, -0.5f,  0.5f,
	0.5f, -0.5f,  0.5f,
	0.5f,  0.5f,  0.5f,
	0.5f,  0.5f,  0.5f,
	-0.5f,  0.5f,  0.5f,
	-0.5f, -0.5f,  0.5f,

	-0.5f,  0.5f,  0.5f,
	-0.5f,  0.5f, -0.5f,
	-0.5f, -0.5f, -0.5f,
	-0.5f, -0.5f, -0.5f,
	-0.5f, -0.5f,  0.5f,
	-0.5f,  0.5f,  0.5f,

	0.5f,  0.5f,  0.5f,
	0.5f,  0.5f, -0.5f,
	0.5f, -0.5f, -0.5f,
	0.5f, -0.5f, -0.5f,
	0.5f, -0.5f,  0.5f,
	0.5f,  0.5f,  0.5f,

	-0.5f, -0.5f, -0.5f,
	0.5f, -0.5f, -0.5f,
	0.5f, -0.5f,  0.5f,
	0.5f, -0.5f,  0.5f,
	-0.5f, -0.5f,  0.5f,
	-0.5f, -0.5f, -0.5f,

	-0.5f,  0.5f, -0.5f,
	0.5f,  0.5f, -0.5f,
	0.5f,  0.5f,  0.5f,
	0.5f,  0.5f,  0.5f,
	-0.5f,  0.5f,  0.5f,
	-0.5f,  0.5f, -0.5f,
};
// CUBES with Colors and Textures
//  COORDINATES				COLORS				TEXTURE POINTS
//float vertices[] = {
//	-0.5f, -0.5f, -0.5f,	0.0f, 0.0f, 0.0f,   0.0f, 0.0f,
//	0.5f, -0.5f, -0.5f,	0.0f, 0.0f, 0.0f,   1.0f, 0.0f,
//	0.5f,  0.5f, -0.5f,	0.0f, 0.0f, 0.0f,   1.0f, 1.0f,
//	0.5f,  0.5f, -0.5f,	0.0f, 0.0f, 0.0f,   1.0f, 1.0f,
//	-0.5f,  0.5f, -0.5f,	0.0f, 0.0f, 0.0f,   0.0f, 1.0f,
//	-0.5f, -0.5f, -0.5f,	0.0f, 0.0f, 0.0f,   0.0f, 0.0f,
//
//	-0.5f, -0.5f,  0.5f,	0.0f, 0.0f, 0.0f,   0.0f, 0.0f,
//	0.5f, -0.5f,  0.5f,	0.0f, 0.0f, 0.0f,   1.0f, 0.0f,
//	0.5f,  0.5f,  0.5f,	0.0f, 0.0f, 0.0f,   1.0f, 1.0f,
//	0.5f,  0.5f,  0.5f,	0.0f, 0.0f, 0.0f,   1.0f, 1.0f,
//	-0.5f,  0.5f,  0.5f,	0.0f, 0.0f, 0.0f,   0.0f, 1.0f,
//	-0.5f, -0.5f,  0.5f,	0.0f, 0.0f, 0.0f,   0.0f, 0.0f,
//
//	-0.5f,  0.5f,  0.5f,	0.0f, 0.0f, 0.0f,   1.0f, 0.0f,
//	-0.5f,  0.5f, -0.5f,	0.0f, 0.0f, 0.0f,   1.0f, 1.0f,
//	-0.5f, -0.5f, -0.5f,	0.0f, 0.0f, 0.0f,   0.0f, 1.0f,
//	-0.5f, -0.5f, -0.5f,	0.0f, 0.0f, 0.0f,   0.0f, 1.0f,
//	-0.5f, -0.5f,  0.5f,	0.0f, 0.0f, 0.0f,   0.0f, 0.0f,
//	-0.5f,  0.5f,  0.5f,	0.0f, 0.0f, 0.0f,   1.0f, 0.0f,
//
//	0.5f,  0.5f,  0.5f,	0.0f, 0.0f, 0.0f,   1.0f, 0.0f,
//	0.5f,  0.5f, -0.5f,	0.0f, 0.0f, 0.0f,   1.0f, 1.0f,
//	0.5f, -0.5f, -0.5f,	0.0f, 0.0f, 0.0f,   0.0f, 1.0f,
//	0.5f, -0.5f, -0.5f,	0.0f, 0.0f, 0.0f,   0.0f, 1.0f,
//	0.5f, -0.5f,  0.5f,	0.0f, 0.0f, 0.0f,   0.0f, 0.0f,
//	0.5f,  0.5f,  0.5f,	0.0f, 0.0f, 0.0f,   1.0f, 0.0f,
//
//	-0.5f, -0.5f, -0.5f,	0.0f, 0.0f, 0.0f,   0.0f, 1.0f,
//	0.5f, -0.5f, -0.5f,	0.0f, 0.0f, 0.0f,   1.0f, 1.0f,
//	0.5f, -0.5f,  0.5f,	0.0f, 0.0f, 0.0f,   1.0f, 0.0f,
//	0.5f, -0.5f,  0.5f,	0.0f, 0.0f, 0.0f,   1.0f, 0.0f,
//	-0.5f, -0.5f,  0.5f,	0.0f, 0.0f, 0.0f,   0.0f, 0.0f,
//	-0.5f, -0.5f, -0.5f,	0.0f, 0.0f, 0.0f,   0.0f, 1.0f,
//
//	-0.5f,  0.5f, -0.5f,	0.0f, 0.0f, 0.0f,   0.0f, 1.0f,
//	0.5f,  0.5f, -0.5f,	0.0f, 0.0f, 0.0f,   1.0f, 1.0f,
//	0.5f,  0.5f,  0.5f,	0.0f, 0.0f, 0.0f,   1.0f, 0.0f,
//	0.5f,  0.5f,  0.5f,	0.0f, 0.0f, 0.0f,   1.0f, 0.0f,
//	-0.5f,  0.5f,  0.5f,	0.0f, 0.0f, 0.0f,   0.0f, 0.0f,
//	-0.5f,  0.5f, -0.5f,	0.0f, 0.0f, 0.0f,   0.0f, 1.0f
//};

#endif // !_VERTICES_H

