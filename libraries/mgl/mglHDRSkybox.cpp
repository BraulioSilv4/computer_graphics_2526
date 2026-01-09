#include "mglHDRSkyBox.hpp"
#include <mglConventions.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <mglCamera.hpp>

HDRSkybox::HDRSkybox(const std::string& _filename, mgl::ShaderProgram* _shaderProgram) {
	filename = _filename;
	shaderProgram = _shaderProgram;
}

HDRSkybox::~HDRSkybox() {
	if (hdriTexture) {
		delete hdriTexture;
		hdriTexture = nullptr;
	}

	if (bakeShader) {
		delete bakeShader;
		bakeShader = nullptr;
	}

	if (envCubemap) {
		glDeleteTextures(1, &envCubemap);
		envCubemap = 0;
	}

	if (quadVAO) {
		glDeleteVertexArrays(1, &quadVAO);
		glDeleteBuffers(1, &quadVBO);
	}
}

void HDRSkybox::equiToCubemap(unsigned int cubemapSize, unsigned int irradianceSize, unsigned int specBaseMip) {
	if (!hdriTexture || !bakeShader) {
		std::cout << "HDRI texture or shader program not set." << std::endl;
		return;
	}

	// Store current viewport to restore later
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);

	// Store and set depth state for baking
	GLboolean depthMask;
	GLint depthFunc;
	glGetBooleanv(GL_DEPTH_WRITEMASK, &depthMask);
	glGetIntegerv(GL_DEPTH_FUNC, &depthFunc);

	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LESS);
	glEnable(GL_DEPTH_TEST);

	unsigned int captureFBO, captureRBO;
	glGenFramebuffers(1, &captureFBO);
	glGenRenderbuffers(1, &captureRBO);

	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);

	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, cubemapSize, cubemapSize);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

	glGenTextures(1, &envCubemap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);

	for (unsigned int i = 0; i < 6; i++) {
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F,
			cubemapSize, cubemapSize, 0, GL_RGB, GL_FLOAT, nullptr);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
	glm::mat4 captureViews[] = {
	   glm::lookAt(glm::vec3(0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
	   glm::lookAt(glm::vec3(0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
	   glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
	   glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
	   glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
	   glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
	};

	bakeShader->bind();

	glUniformMatrix4fv(bakeShader->Uniforms[mgl::PROJECTION_MATRIX].index, 1, GL_FALSE,
		glm::value_ptr(captureProjection));

	hdriTexture->bind(mgl::EQUIRECTANGULAR_TEXTURE_UNIT);
	glUniform1i(bakeShader->Uniforms[mgl::EQUIRECTANGULAR_SAMPLER].index, mgl::EQUIRECTANGULAR_UNIT_INDEX);

	glViewport(0, 0, cubemapSize, cubemapSize);
	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);

	// Disable face culling since the cube is inside out
	glDisable(GL_CULL_FACE);

	for (unsigned int i = 0; i < 6; i++) {
		glUniformMatrix4fv(bakeShader->Uniforms[mgl::VIEW_MATRIX].index, 1, GL_FALSE,
			glm::value_ptr(captureViews[i]));

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
			GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envCubemap, 0);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		unitCube->draw();
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	bakeShader->unbind();

	glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	/* Generate irradiance map by convolution */
	glGenTextures(1, &irradianceMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
	for (unsigned int i = 0; i < 6; i++) {
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F,
			irradianceSize, irradianceSize, 0, GL_RGB, GL_FLOAT, nullptr);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, irradianceSize, irradianceSize);


	convoluteShader->bind();

	glUniformMatrix4fv(convoluteShader->Uniforms[mgl::PROJECTION_MATRIX].index, 1, GL_FALSE,
		glm::value_ptr(captureProjection));

	/* Cubemap to texture unit */
	glActiveTexture(mgl::ENVIRONMENT_TEXTURE_UNIT);
	glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);

	glUniform1i(convoluteShader->Uniforms[mgl::ENVIRONMENT_SAMPLER].index, mgl::ENVIRONMENT_UNIT_INDEX);

	glViewport(0, 0, irradianceSize, irradianceSize);
	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	for (unsigned int i = 0; i < 6; i++) {
		glUniformMatrix4fv(convoluteShader->Uniforms[mgl::VIEW_MATRIX].index, 1, GL_FALSE,
			glm::value_ptr(captureViews[i]));

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
			GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradianceMap, 0);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		unitCube->draw();
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	convoluteShader->unbind();

	/* Prefiltered environment map */
	glGenTextures(1, &prefilterMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
	for (unsigned int i = 0; i < 6; i++) {
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F,
			specBaseMip, specBaseMip, 0, GL_RGB, GL_FLOAT, nullptr);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	prefilterShader->bind();

	glUniformMatrix4fv(prefilterShader->Uniforms[mgl::PROJECTION_MATRIX].index, 1, GL_FALSE,
		glm::value_ptr(captureProjection));

	glUniform1i(prefilterShader->Uniforms[mgl::ENVIRONMENT_SAMPLER].index, mgl::ENVIRONMENT_UNIT_INDEX);
	glActiveTexture(mgl::ENVIRONMENT_TEXTURE_UNIT);
	glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);

	glUniform1f(prefilterShader->Uniforms[mgl::CUBEMAP_RESOLUTION].index, static_cast<float>(cubemapSize));

	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	unsigned int maxMipLevels = 5;
	for (unsigned int mip = 0; mip < maxMipLevels; mip++) {
		unsigned int mipWidth = specBaseMip * std::pow(0.5, mip);
		unsigned int mipHeight = specBaseMip * std::pow(0.5, mip);

		glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
		glViewport(0, 0, mipWidth, mipHeight);

		float roughness = (float)mip / (float)(maxMipLevels - 1);
		glUniform1f(prefilterShader->Uniforms[mgl::ROUGHNESS].index, roughness);
		for (unsigned int i = 0; i < 6; i++) {
			glUniformMatrix4fv(prefilterShader->Uniforms[mgl::VIEW_MATRIX].index, 1, GL_FALSE,
				glm::value_ptr(captureViews[i]));

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				prefilterMap, mip);

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			unitCube->draw();
		}
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	prefilterShader->unbind();

	glGenTextures(1, &brdfLUTTexture);

	glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, 512, 512, 0, GL_RG, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brdfLUTTexture, 0);

	glViewport(0, 0, 512, 512);
	brdfLUTShader->bind();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	drawQuad();
	brdfLUTShader->unbind();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Restore previous state
	glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
	glDepthMask(depthMask);
	glDepthFunc(depthFunc);
	glEnable(GL_CULL_FACE);

	// Cleanup FBO resources
	glDeleteRenderbuffers(1, &captureRBO);
	glDeleteFramebuffers(1, &captureFBO);
}

