#include "spot_light.h"

SpotLight::SpotLight(glm::vec3 position, glm::vec3 direction, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float distance, float cutOffAngle, float outerCutOffAngle)
{
	mPosition = position;
	mDirection = direction;
	mAmbient = ambient;
	mDiffuse = diffuse;
	mSpecular = specular;
	generateAttenuationTerms(distance);
	mCutOff = glm::cos(glm::radians(cutOffAngle));
	mOuterCutOff = glm::cos(glm::radians(outerCutOffAngle));
}

void SpotLight::generateAttenuationTerms(float distance)
{
	if (distance < 1.0f)
	{
		std::cout << "ATTENUATION::TERMS::GENERATION:: Distance value invalid";
		return;
	}
	mConstant = 1.0f;
	mLinear = Spot_Light_Constants::LINEAR / distance;
	mQuadratic = Spot_Light_Constants::QUADRATIC / (distance * distance - distance);
}

void SpotLight::setLight(Shader & shader, unsigned int pos)
{
	std::string index = std::to_string(pos);
	shader.setVec3("spotLights[" + index + "].position", mPosition);
	shader.setVec3("spotLights[" + index + "].direction", mDirection);
	shader.setVec3("spotLights[" + index + "].ambient", mAmbient);
	shader.setVec3("spotLights[" + index + "].diffuse", mDiffuse);
	shader.setVec3("spotLights[" + index + "].specular", mSpecular);
	shader.setFloat("spotLights[" + index + "].constant", mConstant);
	shader.setFloat("spotLights[" + index + "].linear", mLinear);
	shader.setFloat("spotLights[" + index + "].quadratic", mQuadratic);
	shader.setFloat("spotLights[" + index + "].cutOff", mCutOff);
	shader.setFloat("spotLights[" + index + "].outerCutOff", mOuterCutOff);
}

void SpotLight::updatePosition(glm::vec3 position)
{
	mPosition = position;
}

void SpotLight::updateDirection(glm::vec3 direction)
{
	mDirection = direction;
}
