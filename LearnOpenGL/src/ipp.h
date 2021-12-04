#pragma once
#ifndef _IPP_H
#define _IPP_H
#include <glad\glad.h>

#include "Shader.h"
#include "Window.h"
#include "SimpleQuad.h"
#include "RenderText.h"
#include "Buffers.h"

// Image post-processing
class IPP {
public:
	IPP(Window* window = nullptr) : m_IsComplete (false), m_IsEnabled(false), m_BindedWindow(window){
		Setup();
	}
	~IPP() {
		delete m_TextureColorbuffer;
	}
private:

	SimpleQuad ppQuad;
	unsigned int m_Framebuffer;
	RenderTexture* m_TextureColorbuffer;
	Renderbuffer* m_RenderbufferObject;
	unsigned int m_Function;
	bool m_IsComplete;
	bool m_IsEnabled;
	Window * m_BindedWindow;
	std::shared_ptr<Shader> mBindedShader;

	void Setup() {

		// create a color attachment texture
		m_TextureColorbuffer = new RenderTexture(m_BindedWindow->GetWidth(), m_BindedWindow->GetHeight(), GL_TEXTURE_2D, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE);

		// Use Renderbuffer for Depth and Stencil buffer attachment
		// ---------------------------------------------------------
		m_RenderbufferObject = new Renderbuffer(m_BindedWindow->GetWidth(), m_BindedWindow->GetHeight(), GL_DEPTH24_STENCIL8);

		// framebuffer configuration
		// -------------------------
		glGenFramebuffers(1, &m_Framebuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, m_Framebuffer);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_TextureColorbuffer->GetID(), 0);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_RenderbufferObject->GetID()); 

		// now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
		m_IsComplete = true;
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
			m_IsComplete = false;
		}
		printf("%d\n", m_TextureColorbuffer->GetID());
		printf("%d\n", m_TextureColorbuffer->GetWidth());
		printf("%d\n", m_TextureColorbuffer->GetHeight());
		// Reset to defaults
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

public:

	void Enable() {
		if(m_IsComplete)
		{
			// bind to framebuffer and draw scene as we normally would to color texture 
			glBindFramebuffer(GL_FRAMEBUFFER, m_Framebuffer);
			glEnable(GL_DEPTH_TEST); // enable depth testing (is disabled for rendering screen-space quad)

			// make sure we clear the framebuffer's content
			m_BindedWindow->ClearScreen();
			m_IsEnabled = true;
		}
		else {
			std::cout << "Framebuffer is not complete. Cannot activate it for post-processing!" << std::endl;
			m_IsEnabled = false;
		}
	}

	void Disable() {
		// now bind back to default framebuffer and draw a quad plane with the attached framebuffer color texture
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDisable(GL_DEPTH_TEST); // disable depth test so screen-space quad isn't discarded due to depth test.
								  // clear all relevant buffers

								  // clear buffer info
		m_BindedWindow->ClearScreen();
		m_IsEnabled = false;
	}

	void Draw() {
		if (!m_IsEnabled && mBindedShader != nullptr)
		{
			// Save face-culling option
			bool previousFaceCullingState = false | glIsEnabled(GL_CULL_FACE);
			// Disable face-culling
			glDisable(GL_CULL_FACE);
			// Draw
			mBindedShader->Use();

			glActiveTexture(0);
			glBindTexture(GL_TEXTURE_2D, m_TextureColorbuffer->GetID());	// use the color attachment texture as the texture of the quad plane
			mBindedShader->SetInt("screenTexture", 0);

			ppQuad.Draw(mBindedShader);
			// Reset face culling to previous state
			if (previousFaceCullingState) glEnable(GL_CULL_FACE);
		}
		// error checking
		else {
			if (m_IsEnabled)
			{
				std::cout << "Post-processing framebuffer is enabled! Disable before drawing." << std::endl;
			}
			if (mBindedShader == nullptr)
			{
				std::cout << "Shader was not bound! Cannot draw in post-processing" << std::endl;
			}
		}
	}

	void setShaderFunction(int function) {
		if (mBindedShader != nullptr)
		{
			m_Function = function;
			mBindedShader->SetInt("function", function);
		}
		// error checking
		else {
			std::cout << "Shader was not bound! Cannot set post-processing shader function!" << std::endl;
		}
	}

	void bindShader(std::shared_ptr<Shader> shader) { mBindedShader = shader; }
	void bindWindow(Window * window) { m_BindedWindow = window; }

};
#endif // !_IPP_H
