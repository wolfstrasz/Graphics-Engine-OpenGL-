#include "Camera.h"

Camera::Camera(unsigned int id)
	:  m_ID(id)
{
	UpdateCameraVectors();
}


void Camera::UpdateCameraVectors()
{
	// Calculate the new Front vector
	glm::vec3 front;
	front.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
	front.y = sin(glm::radians(m_Pitch));
	front.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
	m_Front = glm::normalize(front);
	// Also re-calculate the Right and Up vector
	// Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
	m_Right = glm::normalize(glm::cross(m_Front, glm::vec3(0.0f, 1.0f, 0.0f))); // cross product with the world's Up vector
	m_Up = glm::normalize(glm::cross(m_Right, m_Front));

	m_IsDirty = true;
}

void Camera::ProcessKeyboard(Camera_Movement direction, float deltaTime)
{
	float distance = m_MovementSpeed * deltaTime;
	if (direction == FORWARD)
		m_Position += m_Front * distance;
	if (direction == BACKWARD)
		m_Position -= m_Front * distance;
	if (direction == LEFT)
		m_Position -= m_Right * distance;
	if (direction == RIGHT)
		m_Position += m_Right * distance;

	m_IsDirty = true;
}
void Camera::ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch)
{
	xoffset *= m_MouseSensitivity;
	yoffset *= m_MouseSensitivity;

	m_Yaw += xoffset;
	m_Pitch += yoffset;

	// Make sure that when pitch is out of bounds, screen doesn't get flipped
	if (constrainPitch)
	{
		if (m_Pitch > Camera_Margins::PITCH_MAX)
			m_Pitch = Camera_Margins::PITCH_MAX;
		if (m_Pitch < Camera_Margins::PITCH_MIN)
			m_Pitch = Camera_Margins::PITCH_MIN;
	}

	m_IsDirty = true;
	// Update Front, Right and Up Vectors using the updated Euler angles
	UpdateCameraVectors();
}
void Camera::ProcessMouseScroll(float yoffset)
{
	if (m_Zoom >= Camera_Margins::ZOOM_MIN && m_Zoom <= Camera_Margins::ZOOM_MAX)
		m_Zoom -= yoffset;
	if (m_Zoom < Camera_Margins::ZOOM_MIN)
		m_Zoom = Camera_Margins::ZOOM_MIN;
	if (m_Zoom > Camera_Margins::ZOOM_MAX)
		m_Zoom = Camera_Margins::ZOOM_MAX;

	m_IsDirty = true;
}

// Getters
glm::mat4 Camera::GetView()
{
	if (m_IsDirty) {
		RecalculateMatrices();
	}
	return m_ViewMatrix;
}

unsigned int Camera::GetID() const
{
	return m_ID;
}

void Camera::SetPosition(glm::vec3 pos)
{
	m_Position = pos;
	m_IsDirty = true;
}


void Camera::SetNearPlane(float nearPlane) { m_NearPlane = nearPlane; m_IsDirty = true; }

void Camera::SetFarPlane(float farPlane) { m_FarPlane = farPlane; m_IsDirty = true;}

void Camera::SetRatio(float ratio)
{
	m_Ratio = ratio;
	m_IsDirty = true;
}

void Camera::SetSpeed(float speed)
{
	m_MovementSpeed = speed;
}

void Camera::Reset()
{
	m_Position = glm::vec3(0.0f);

	// Calculated at construction
	m_Front = glm::vec3(0.0f);
	m_Up = glm::vec3(0.0f);
	m_Right = glm::vec3(0.0f);

	// Euler Angles
	m_Yaw = Camera_Defaults::YAW;
	m_Pitch = Camera_Defaults::PITCH;

	// Camera options
	m_MovementSpeed = Camera_Defaults::SPEED;
	m_MouseSensitivity = Camera_Defaults::SENSITIVITY;
	m_Zoom = Camera_Defaults::ZOOM;
	

	// Camera planes
	m_NearPlane = Camera_Defaults::NEAR_PLANE;
	m_FarPlane = Camera_Defaults::FAR_PLANE;
	UpdateCameraVectors();
}

glm::vec3 Camera::GetFront() const
{
	return m_Front;
}

glm::vec3 Camera::GetPosition() const
{
	return m_Position;
}

float Camera::GetNearPlane() const { return m_NearPlane; }

float Camera::GetFarPlane() const { return m_FarPlane; }

glm::mat4 Camera::GetPerspective() {

	if (m_IsDirty) {
		RecalculateMatrices();
	}
	return m_PerspectiveMatrix;
}

void Camera::RecalculateMatrices() {

	m_ViewMatrix = glm::lookAt(m_Position, m_Position + m_Front, m_Up);
	m_PerspectiveMatrix = glm::perspective(glm::radians(m_Zoom), m_Ratio, m_NearPlane, m_FarPlane);
	m_IsDirty = false;

}
