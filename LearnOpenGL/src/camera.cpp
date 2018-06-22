#include<camera.h>

Camera::Camera(glm::vec3 cameraPos, glm::vec3 cameraFront, glm::vec3 upVec, float speed)
{
	mPos = cameraPos;
	mFront = cameraFront;
	mUpVec = upVec;
	mSpeed = speed;
}

void Camera::update()
{
	mView = glm::lookAt(mPos, mPos + mFront, mUpVec);
}

void Camera::init()
{
	mView = glm::mat4(1.0f);
	mView = glm::lookAt(mPos, mPos + mFront, mUpVec);
}

void Camera::moveForward(float deltaTime)
{
	float distance = mSpeed * deltaTime;
	mPos += glm::normalize(mFront) * distance;
}

void Camera::moveBackward(float deltaTime)
{
	float distance = mSpeed * deltaTime;
	mPos -= glm::normalize(mFront) * distance;
}

void Camera::moveLeft(float deltaTime)
{
	float distance = mSpeed * deltaTime;
	mPos -= glm::normalize(glm::cross(mFront, mUpVec)) * distance;
	//	cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}

void Camera::moveRight(float deltaTime)
{
	float distance = mSpeed * deltaTime;
	mPos += glm::normalize(glm::cross(mFront, mUpVec)) * distance;
}

glm::mat4 Camera::getView()
{
	return mView;
}