void HDRSkybox::initQuad() {
	if (quadVAO != 0) return;

	float quadVertices[] = {
		// positions   // texCoords
		-1.0f,  1.0f,   0.0f, 1.0f,
		-1.0f, -1.0f,   0.0f, 0.0f,
		 1.0f, -1.0f,   1.0f, 0.0f,

		-1.0f,  1.0f,   0.0f, 1.0f,
		 1.0f, -1.0f,   1.0f, 0.0f,
		 1.0f,  1.0f,   1.0f, 1.0f
	};

	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);

	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

	glBindVertexArray(0);
}

void HDRSkybox::drawQuad() {
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

void HDRSkybox::createBakeShaders() {
	bakeShader = new mgl::ShaderProgram();
	bakeShader->addShader(GL_VERTEX_SHADER, "hdr_to_cubemap-vs.glsl");
	bakeShader->addShader(GL_FRAGMENT_SHADER, "hdr_to_cubemap-fs.glsl");

	bakeShader->addAttribute(mgl::POSITION_ATTRIBUTE, mgl::Mesh::POSITION);
	bakeShader->addUniform(mgl::EQUIRECTANGULAR_SAMPLER);
	bakeShader->addUniform(mgl::PROJECTION_MATRIX);
	bakeShader->addUniform(mgl::VIEW_MATRIX);
	bakeShader->create();

	convoluteShader = new mgl::ShaderProgram();
	convoluteShader->addShader(GL_VERTEX_SHADER, "convolution-vs.glsl");
	convoluteShader->addShader(GL_FRAGMENT_SHADER, "convolution-fs.glsl");

	convoluteShader->addAttribute(mgl::POSITION_ATTRIBUTE, mgl::Mesh::POSITION);
	convoluteShader->addUniform(mgl::ENVIRONMENT_SAMPLER);
	convoluteShader->addUniform(mgl::PROJECTION_MATRIX);
	convoluteShader->addUniform(mgl::VIEW_MATRIX);
	convoluteShader->create();

	prefilterShader = new mgl::ShaderProgram();
	prefilterShader->addShader(GL_VERTEX_SHADER, "prefilter_convolution-vs.glsl");
	prefilterShader->addShader(GL_FRAGMENT_SHADER, "prefilter_convolution-fs.glsl");

	prefilterShader->addAttribute(mgl::POSITION_ATTRIBUTE, mgl::Mesh::POSITION);
	prefilterShader->addUniform(mgl::ENVIRONMENT_SAMPLER);
	prefilterShader->addUniform(mgl::CUBEMAP_RESOLUTION);
	prefilterShader->addUniform(mgl::PROJECTION_MATRIX);
	prefilterShader->addUniform(mgl::VIEW_MATRIX);
	prefilterShader->addUniform(mgl::ROUGHNESS);
	prefilterShader->create();

	brdfLUTShader = new mgl::ShaderProgram();
	brdfLUTShader->addShader(GL_VERTEX_SHADER, "brdf_convolution-vs.glsl");
	brdfLUTShader->addShader(GL_FRAGMENT_SHADER, "brdf_convolution-fs.glsl");

	brdfLUTShader->addAttribute(mgl::POSITION_ATTRIBUTE, 0);
	brdfLUTShader->addAttribute(mgl::TEXCOORD_ATTRIBUTE, 1);
	brdfLUTShader->create();
}

bool HDRSkybox::init() {
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	if (filename.empty()) {
		std::cout << "No HDRI texture filename provided for HDRSkybox." << std::endl;
		return false;
	}

	if (!loadHDRITexture(filename)) {
		return false;
	}

	createBakeShaders();

	unitCube = new mgl::Mesh();
	unitCube->create("..\\assets\\models\\UnitCube\\unit_cube.obj");

	initQuad();

	equiToCubemap(512, 32, 128);

	return true;
}

void HDRSkybox::setShaderProgram(mgl::ShaderProgram* shader) {
	shaderProgram = shader;
}

mgl::ShaderProgram* HDRSkybox::getShaderProgram() {
	return shaderProgram;
}

HDRITexture* HDRSkybox::getHDRITexture() const {
	return hdriTexture;
}

void HDRSkybox::setHDRITexture(const std::string& _filename) {
	filename = _filename;
}

bool HDRSkybox::loadHDRITexture(const std::string& hdrFilename) {
	HDRITexture* _hdriTexture = new HDRITexture(hdrFilename);
	if (!_hdriTexture->load()) {
		std::cout << "Failed to load HDRI texture: " << hdrFilename << std::endl;
		delete _hdriTexture;
		return false;
	}

	std::cout << "Successfully loaded HDRI texture: " << hdrFilename << std::endl;
	hdriTexture = _hdriTexture;

	return true;
}

void HDRSkybox::bindIrradianceMap(GLenum textureUnit) {
	glActiveTexture(textureUnit);
	glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
}

void HDRSkybox::bindPrefilteredEnvMap(GLenum textureUnit) {
	glActiveTexture(textureUnit);
	glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
}

void HDRSkybox::bindBRDFLUTTexture(GLenum textureUnit) {
	glActiveTexture(textureUnit);
	glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);
}

