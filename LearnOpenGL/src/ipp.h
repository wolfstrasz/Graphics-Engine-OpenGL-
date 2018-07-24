#pragma once
#ifndef _IPP_H
#define _IPP_H
#include "shader.h"
#include "window.h"

// Image post-processing
class IPP {
public:
	IPP(Window* window = nullptr) : mComplete (false), mEnabled(false){
		mBindedWindow = window;
		setupQuad();
		setup();
	}
private:
	// vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
	float mQuadVertices[24] = { 
		// positions   // texCoords
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		-1.0f,  1.0f,  0.0f, 1.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		 1.0f,  1.0f,  1.0f, 1.0f
	};
	unsigned int mQuadVAO, mQuadVBO;
	unsigned int mFramebuffer;
	unsigned int mTextureColorbuffer;
	unsigned int mRenderbufferObject;
	float mComplete;
	float mEnabled;
	int mFunction;
	Window * mBindedWindow;
	Shader * mBindedShader;
	// Functions
	void setupQuad() {
		glGenVertexArrays(1, &mQuadVAO);
		glGenBuffers(1, &mQuadVBO);
		glBindVertexArray(mQuadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, mQuadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(mQuadVertices), &mQuadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(0 * sizeof(float)));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
		// Reset to defaults
		glBindVertexArray(0);
	}
	void setup() {
		// framebuffer configuration
		// -------------------------
		glGenFramebuffers(1, &mFramebuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, mFramebuffer);
		// create a color attachment texture
		glGenTextures(1, &mTextureColorbuffer);
		glBindTexture(GL_TEXTURE_2D, mTextureColorbuffer);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, mBindedWindow->getWidth(), mBindedWindow->getHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mTextureColorbuffer, 0);
		// create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
		glGenRenderbuffers(1, &mRenderbufferObject);
		glBindRenderbuffer(GL_RENDERBUFFER, mRenderbufferObject);
		// use a single renderbuffer object for both a depth AND stencil buffer.
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, mBindedWindow->getWidth(), mBindedWindow->getHeight()); 
		// now actually attach it
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, mRenderbufferObject); 
		// now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
		mComplete = true;
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
			mComplete = false;
		}
		// Reset to defaults
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
public:
	void enable() {
		if(mComplete)
		{
		// bind to framebuffer and draw scene as we normally would to color texture 
		glBindFramebuffer(GL_FRAMEBUFFER, mFramebuffer);
		glEnable(GL_DEPTH_TEST); // enable depth testing (is disabled for rendering screen-space quad)

								 // make sure we clear the framebuffer's content
		mBindedWindow->clearScreen();
		mEnabled = true;
		}
		else {
			std::cout << "Framebuffer is not complete. Cannot activate it for post-processing!" << std::endl;
			mEnabled = false;
		}
	}
	void disable() {
		// now bind back to default framebuffer and draw a quad plane with the attached framebuffer color texture
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDisable(GL_DEPTH_TEST); // disable depth test so screen-space quad isn't discarded due to depth test.
								  // clear all relevant buffers

								  // clear buffer info
		mBindedWindow->clearScreen();
		mEnabled = false;
	}
	void draw() {
		if (!mEnabled && mBindedShader != nullptr)
		{
			mBindedShader->use();
			glBindVertexArray(mQuadVAO);
			glBindTexture(GL_TEXTURE_2D, mTextureColorbuffer);	// use the color attachment texture as the texture of the quad plane
			mBindedShader->setInt("screenTexture", 0);
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}
		// error checking
		else {
			if (mEnabled)
				std::cout << "Post-processing framebuffer is enabled! Disable before drawing." << std::endl;
			if (mBindedShader == nullptr)
				std::cout << "Shader was not bound! Cannot draw in post-processing" << std::endl;
		}
	}
	void setShaderFunction(int function) {
		if (mBindedShader != nullptr)
		{
			mFunction = function;
			mBindedShader->setInt("function", function);
		}
		// error checking
		else {
			std::cout << "Shader was not bound! Cannot set post-processing shader function!" << std::endl;
		}
	}
	void bindShader(Shader * shader) { mBindedShader = shader; }
	void bindWindow(Window * window) { mBindedWindow = window; }

};
#endif // !_IPP_H
