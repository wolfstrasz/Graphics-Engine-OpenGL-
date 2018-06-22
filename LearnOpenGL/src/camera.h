#pragma once
#ifndef _CAMERA_H
#define _CAMERA_H
#include<glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

enum Camera_Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

// DEFAULT VALUES:
const float YAW		= -90.0f;
const float PITCH	= 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;

// MARGIN VALUES:
const float PITCH_MAX = 89.0f;
const float PITCH_MIN = -89.0f;
const float ZOOM_MAX = 45.0f;
const float ZOOM_MIN = 1.0f;

class Camera
{


private:
	// Camera Attributes
	//float mMovementSpeed;
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
	// Camera View matrix;
	glm::mat4 mView;
	// Private func
	void updateCameraVectors();

public:
	//Constructors
	Camera(glm::vec3 cameraPos, glm::vec3 cameraFront, glm::vec3 upVec, float speed);
	Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch);
	Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch);

	//Functions
	void update();
	void init();
	void processKeyboard(Camera_Movement direction, float deltaTime);
	void ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch = true);
	void ProcessMouseScroll(float yoffset);

	void moveForward(float deltaTime);
	void moveBackward(float deltaTime);
	void moveLeft(float deltaTime);
	void moveRight(float deltaTime);
	glm::mat4 getView();

};
#endif // !_CAMERA_H
