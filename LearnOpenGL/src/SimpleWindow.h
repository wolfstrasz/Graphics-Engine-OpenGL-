#pragma once
#include "SimpleModel.h"

class SimpleWindow : public SimpleModel
{
static constexpr float sm_vertices[] = {

		// positions		// texture Coords
		-0.5f, -0.5f, 0.0f,		0.0f, 0.0f, 
		-0.5f,  0.5f, 0.0f,		0.0f, 1.0f, 
		 0.5f,  0.5f, 0.0f,		1.0f, 1.0f,
		 0.5f, -0.5f, 0.0f,		1.0f, 0.0f,

	};

	static constexpr unsigned int sm_indices[] = {
		0,  1,  2,
		0,  2,  3
	};
public:
	SimpleWindow() : SimpleModel(sm_vertices, sizeof(sm_indices) / sizeof(int), sm_indices, SM_Attrib::SM_POSITION | SM_Attrib::SM_TEXCOORD) {}
};