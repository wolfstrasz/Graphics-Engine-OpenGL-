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

#include "Shader.h"
#include "Model.h"
#include "Transform.h"
#include <iostream>

class MeteorOrbit
{
	struct MO_DEFAULTS
	{
		static constexpr int COUNT = 5000;
		static constexpr float RADIUS = 50.0f;
		static constexpr float OFFSET = 10.0f;
		static constexpr float SCALE_MARGIN_TOP = 0.25f;
		static constexpr float SCALE_MARGIN_BOTTOM = 0.05f;
	};

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
		m_Count = count;
		m_Radius = radius;
		m_Offset = offset;
		m_ScaleMarginTop = scaleMarginTop;
		m_ScaleMarginBottom = scaleMarginBottom;
		GenerateMeteors();
		Setup();
	}

	Transform transform;

private:
	Model* mMeteorModel;
	glm::vec3 mCenterVector;
	int m_Count;
	float m_Radius;
	float m_Offset;
	float m_ScaleMarginTop;
	float m_ScaleMarginBottom;

	glm::mat4* m_ModelMatrices;
	glm::mat3* m_tiModelMatrices;

	unsigned int mMeteoriteBuffer;
	unsigned int mtiMeteoriteBuffer;


	void GenerateMeteors()
	{
		m_ModelMatrices = new glm::mat4[m_Count];
		m_tiModelMatrices = new glm::mat3[m_Count];
		srand((unsigned int)glfwGetTime());
		// Generation
		for (int i = 0; i < m_Count; i++)
		{
			glm::mat4 model = glm::mat4(1.0f);
			// 1. translation: displace along circle with 'radius' in range [-offset, offset]
			float angle = (float)i / (float)m_Count * 360.0f;
			float displacement = (rand() % (int)(2 * m_Offset * 100)) / 100.0f - m_Offset;
			float x = sin(angle) * m_Radius + displacement;
			displacement = (rand() % (int)(2 * m_Offset * 100)) / 100.0f - m_Offset;
			float y = displacement * 0.4f; // keep height of asteroid field smaller compared to width of x and z
			displacement = (rand() % (int)(2 * m_Offset * 100)) / 100.0f - m_Offset;
			float z = cos(angle) * m_Radius + displacement;
			model = glm::translate(model, glm::vec3(x, y, z));
			model = glm::translate(model, mCenterVector);
			// 2. scale: Scale between 0.05 and 0.25f
			int scaleDiff = (int)(m_ScaleMarginTop - m_ScaleMarginBottom);
			float scale = (rand() % 20) / 100.0f + 0.05f;
			model = glm::scale(model, glm::vec3(scale));
			// 3. rotation: add random rotation around a (semi)randomly picked rotation axis vector
			float rotAngle = (float)(rand() % 360);
			model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));
			// 4. now add to list of matrices
			m_ModelMatrices[i] = model;
			m_tiModelMatrices[i] = glm::mat3(glm::transpose(glm::inverse(model)));
		}
	}

	void Setup() 
	{
		// configure instanced array
		// -------------------------
		//unsigned int meteoriteBuffer;
		glGenBuffers(1, &mMeteoriteBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, mMeteoriteBuffer);
		glBufferData(GL_ARRAY_BUFFER, m_Count * sizeof(glm::mat4), &m_ModelMatrices[0], GL_STATIC_DRAW);


		// set transformation matrices as an instance vertex attribute (with divisor 1)
		// note: we're cheating a little by taking the, now publicly declared, VAO of the model's mesh(es) and adding new vertexAttribPointers
		// normally you'd want to do this in a more organized fashion, but for learning purposes this will do.
		// -----------------------------------------------------------------------------------------------------------------------------------
		for (unsigned int i = 0; i < mMeteorModel->meshes.size(); i++)
		{
			unsigned int meteorVAO = mMeteorModel->meshes[i].GetVAO();
			glBindVertexArray(meteorVAO);

			// set attribute pointers for matrix (4 times vec4)
			glEnableVertexAttribArray(5);
			glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
			glEnableVertexAttribArray(6);
			glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
			glEnableVertexAttribArray(7);
			glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
			glEnableVertexAttribArray(8);
			glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

			glVertexAttribDivisor(5, 1);
			glVertexAttribDivisor(6, 1);
			glVertexAttribDivisor(7, 1);
			glVertexAttribDivisor(8, 1);

			glBindVertexArray(0);
		}

		glGenBuffers(1, &mtiMeteoriteBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, mtiMeteoriteBuffer);
		glBufferData(GL_ARRAY_BUFFER, m_Count * sizeof(glm::mat3), &m_tiModelMatrices[0], GL_STATIC_DRAW);

		for (unsigned int i = 0; i < mMeteorModel->meshes.size(); i++)
		{
			unsigned int meteorVAO = mMeteorModel->meshes[i].GetVAO();
			glBindVertexArray(meteorVAO);

			glEnableVertexAttribArray(9);
			glVertexAttribPointer(9, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat3), (void*)0);
			glEnableVertexAttribArray(10);
			glVertexAttribPointer(10, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat3), (void*)(sizeof(glm::vec3)));
			glEnableVertexAttribArray(11);
			glVertexAttribPointer(11, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat3), (void*)(2 * sizeof(glm::vec3)));

			glVertexAttribDivisor(9, 1);
			glVertexAttribDivisor(10, 1);
			glVertexAttribDivisor(11, 1);

			glBindVertexArray(0);
		}

	}
public:
	void Draw(std::shared_ptr<Shader>& shader)
	{
		shader->Use();
		shader->SetMat4("baseModel", transform.GetLocalToWorldMatrix());
		shader->SetMat3("tiBaseModel", transform.GetNormalMatrix());

		for (unsigned int i = 0; i < mMeteorModel->meshes.size(); i++)
		{

			mMeteorModel->meshes[i].ApplyTextures(shader);
			glBindVertexArray(mMeteorModel->meshes[i].GetVAO());
			glDrawElementsInstanced(GL_TRIANGLES, (GLsizei)mMeteorModel->meshes[i].GetIndicesSize(), GL_UNSIGNED_INT, 0, m_Count);
			glBindVertexArray(0);
		}
	}

};
#endif // !_METEOR_ORBIT_H
