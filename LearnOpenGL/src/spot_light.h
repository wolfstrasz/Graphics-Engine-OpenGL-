#pragma once
#ifndef _SPOT_LIGHT_H
#define _SPOT_LIGHT_H
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include "shader.h"

struct Spot_Light_Constants {
	static constexpr float LINEAR = 4.5f;
	static constexpr float QUADRATIC = 73.0f;
};
struct Spot_Light_Defaults {
	static constexpr glm::vec3 POSITION = glm::vec3(0.0f, 0.0f, 0.0f);
	static constexpr glm::vec3 DIRECTION = glm::vec3(0.0f, 0.0f, 0.0f);
	static constexpr glm::vec3 AMBIENT = glm::vec3(0.0f, 0.0f, 0.0f);
	static constexpr glm::vec3 DIFFUSE = glm::vec3(1.0f, 1.0f, 1.0f);
	static constexpr glm::vec3 SPECULAR = glm::vec3(1.0f, 1.0f, 1.0f);
	static constexpr float DISTANCE = 50.0f;
	static constexpr float CUT_OFF_ANGLE = 12.5f;
	static constexpr float OUTER_CUT_OFF_ANGLE = 15.0f;
};

//glm::cos(glm::radians(12.5f));
class SpotLight
{
public:
	enum Spot_Light_Vector_Attributes
	{
		POSITION,
		DIRECTION,
		AMBIENT,
		DIFFUSE,
		SPECULAR
	};

	enum Spot_Light_Float_Attributes
	{
		DISTANCE,
		CUT_OFF_ANGLE,
		OUTER_CUT_OFF_ANGLE
	};

	SpotLight(glm::vec3 position = Spot_Light_Defaults::POSITION,
		glm::vec3 direction = Spot_Light_Defaults::DIRECTION,
		glm::vec3 ambient = Spot_Light_Defaults::AMBIENT,
		glm::vec3 diffuse = Spot_Light_Defaults::DIFFUSE,
		glm::vec3 specular = Spot_Light_Defaults::SPECULAR,
		float distance = Spot_Light_Defaults::DISTANCE,
		float cutOffAngle = Spot_Light_Defaults::CUT_OFF_ANGLE,
		float outerCutOffAngle = Spot_Light_Defaults::OUTER_CUT_OFF_ANGLE);
private:
	glm::vec3 mPosition;
	glm::vec3 mDirection;
	glm::vec3 mAmbient;
	glm::vec3 mDiffuse;
	glm::vec3 mSpecular;
	float mCutOff;
	float mOuterCutOff;
	float mConstant;
	float mLinear;
	float mQuadratic;

	void generateAttenuationTerms(float distance);
public:
	void setLight(Shader& shader, unsigned int pos);
	void SetVec3(Spot_Light_Vector_Attributes attrib, glm::vec3 value);
	void setFloat(Spot_Light_Float_Attributes attrib, float value);
};
#endif // !_SPOT_LIGHT
