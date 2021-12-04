#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

class Transform
{
public:
	Transform(const Transform&) = default;
	Transform(const glm::vec3 & position = glm::vec3(0.0f), const glm::vec3 & rotation = glm::vec3(0.0f), const glm::vec3 & scale = glm::vec3(1.0f))
		: m_LocalPosition(position), m_LocalRotation(rotation), m_LocalScale(scale) {}

private:
	// the parent transform of this transform if it is null then 
	// the parent transform is the world coordinate system 
	Transform* m_Parent = nullptr;

	// all of the transforms that have this transform set as their parent private 
	std::vector<Transform*> m_Children = std::vector<Transform*>(0);

	// the position relative to the parent transform 
	glm::vec3 m_LocalPosition = glm::vec3(0.0f);

	// rotation relative to the parent 
	glm::vec3 m_LocalRotation = glm::vec3(0.0f);

	// scale relative to the parent 
	glm::vec3 m_LocalScale = glm::vec3(1.0f);

	// specifies if the localToWorldTransform needs to be recalulated 
	bool m_IsDirty = true;

	// the transform that converts local coordinates to world coordinates 
	glm::mat4 m_LocalToWorldMatrix = glm::mat4(1.0f);

	// the transform that converts world cooridnates to local coordinates 
	glm::mat4 m_WorldToLocalMatrix = glm::mat4(1.0f);

	// the transform that converts vertex normals to world coordinates.
	glm::mat4 m_NormalMatrix = glm::mat4(1.0f);

private:

	void SetDirty();

	void AddChild(Transform* childTransform);

	void RemoveChild(Transform* childTransform);

public:

	glm::mat3 GetNormalMatrix();

	void Print();
	glm::mat4 CalculateLocalToParentMatrix();
	glm::mat4 GetLocalToWorldMatrix();

	void SetParent(Transform* parentTransform);
	void RemoveParent();
	void SetLocalPosition(glm::vec3 value);
	void SetLocalRotation(glm::vec3 value);
	void SetLocalScale(glm::vec3 value);
	Transform* GetParent();
	glm::vec3 GetLocalPosition();
	glm::vec3 GetLocalRotation();
	glm::vec3 GetLocalScale();

	glm::vec3 GetPosition() {
		if (m_Parent == nullptr) {
			return GetLocalPosition();
		}
		else {
			return glm::vec3(m_Parent->GetLocalToWorldMatrix() * glm::vec4( m_LocalPosition, 0.0f)) + m_Parent->GetPosition();
		}
	}

	glm::vec3 GetRotation() {
		if (m_Parent == nullptr) {
			return GetLocalRotation();
		}
		else {
			return m_LocalRotation + m_Parent->GetRotation();
		}
	}


	void Rotate(glm::vec3 angle, glm::vec3 axis) {

	}

	void Translate(glm::vec3 translation) {
		SetLocalPosition(m_LocalPosition + translation);
	}

	void Scale(glm::vec3 scale) {

	}


};
