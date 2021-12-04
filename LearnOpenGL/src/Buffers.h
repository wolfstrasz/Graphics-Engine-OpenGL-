#pragma once
#include <glad\glad.h>
#include "RenderText.h"
#include "Transform.h"
#include "Camera.h"
#include "Window.h"

class Renderbuffer
{

private:
	GLuint m_InternalFormat;
	unsigned int m_Width;
	unsigned int m_Height;
	unsigned int m_ID;
	bool m_IsGenerated = false;
public:

	Renderbuffer(int width, int height, GLuint internalFormat) :
		m_Width(width), m_Height(height), m_InternalFormat(internalFormat)
	{
		m_IsGenerated = true;
		glGenRenderbuffers(1, &m_ID);
		glBindRenderbuffer(GL_RENDERBUFFER, m_ID);
		glRenderbufferStorage(GL_RENDERBUFFER, internalFormat, width, height);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
	}
	Renderbuffer() = default;

	~Renderbuffer() {
		if (m_IsGenerated) {
			glDeleteRenderbuffers(1, &m_ID);
		}
	}

	unsigned int GetID() const { return m_ID; }

};
