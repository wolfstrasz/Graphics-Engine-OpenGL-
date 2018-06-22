#include<camera.h>

Camera::Camera(glm::vec3 cameraPos, glm::vec3 cameraFront, glm::vec3 upVec, float speed)
{
	mPosition = cameraPos;
	mFront = cameraFront;
	mWorldUp = upVec;
	mMovementSpeed = speed;
}
Camera::Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) 
		: mFront(glm::vec3(0.0f, 0.0f, -1.0f)), mMovementSpeed(SPEED), mMouseSensitivity(SENSITIVITY), mZoom(ZOOM)
{
	mPosition = position;
	mWorldUp = up;
	mYaw = yaw;
	mPitch = pitch;
	updateCameraVectors();
}
Camera::Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) 
		: mFront(glm::vec3(0.0f, 0.0f, -1.0f)), mMouseSensitivity(SENSITIVITY), mMovementSpeed(SPEED), mZoom(ZOOM)
{
	mPosition = glm::vec3(posX, posY, posZ);
	mWorldUp = glm::vec3(upX, upY, upZ);
	mYaw = yaw;
	mPitch = pitch;
	updateCameraVectors();
}
void Camera::update()
{
	mView = glm::lookAt(mPosition, mPosition + mFront, mWorldUp);
}

void Camera::init()
{
	mView = glm::mat4(1.0f);
	mView = glm::lookAt(mPosition, mPosition + mFront, mWorldUp);
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
		if (mPitch > PITCH_MAX)
			mPitch = PITCH_MAX;
		if (mPitch < PITCH_MIN)
			mPitch = PITCH_MIN;
	}

	// Update Front, Right and Up Vectors using the updated Euler angles
	updateCameraVectors();
}

void Camera::ProcessMouseScroll(float yoffset)
{
	if (mZoom >= ZOOM_MIN && mZoom <= ZOOM_MAX)
		mZoom -= yoffset;
	if (mZoom < ZOOM_MIN)
		mZoom = ZOOM_MIN;
	if (mZoom > ZOOM_MAX)
		mZoom = ZOOM_MAX;
}

void Camera::moveForward(float deltaTime)
{
	float distance = mMovementSpeed * deltaTime;
	mPosition += glm::normalize(mFront) * distance;
}

void Camera::moveBackward(float deltaTime)
{
	float distance = mMovementSpeed * deltaTime;
	mPosition -= glm::normalize(mFront) * distance;
}

void Camera::moveLeft(float deltaTime)
{
	float distance = mMovementSpeed * deltaTime;
	mPosition -= glm::normalize(glm::cross(mFront, mWorldUp)) * distance;
	//	cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}

void Camera::moveRight(float deltaTime)
{
	float distance = mMovementSpeed * deltaTime;
	mPosition += glm::normalize(glm::cross(mFront, mWorldUp)) * distance;
}

glm::mat4 Camera::getView()
{
	return mView;
}

float Camera::getZoom()
{
	return mZoom;
}
