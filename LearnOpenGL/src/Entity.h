#pragma once
#include "Transform.h"

class Entity
{
public:
	Entity(const glm::vec3& position = glm::vec3(0.0f), const glm::vec3& rotation = glm::vec3(0.0f), const glm::vec3& scale = glm::vec3(1.0f))
	: Transform(position, rotation, scale) {}
private:

public:
	Transform Transform;

private:
};

