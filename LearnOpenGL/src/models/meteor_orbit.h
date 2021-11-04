#pragma once
#ifndef _METEOR_ORBIT_H
#define _METEOR_ORBIT_H
#include <glad/glad.h> // holds all OpenGL type declarations
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <vector>
#include <type_traits>

#include "shader.h"
#include "model.h"
#include <iostream>
struct MO_DEFAULTS
{
	static constexpr int COUNT = 5000;
	static constexpr float RADIUS = 50.0f;
	static constexpr float OFFSET = 10.0f;
	static constexpr float SCALE_MARGIN_TOP = 0.25f;
	static constexpr float SCALE_MARGIN_BOTTOM = 0.05f ;
};
class MeteorOrbit
{
public:
	MeteorOrbit(Model* meteorModel,
				glm::vec3 centerVector,
				int count = MO_DEFAULTS::COUNT,
				float radius = MO_DEFAULTS::RADIUS,
				float offset = MO_DEFAULTS::OFFSET,
				float scaleMarginTop = MO_DEFAULTS::SCALE_MARGIN_TOP,
				float scaleMarginBottom = MO_DEFAULTS::SCALE_MARGIN_BOTTOM)
	{
		mMeteorModel = meteorModel;
		mCenterVector = centerVector;
		mCount = count;
		mRadius = radius;
		mOffset = offset;
		mScaleMarginTop = scaleMarginTop;
		mScaleMarginBottom = scaleMarginBottom;
		generateMeteors();
		setup();
	}
private:
	Model* mMeteorModel;
	glm::vec3 mCenterVector;
	int mCount;
	float mRadius;
	float mOffset;
	float mScaleMarginTop;
	float mScaleMarginBottom;
	glm::mat4* mModelMatrices;
	unsigned int mMeteoriteBuffer;

	void generateMeteors()
	{
		mModelMatrices = new glm::mat4[mCount];
		srand((unsigned int)glfwGetTime());
		// Generation
		for (int i = 0; i < mCount; i++)
		{
			//std::cout << " METEORa: " << std::to_string(i) << std::endl;
			glm::mat4 model = glm::mat4(1.0f);
			// 1. translation: displace along circle with 'radius' in range [-offset, offset]
			float angle = (float)i / (float)mCount * 360.0f;
			float displacement = (rand() % (int)(2 * mOffset * 100)) / 100.0f - mOffset;
			float x = sin(angle) * mRadius + displacement;
			displacement = (rand() % (int)(2 * mOffset * 100)) / 100.0f - mOffset;
			float y = displacement * 0.4f; // keep height of asteroid field smaller compared to width of x and z
			displacement = (rand() % (int)(2 * mOffset * 100)) / 100.0f - mOffset;
			float z = cos(angle) * mRadius + displacement;
			//std::cout << " METEORb: " << std::to_string(i) << std::endl;
			model = glm::translate(model, glm::vec3(x, y, z));
			model = glm::translate(model, mCenterVector);
			//std::cout << " METEORc: " << std::to_string(i) << std::endl;
			// 2. scale: Scale between 0.05 and 0.25f
			int scaleDiff = (int)(mScaleMarginTop - mScaleMarginBottom);
			//std::cout << " METEORc1: " << std::to_string(i) << std::endl;
			float scale = (rand() % 20) / 100.0f + 0.05f;
			//std::cout << " METEORc2: " << std::to_string(i) << std::endl;
			model = glm::scale(model, glm::vec3(scale));
			//std::cout << " METEORd: " << std::to_string(i) << std::endl;
			// 3. rotation: add random rotation around a (semi)randomly picked rotation axis vector
			float rotAngle = (float)(rand() % 360);
			model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));
			//std::cout << " METEORe: " << std::to_string(i) << std::endl;
			// 4. now add to list of matrices
			mModelMatrices[i] = model;
		}
	}
	void setup() 
	{
		// configure instanced array
		// -------------------------
		//unsigned int meteoriteBuffer;
		glGenBuffers(1, &mMeteoriteBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, mMeteoriteBuffer);
		glBufferData(GL_ARRAY_BUFFER, mCount * sizeof(glm::mat4), &mModelMatrices[0], GL_STATIC_DRAW);

		// set transformation matrices as an instance vertex attribute (with divisor 1)
		// note: we're cheating a little by taking the, now publicly declared, VAO of the model's mesh(es) and adding new vertexAttribPointers
		// normally you'd want to do this in a more organized fashion, but for learning purposes this will do.
		// -----------------------------------------------------------------------------------------------------------------------------------
		for (unsigned int i = 0; i < mMeteorModel->meshes.size(); i++)
		{
			unsigned int meteorVAO = mMeteorModel->meshes[i].VAO;
			glBindVertexArray(meteorVAO);
			// set attribute pointers for matrix (4 times vec4)
			glEnableVertexAttribArray(3);
			glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
			glEnableVertexAttribArray(4);
			glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
			glEnableVertexAttribArray(5);
			glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
			glEnableVertexAttribArray(6);
			glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

			glVertexAttribDivisor(3, 1);
			glVertexAttribDivisor(4, 1);
			glVertexAttribDivisor(5, 1);
			glVertexAttribDivisor(6, 1);

			glBindVertexArray(0);
		}
	}
public:
	void draw(Shader shader)
	{
		shader.use();
		shader.setInt("texture_diffuse1", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mMeteorModel->textures_loaded[0].id); // note: we also made the textures_loaded vector public (instead of private) from the model class.
		for (unsigned int i = 0; i < mMeteorModel->meshes.size(); i++)
		{
			glBindVertexArray(mMeteorModel->meshes[i].VAO);
			glDrawElementsInstanced(GL_TRIANGLES, (GLsizei)mMeteorModel->meshes[i].indices.size(), GL_UNSIGNED_INT, 0, mCount);
			glBindVertexArray(0);
		}
	}

};
#endif // !_METEOR_ORBIT_H
