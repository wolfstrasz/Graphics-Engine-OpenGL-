#pragma once
#ifndef _POINT_LIGHT_H
#define _POINT_LIGHT_H
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <string.h>
#include "shader.h"

struct Point_Light_Constants {
	static constexpr float LINEAR = 4.5f;
	static constexpr float QUADRATIC = 73.0f;
};
struct Point_Light_Defaults {
	static constexpr glm::vec3 POSITION = glm::vec3(0.0f, 0.0f, 0.0f);
	static constexpr glm::vec3 AMBIENT = glm::vec3(0.05f, 0.05f, 0.05f);
	static constexpr glm::vec3 DIFFUSE = glm::vec3(0.8f, 0.8f, 0.8f);
	static constexpr glm::vec3 SPECULAR = glm::vec3(1.0f, 1.0f, 1.0f);
	static constexpr float DISTANCE = 50.0f;
};

class PointLight
{
public:
	PointLight(glm::vec3 position = Point_Light_Defaults::POSITION,
		glm::vec3 ambient = Point_Light_Defaults::AMBIENT,
		glm::vec3 diffuse = Point_Light_Defaults::DIFFUSE,
		glm::vec3 specular = Point_Light_Defaults::SPECULAR,
		float distance = Point_Light_Defaults::DISTANCE);
private:
	glm::vec3 mPosition;
	glm::vec3 mAmbient;
	glm::vec3 mDiffuse;
	glm::vec3 mSpecular;
	float mConstant;
	float mLinear;
	float mQuadratic;

	void generateAttenuationTerms(glm::vec3 &attenuationTerms, float distance);

public:
	void setLight(Shader& shader, unsigned int pos);
};
#endif // !_POINT_LIGHT_H
