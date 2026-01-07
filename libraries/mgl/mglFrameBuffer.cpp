#include "mglFrameBuffer.hpp"
#include <GL/glew.h>
#include <iostream>
#include <mglMesh.hpp>
#include <mglConventions.hpp>


FrameBuffer::FrameBuffer(unsigned int _width, unsigned int _height) {
	FBO = 0;
	RBO = 0;
	quadVAO = 0;
	quadVBO = 0;
	frameBufferTexture = 0;
	width = _width;
	height = _height;
	frameBufferShader = nullptr;
}

FrameBuffer::~FrameBuffer() {
	if (frameBufferTexture != 0) {
		glDeleteTextures(1, &frameBufferTexture);
	}
	if (RBO != 0) {
		glDeleteRenderbuffers(1, &RBO);
	}
	if (FBO != 0) {
		glDeleteFramebuffers(1, &FBO);
	}
	destroyScreenQuad();
}

void FrameBuffer::create() {
	/* Buffer for FrameBuffer */
	glGenFramebuffers(1, &FBO);

	/* Binding FrameBuffer */
	createAttachments();

	/* Create screen quad to render the framebuffer texture */
	createScreenQuad();
}


void FrameBuffer::resize(unsigned int _width, unsigned int _height) {
	if (width != _width || height != _height) {
		width = _width;
		height = _height;

		if (frameBufferTexture != 0) {
			glDeleteTextures(1, &frameBufferTexture);
			frameBufferTexture = 0;
		}

		if (RBO != 0) {
			glDeleteRenderbuffers(1, &RBO);
			RBO = 0;
		}

		if (FBO != 0) {
			glDeleteFramebuffers(1, &FBO);
			FBO = 0;
		}

		glGenFramebuffers(1, &FBO);

		createAttachments();
	}
}


void FrameBuffer::createAttachments() {
	// Bind the framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	// Create color texture attachment
	glGenTextures(1, &frameBufferTexture);
	glBindTexture(GL_TEXTURE_2D, frameBufferTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, frameBufferTexture, 0);

	// Create renderbuffer for depth and stencil
	glGenRenderbuffers(1, &RBO);
	glBindRenderbuffer(GL_RENDERBUFFER, RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);

	// Check framebuffer completeness
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "ERROR: Framebuffer is not complete!" << std::endl;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void FrameBuffer::createScreenQuad() {
	float quadVertices[] = {
		// positions   // texCoords
		1.0f, -1.0f,   1.0f, 0.0f,
	   -1.0f, -1.0f,   0.0f, 0.0f,
	   -1.0f,  1.0f,   0.0f, 1.0f,

	    1.0f,  1.0f,   1.0f, 1.0f,
	    1.0f, -1.0f,   1.0f, 0.0f,
	   -1.0f,  1.0f,   0.0f, 1.0f 
	};

	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);

	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

	glBindVertexArray(0);
}


void FrameBuffer::render() {
	if (frameBufferShader == nullptr) {
		std::cout << "No Shader Program set for FrameBuffer rendering..." << std::endl;
		exit(0);
	}

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	frameBufferShader->bind();
	glUniform1f(frameBufferShader->Uniforms[mgl::GAMMA].index, gamma);
	glUniform1f(frameBufferShader->Uniforms[mgl::EXPOSURE].index, exposure);
	
	glBindVertexArray(quadVAO);
	
	glActiveTexture(mgl::SCREEN_TEXTURE_UNIT);
	glBindTexture(GL_TEXTURE_2D, frameBufferTexture);
	
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
	
	frameBufferShader->unbind();

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
}

void FrameBuffer::destroyScreenQuad() {
	if (quadVAO != 0) {
		glDeleteVertexArrays(1, &quadVAO);
		quadVAO = 0;
	}
	if (quadVBO != 0) {
		glDeleteBuffers(1, &quadVBO);
		quadVBO = 0;
	}
}


void FrameBuffer::setShaderProgram(mgl::ShaderProgram* shaderProgram) {
	frameBufferShader = shaderProgram;
}

void FrameBuffer::bind() {
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
}

void FrameBuffer::unbind() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
