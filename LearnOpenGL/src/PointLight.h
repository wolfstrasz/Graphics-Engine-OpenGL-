#pragma once
#ifndef _POINT_LIGHT_H
#define _POINT_LIGHT_H
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string.h>
#include "Shader.h"


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
	enum Point_Light_Vector_Attributes
	{
		POSITION,
		AMBIENT,
		DIFFUSE,
		SPECULAR
	};

	enum Point_Light_Float_Attributes
	{
		DISTANCE
	};

	PointLight(glm::vec3 position = Point_Light_Defaults::POSITION,
		glm::vec3 ambient = Point_Light_Defaults::AMBIENT,
		glm::vec3 diffuse = Point_Light_Defaults::DIFFUSE,
		glm::vec3 specular = Point_Light_Defaults::SPECULAR,
		float distance = Point_Light_Defaults::DISTANCE);
private:
	glm::vec3 m_Position;
	glm::vec3 m_Ambient;
	glm::vec3 m_Diffuse;
	glm::vec3 m_Specular;
	float m_Constant;
	float m_Linear;
	float m_Quadratic;

	void generateAttenuationTerms(float distance);

public:
	glm::vec3 GetPosition();
	glm::vec3 GetColour();
	void ApplyLight(std::shared_ptr<Shader>& shader, unsigned int pos) const;
	void SetVec3(Point_Light_Vector_Attributes attrib, glm::vec3 value);
	void SetFloat(Point_Light_Float_Attributes attrib, float value);
};
#endif // !_POINT_LIGHT_H
