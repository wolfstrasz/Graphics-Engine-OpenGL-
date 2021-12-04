#pragma once

#include <glad/glad.h> // holds all OpenGL type declarations
#include <glm/glm.hpp>
#include <iostream>

class RenderTexture {

protected:
	unsigned int m_ID, m_Width, m_Height;
	GLenum m_Type, m_InternalFormat, m_Format, m_DataFormat, m_Wrapping, m_Filter;
	bool m_IsGenerated = false;

public:
	RenderTexture() = delete;
	RenderTexture(
		unsigned int width,
		unsigned int height,
		GLenum type,
		GLenum internalFormat,
		GLenum format,
		GLenum dataFormat,
		GLenum filter = GL_LINEAR,
		GLenum wrapping = GL_CLAMP_TO_EDGE,
		bool whiteborder = false
		) :
		m_Type(type),
		m_Width(width),
		m_Height(height),
		m_InternalFormat(internalFormat),
		m_Format(format),
		m_DataFormat(dataFormat),
		m_Filter(filter),
		m_Wrapping(wrapping)
	{
		m_IsGenerated = true;
		glGenTextures(1, &m_ID);
		glBindTexture(m_Type, m_ID);

		switch (m_Type) 
		{
			case GL_TEXTURE_2D: 
				glTexImage2D(m_Type, 0, m_InternalFormat, m_Width, m_Height, 0, m_Format, m_DataFormat, NULL);
			case GL_TEXTURE_CUBE_MAP:
				for (unsigned int i = 0; i < 6; ++i) {
					glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internalFormat, width, height, 0, format, GL_FLOAT, NULL);
				}
		}
		
		glTexParameteri(m_Type, GL_TEXTURE_MIN_FILTER, m_Filter);
		glTexParameteri(m_Type, GL_TEXTURE_MAG_FILTER, m_Filter);
		glTexParameteri(m_Type, GL_TEXTURE_WRAP_S, m_Wrapping);
		glTexParameteri(m_Type, GL_TEXTURE_WRAP_T, m_Wrapping);
		glTexParameteri(m_Type, GL_TEXTURE_WRAP_R, m_Wrapping);

		//// specifics 
		if (whiteborder)
		{
			float borderColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
			glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
		}

	}
	virtual ~RenderTexture() { if (m_IsGenerated) glDeleteTextures(1, &m_ID); }
	unsigned int GetWidth() const { return m_Width; }
	unsigned int GetHeight() const { return m_Height; }
	unsigned int GetID() const { return m_ID; }

	bool operator==(const RenderTexture& other) const 
	{
		return m_ID == other.m_ID;
	}


	void Bind(unsigned int slot = 0) const {
		glActiveTexture(slot);
		glBindTexture(m_Type, m_ID);
	}

};


class DepthTexture2D : public RenderTexture 
{
public:
	DepthTexture2D(unsigned int width, unsigned int height, bool whiteborder = false)
		: RenderTexture (width, height, GL_TEXTURE_2D, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT, GL_NEAREST, GL_CLAMP_TO_BORDER, whiteborder) {}
	
};

class DepthTextureCubemap : public RenderTexture
{
public:
	DepthTextureCubemap(unsigned int width, unsigned int height, bool whiteborder = false)
		: RenderTexture(width, height, GL_TEXTURE_CUBE_MAP, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT, GL_NEAREST, GL_CLAMP_TO_BORDER, whiteborder) {}

};