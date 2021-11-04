#pragma once
#include "simple_model.h"

class SimpleWindow : public SimpleModel
{
	static constexpr float sm_vertices[] = {
		// positions		  texCoords       
		0.0f,  0.5f,  0.0f,   0.0f,  0.0f,
		0.0f, -0.5f,  0.0f,   0.0f,  1.0f,
		1.0f, -0.5f,  0.0f,   1.0f,  1.0f,

		0.0f,  0.5f,  0.0f,   0.0f,  0.0f,
		1.0f, -0.5f,  0.0f,   1.0f,  1.0f,
		1.0f,  0.5f,  0.0f,   1.0f,  0.0f
	};
	static constexpr unsigned int sm_indices[] = {
		0,  1,  2,
		3,  4,  5
	};
public:
	SimpleWindow() : SimpleModel(sm_vertices, sizeof(sm_indices) / sizeof(int), sm_indices, SM_Attrib::SM_POSITION | SM_Attrib::SM_TEXCOORD) {}
};