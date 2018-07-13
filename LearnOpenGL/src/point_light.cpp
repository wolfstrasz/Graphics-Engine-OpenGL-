#include "point_light.h"



PointLight::PointLight(glm::vec3 position, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float distance)
{
	glm::vec3 attTerms;
	generateAttenuationTerms(attTerms, distance);
	mPosition = position;
	mAmbient = ambient;
	mDiffuse = diffuse;
	mSpecular = specular;
	mConstant = attTerms.x;
	mLinear = attTerms.y;
	mQuadratic = attTerms.z;
}

// utility function for calculating light attentuation terms
// ---------------------------------------------------------
void PointLight::generateAttenuationTerms(glm::vec3 &attenuationTerms, float distance)
{
	if (distance < 1.0f)
	{
		std::cout << "ATTENUATION::TERMS::GENERATION:: Distance value invalid";
		return;
	}
	attenuationTerms.x = 1.0f;
	attenuationTerms.y = Point_Light_Constants::LINEAR / distance;
	attenuationTerms.z = Point_Light_Constants::QUADRATIC / (distance * distance - distance);
}

void PointLight::setLight(Shader & shader, unsigned int pos)
{
	std::string index = std::to_string(pos);
	shader.setVec3("pointLights[" + index + "].position",	mPosition);
	shader.setVec3("pointLights[" + index + "].ambient",	mAmbient);
	shader.setVec3("pointLights[" + index + "].diffuse",	mDiffuse);
	shader.setVec3("pointLights[" + index + "].specular",	mSpecular);

	shader.setFloat("pointLights[" + index + "].constant",	mConstant);
	shader.setFloat("pointLights[" + index + "].linear",	mLinear);
	shader.setFloat("pointLights[" + index + "].quadratic",	mQuadratic);
}
