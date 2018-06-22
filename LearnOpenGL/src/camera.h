#pragma once
#ifndef _CAMERA_H
#define _CAMERA_H
#include<glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Camera
{
	//Constructors
public:
	Camera(glm::vec3 cameraPos, glm::vec3 cameraFront, glm::vec3 upVec, float speed);

	// Private vars/func
private:
	float mSpeed;
	glm::vec3 mPos;
	glm::vec3 mFront;
	glm::vec3 mUpVec;
	glm::mat4 mView;
	// Public vars/func
public:
	void update();
	void init();
	void moveForward(float deltaTime);
	void moveBackward(float deltaTime);
	void moveLeft(float deltaTime);
	void moveRight(float deltaTime);
	glm::mat4 getView();

};
#endif // !_CAMERA_H
