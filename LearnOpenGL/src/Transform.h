#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


struct Transform
{
	glm::vec3 Position = { 0.0f, 0.0f, 0.0f };
	glm::vec3 Rotation = { 0.0f, 0.0f, 0.0f };
	glm::vec3 Scale = { 1.0f, 1.0f, 1.0f };

	Transform(const Transform&) = default;
	Transform(const glm::vec3& position = glm::vec3(0.0f), const glm::vec3& rotation = glm::vec3(0.0f), const glm::vec3& scale = glm::vec3(1.0f)) 
	: Position(position), Rotation(rotation), Scale(scale) {}

	glm::mat4 GetTransform() const
	{
		return glm::translate(glm::mat4(1.0f), Position)
			* glm::mat4_cast(glm::quat(glm::vec3(glm::radians(Rotation.x), glm::radians(Rotation.y), glm::radians(Rotation.z))))
			* glm::scale(glm::mat4(1.0f), Scale);
	}

	glm::mat3 GetTransposeInverseTransform() const
	{
		return glm::mat3(transpose(inverse(GetTransform())));
	}

	void Print() {
		printf("%f, %f, %f\n", Position.x, Position.y, Position.z);
		printf("%f, %f, %f\n", Rotation.x, Rotation.y, Rotation.z);
		printf("%f, %f, %f\n", Scale.x, Scale.y, Scale.z);
	}

};

