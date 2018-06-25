#include "camera.h"

Camera::Camera(glm::vec3 cameraPos, glm::vec3 cameraFront, glm::vec3 upVec, float speed)
{
	mPosition = cameraPos;
	mFront = cameraFront;
	mWorldUp = upVec;
	mMovementSpeed = speed;
}
Camera::Camera(glm::vec3 position , glm::vec3 up , float yaw , float pitch) 
		: mFront(glm::vec3(0.0f, 0.0f, -1.0f)), 
		  mMovementSpeed(Camera_Defaults::SPEED),
		  mMouseSensitivity(Camera_Defaults::SENSITIVITY),
		  mZoom(Camera_Defaults::ZOOM)
{
	mPosition = position;
	mWorldUp = up;
	mYaw = yaw;
	mPitch = pitch;
	updateCameraVectors();
}
Camera::Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) 
		: mFront(glm::vec3(0.0f, 0.0f, -1.0f)), 
		  mMouseSensitivity(Camera_Defaults::SENSITIVITY), 
		  mMovementSpeed(Camera_Defaults::SPEED),
		  mZoom(Camera_Defaults::ZOOM)
{
	mPosition = glm::vec3(posX, posY, posZ);
	mWorldUp = glm::vec3(upX, upY, upZ);
	mYaw = yaw;
	mPitch = pitch;
	updateCameraVectors();
}
void Camera::update()
{
}
void Camera::init()
{
}
void Camera::updateCameraVectors()
{
	// Calculate the new Front vector
	glm::vec3 front;
	front.x = cos(glm::radians(mYaw)) * cos(glm::radians(mPitch));
	front.y = sin(glm::radians(mPitch));
	front.z = sin(glm::radians(mYaw)) * cos(glm::radians(mPitch));
	mFront = glm::normalize(front);
	// Also re-calculate the Right and Up vector
	// Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
	mRight = glm::normalize(glm::cross(mFront, mWorldUp));  
	mUp = glm::normalize(glm::cross(mRight, mFront));
}
void Camera::processKeyboard(Camera_Movement direction, float deltaTime)
{
	float distance = mMovementSpeed * deltaTime;
	if (direction == FORWARD)
		mPosition += mFront * distance;
	if (direction == BACKWARD)
		mPosition -= mFront * distance;
	if (direction == LEFT)
		mPosition -= mRight * distance;
	if (direction == RIGHT)
		mPosition += mRight * distance;
}
void Camera::ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch)
{
	xoffset *= mMouseSensitivity;
	yoffset *= mMouseSensitivity;

	mYaw += xoffset;
	mPitch += yoffset;

	// Make sure that when pitch is out of bounds, screen doesn't get flipped
	if (constrainPitch)
	{
		if (mPitch > Camera_Margins::PITCH_MAX)
			mPitch = Camera_Margins::PITCH_MAX;
		if (mPitch < Camera_Margins::PITCH_MIN)
			mPitch = Camera_Margins::PITCH_MIN;
	}

	// Update Front, Right and Up Vectors using the updated Euler angles
	updateCameraVectors();
}
void Camera::ProcessMouseScroll(float yoffset)
{
	if (mZoom >= Camera_Margins::ZOOM_MIN && mZoom <= Camera_Margins::ZOOM_MAX)
		mZoom -= yoffset;
	if (mZoom < Camera_Margins::ZOOM_MIN)
		mZoom = Camera_Margins::ZOOM_MIN;
	if (mZoom > Camera_Margins::ZOOM_MAX)
		mZoom = Camera_Margins::ZOOM_MAX;
}
glm::mat4 Camera::getView()
{
	return glm::lookAt(mPosition, mPosition + mFront, mUp);
}
float Camera::getZoom()
{
	return mZoom;
}