void HDRSkybox::render(const mgl::Camera& camera) {
	if (!shaderProgram || !unitCube) {
		std::cout << "HDRSkybox not properly initialized for rendering." << std::endl;
		return;
	}

	GLint cullFaceMode, depthMode;
	glGetIntegerv(GL_CULL_FACE_MODE, &cullFaceMode);
	glGetIntegerv(GL_DEPTH_FUNC, &depthMode);

	glCullFace(GL_FRONT);
	glDepthFunc(GL_LEQUAL);

	glDisable(GL_CULL_FACE);

	shaderProgram->bind();

	glActiveTexture(mgl::CUBEMAP_TEXTURE_UNIT);
	glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);

	hdriTexture->bind(mgl::EQUIRECTANGULAR_TEXTURE_UNIT);

	glm::mat4 view = glm::mat4(glm::mat3(camera.getViewMatrix()));
	glm::mat4 projection = camera.getProjectionMatrix();

	glUniformMatrix4fv(
		shaderProgram->Uniforms[mgl::VIEW_MATRIX].index,
		1, GL_FALSE,
		glm::value_ptr(view)
	);

	glUniformMatrix4fv(
		shaderProgram->Uniforms[mgl::PROJECTION_MATRIX].index,
		1, GL_FALSE,
		glm::value_ptr(projection)
	);

	unitCube->draw();

	hdriTexture->unbind();

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	shaderProgram->unbind();

	glEnable(GL_CULL_FACE);

	glCullFace(cullFaceMode);
	glDepthFunc(depthMode);
}