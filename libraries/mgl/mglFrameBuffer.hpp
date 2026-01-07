#ifndef MGL_FRAMEBUFFER_HPP
#define MGL_FRAMEBUFFER_HPP

#include "mglShader.hpp"

class FrameBuffer {
public: 
	FrameBuffer(unsigned int _width, unsigned int _height);
	
	~FrameBuffer();

	void create();
	
	void bind();

	void unbind();

	void resize(unsigned int _width, unsigned int _height);

	void render();

	void setShaderProgram(mgl::ShaderProgram* shaderProgram);

	unsigned int getWidth() const { return width; }
	unsigned int getHeight() const { return height; }

	float getExposure() const { return exposure; }
	void setExposure(float _exposure) { exposure = _exposure; }
	float getGamma() const { return gamma; }
	void setGamma(float _gamma) { gamma = _gamma; }

private:
	unsigned int FBO;
	unsigned int RBO;
	unsigned int frameBufferTexture;
	unsigned int width, height;

	unsigned int quadVAO;
	unsigned int quadVBO;

	float exposure = 1.0f;
	float gamma = 2.2f;

	mgl::ShaderProgram* frameBufferShader;

	void createScreenQuad();
	void destroyScreenQuad();
	void createAttachments();
};

#endif // MGL_FRAMEBUFFER_HPP
