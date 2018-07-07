#pragma once
#ifndef _CAMERA_H
#define _CAMERA_H
#include<glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

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
	static constexpr float SPEED = 2.5f;
	static constexpr float SENSITIVITY = 0.1f;
	static constexpr float ZOOM = 45.0f;
};
class Camera
{
private:
	// Camera Attributes
	unsigned int ID;
	glm::vec3 mPosition;
	glm::vec3 mFront;
	glm::vec3 mWorldUp;
	glm::vec3 mUp;
	glm::vec3 mRight;
	// Euler Angles
	float mYaw;
	float mPitch;
	// Camera options
	float mMovementSpeed;
	float mMouseSensitivity;
	float mZoom;
	// Private func
	void updateCameraVectors();

public:
	//Constructors
	Camera(unsigned int id, glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
			float yaw = Camera_Defaults::YAW, float pitch = Camera_Defaults::PITCH);
	Camera(unsigned int id, float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch);
	// Functions
	void update();
	void init();
	void processKeyboard(Camera_Movement direction, float deltaTime);
	void ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch = true);
	void ProcessMouseScroll(float yoffset);
	// Getters
	glm::mat4 getView();
	float getZoom();
	unsigned int getID();
	glm::vec3 getPosition();
	glm::vec3 getFront();
};
#endif // !_CAMERA_H
