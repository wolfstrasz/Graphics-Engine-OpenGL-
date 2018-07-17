#pragma once
#ifndef _SIMPLE_PLANE_H
#define _SIMPLE_PLANE_H
#include "simple_model.h"

#define SIMPLE_PLANE_NR_INDICES 6
#define SIMPLE_PLANE_VERTEX_SIZE 8
#define SIMPLE_PLANE_FLAGS (SM_ATTRIBS::SM_POSITION | SM_ATTRIBS::SM_NORMAL | SM_ATTRIBS::SM_TEXCOORD)

class SimplePlane : public SimpleModel
{
	static constexpr float cube_vertices[SIMPLE_PLANE_VERTEX_SIZE * SIMPLE_PLANE_NR_INDICES] = {
		// positions			normals				texCoords
		-5.0f, -0.5f,  -5.0f,   0.0f,  1.0f,  0.0f,   0.0f, 4.0f,
		5.0f, -0.5f,  -5.0f,  0.0f,  1.0f,  0.0f,   4.0f, 4.0f,
		5.0f, -0.5f, 5.0f,  0.0f,  1.0f,  0.0f,   4.0f, 0.0f,

		5.0f, -0.5f,  5.0f,   0.0f,  1.0f,  0.0f,   4.0f, 0.0f,
		-5.0f, -0.5f, 5.0f,  0.0f,  1.0f,  0.0f,   0.0f, 0.0f,
		-5.0f, -0.5f, -5.0f,   0.0f,  1.0f,  0.0f,   0.0f, 4.0f
	};
	static constexpr unsigned int cube_indices[SIMPLE_PLANE_NR_INDICES] = {
		0,  1,  2,
		3,  4,  5
	};
public:
	SimplePlane() : SimpleModel(cube_vertices, SIMPLE_PLANE_NR_INDICES, cube_indices, SIMPLE_PLANE_FLAGS) {}
};
#endif // !_SIMPLE_PLANE_H