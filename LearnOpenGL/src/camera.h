#pragma once
#ifndef _CAMERA_H
#define _CAMERA_H
#include<glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Window.h"

// CAMERA MOVEMENT TYPES:
enum Camera_Movement 
{
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};
// CAMERA MARGIN VALUES:
struct Camera_Margins 
{
	static constexpr float PITCH_MAX =  89.0f;
	static constexpr float PITCH_MIN = -89.0f;
	static constexpr float ZOOM_MAX = 45.0f;
	static constexpr float ZOOM_MIN = 1.0f;
};
// CAMERA DEFAULT VALUES:
struct Camera_Defaults 
{
	static constexpr float YAW = -90.0f;
	static constexpr float PITCH = 0.0f;
	static constexpr float SPEED = 10.5f;
	static constexpr float SENSITIVITY = 0.1f;
	static constexpr float ZOOM = 45.0f;
	static constexpr float NEAR_PLANE = 0.1f;
	static constexpr float FAR_PLANE = 200.0f;
};

class Camera
{
private:
	// Camera Attributes
	unsigned int m_ID;
	bool m_IsDirty = true;
	glm::vec3 m_Position = glm::vec3(0.0f);

	// Calculated at construction
	glm::vec3 m_Front = glm::vec3(0.0f);
	glm::vec3 m_Up = glm::vec3(0.0f);
	glm::vec3 m_Right = glm::vec3(0.0f);

	// Euler Angles
	float m_Yaw = Camera_Defaults::YAW;
	float m_Pitch = Camera_Defaults::PITCH;

	// Camera options
	float m_MovementSpeed = Camera_Defaults::SPEED;
	float m_MouseSensitivity = Camera_Defaults::SENSITIVITY;
	float m_Zoom = Camera_Defaults::ZOOM;
	float m_Ratio;

	// Camera planes
	float m_NearPlane = Camera_Defaults::NEAR_PLANE;
	float m_FarPlane = Camera_Defaults::FAR_PLANE;

	// Private func
	void UpdateCameraVectors();
	void RecalculateMatrices();

	// Cached matrices
	glm::mat4 m_ViewMatrix;
	glm::mat4 m_PerspectiveMatrix;

public:

	//Constructors
	Camera(unsigned int id);

	// Functions
	void ProcessKeyboard(Camera_Movement direction, float deltaTime);
	void ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch = true);
	void ProcessMouseScroll(float yoffset);

	unsigned int GetID() const;
	float GetNearPlane() const;
	float GetFarPlane() const;

	glm::vec3 GetFront() const;
	glm::vec3 GetPosition() const;
	glm::mat4 GetView();
	glm::mat4 GetPerspective();
	
	void SetPosition(glm::vec3 pos);
	void SetNearPlane(float nearPlane);
	void SetFarPlane(float farPlane);
	void SetRatio(float ratio);
	void SetSpeed(float speed);
	void Reset();

};
#endif // !_CAMERA_H
