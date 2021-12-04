#include "Transform.h"

void Transform::SetDirty()
{
	// only update dirty boolean if it isn't already dirty 
	if (!m_IsDirty)
	{
		m_IsDirty = true;
		// set all children to be dirty since any modification of a parent 
		// transform also effects its children's localToWorldTransform 
		for (int i = 0; i < m_Children.size(); ++i) {
			m_Children[i]->SetDirty();
		}
	}

}

void Transform::AddChild(Transform* childTransform)
{
	// Check for duplication
	for (int i = 0; i < m_Children.size(); ++i) {
		if (m_Children[i] == childTransform) return;
	}
	m_Children.push_back(childTransform);
}

void Transform::RemoveChild(Transform* childTransform)
{
	auto it = m_Children.begin();

	for (; it != m_Children.end(); ++it) {
		if (*it == childTransform) {
			break;
		}
	}

	m_Children.erase(it);
}


glm::mat3 Transform::GetNormalMatrix()
{

	if (m_IsDirty) {
		return glm::mat3(transpose(inverse(GetLocalToWorldMatrix())));
	}
	return m_NormalMatrix;
}

void Transform::Print() {

	printf("%f, %f, %f\n", m_LocalPosition.x, m_LocalPosition.y, m_LocalPosition.z);
	printf("%f, %f, %f\n", m_LocalRotation.x, m_LocalRotation.y, m_LocalRotation.z);
	printf("%f, %f, %f\n", m_LocalScale.x, m_LocalScale.y, m_LocalScale.z);
}

glm::mat4 Transform::CalculateLocalToParentMatrix() {

	return glm::translate(glm::mat4(1.0f), m_LocalPosition)
		* glm::mat4_cast(glm::quat(glm::vec3(glm::radians(m_LocalRotation.x), glm::radians(m_LocalRotation.y), glm::radians(m_LocalRotation.z))))
		* glm::scale(glm::mat4(1.0f), m_LocalScale);
}

// gets the matrix that converts from local coordinates to world coordinates 

glm::mat4 Transform::GetLocalToWorldMatrix() {

	if (m_IsDirty)
	{
		//printf("DIRTY!\n");
		if (m_Parent == nullptr)
		{
			m_LocalToWorldMatrix = CalculateLocalToParentMatrix();
		}
		else {
			// Go up the parent tree
			m_LocalToWorldMatrix = m_Parent->GetLocalToWorldMatrix() * CalculateLocalToParentMatrix();
		}
		m_IsDirty = false;
		m_NormalMatrix = glm::mat3(transpose(inverse(GetLocalToWorldMatrix())));
	}
	return m_LocalToWorldMatrix;
}

void Transform::SetParent(Transform* parentTransform) {
	if (m_Parent != nullptr) {
		m_Parent->RemoveChild(this);
	}

	// assign new parent 
	m_Parent = parentTransform;

	// add this to new parent 
	if (m_Parent != nullptr)
	{
		m_Parent->AddChild(this);
	}
	// changes parents effects the world position 
	SetDirty();
}

inline void Transform::RemoveParent() {
	SetParent(nullptr);
}

void Transform::SetLocalPosition(glm::vec3 value)
{
	m_LocalPosition = value;
	SetDirty();
}

void Transform::SetLocalRotation(glm::vec3 value)
{
	m_LocalRotation = value;
	SetDirty();
}

void Transform::SetLocalScale(glm::vec3 value)
{
	m_LocalScale = value;
	SetDirty();
}

Transform* Transform::GetParent() { return m_Parent; }

glm::vec3 Transform::GetLocalPosition() { return m_LocalPosition; }

glm::vec3 Transform::GetLocalRotation() { return m_LocalRotation; }

glm::vec3 Transform::GetLocalScale() { return m_LocalScale; }
