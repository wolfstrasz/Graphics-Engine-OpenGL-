#pragma once
#include "simple_model.h"
class SimplePlane : public SimpleModel
{
	static constexpr float sm_vertices[] = {
		// positions		   normals				 texCoords           tangets           bitangents
		-1.0f,  1.0f, 0.0f,    0.0f, 0.0f, 1.0f,    0.0f, 1.0f,     1.0f, 0.0f, 0.0f,    0.0f, 1.0f, 0.0f,
		-1.0f, -1.0f, 0.0f,    0.0f, 0.0f, 1.0f,    0.0f, 0.0f,     1.0f, 0.0f, 0.0f,    0.0f, 1.0f, 0.0f,
		 1.0f, -1.0f, 0.0f,    0.0f, 0.0f, 1.0f,    1.0f, 0.0f,     1.0f, 0.0f, 0.0f,    0.0f, 1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f,    0.0f, 0.0f, 1.0f,    0.0f, 1.0f,     1.0f, 0.0f, 0.0f,    0.0f, 1.0f, 0.0f,
		 1.0f, -1.0f, 0.0f,    0.0f, 0.0f, 1.0f,    1.0f, 0.0f,     1.0f, 0.0f, 0.0f,    0.0f, 1.0f, 0.0f,
		 1.0f,  1.0f, 0.0f,    0.0f, 0.0f, 1.0f,    1.0f, 1.0f,     1.0f, 0.0f, 0.0f,    0.0f, 1.0f, 0.0f
	};
	static constexpr unsigned int sm_indices[] = {
		0,  1,  2,
		3,  4,  5
	};
public:
	SimplePlane() : SimpleModel(sm_vertices, sizeof(sm_indices) / sizeof(int), sm_indices, SM_Attrib::SM_POSITION | SM_Attrib::SM_NORMAL | SM_Attrib::SM_TEXCOORD | SM_Attrib::SM_TANGENT | SM_Attrib:: SM_BITANGENT) {}
};