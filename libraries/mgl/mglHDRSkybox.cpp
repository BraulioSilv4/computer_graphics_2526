#include "mglHDRSkyBox.hpp"
#include <mglConventions.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <mglCamera.hpp>








void glCheckError_(const char* file, int line) {
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR) {
		std::string error;
		switch (errorCode) {
		case GL_INVALID_ENUM:      error = "INVALID_ENUM"; break;
		case GL_INVALID_VALUE:     error = "INVALID_VALUE"; break;
		case GL_INVALID_OPERATION: error = "INVALID_OPERATION"; break;
		case GL_STACK_OVERFLOW:    error = "STACK_OVERFLOW"; break;
		case GL_STACK_UNDERFLOW:   error = "STACK_UNDERFLOW"; break;
		case GL_OUT_OF_MEMORY:     error = "OUT_OF_MEMORY"; break;
		case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
		}
		std::cout << error << " | " << file << " (" << line << ")" << std::endl;
	}
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)

















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

	// sceneNode is unique_ptr, automatically cleaned up
}

void HDRSkybox::equiToCubemap(unsigned int cubemapSize) {
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
	glCheckError();

	glGenRenderbuffers(1, &captureRBO);
	glCheckError();

	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glCheckError();

	glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
	glCheckError();

	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, cubemapSize, cubemapSize);
	glCheckError();

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);
	glCheckError();

	// Check framebuffer completeness
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "ERROR: Capture framebuffer not complete! Status: " << status << std::endl;
		return;
	}

	glGenTextures(1, &envCubemap);
	if (envCubemap == 0) {
		std::cout << "ERROR: Failed to generate cubemap texture!" << std::endl;
		return;
	}

	glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
	glCheckError();

	for (unsigned int i = 0; i < 6; i++) {
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F,
			cubemapSize, cubemapSize, 0, GL_RGB, GL_FLOAT, nullptr);
		glCheckError();
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glCheckError();

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glCheckError();

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glCheckError();

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glCheckError();

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glCheckError();

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

	// Disable face culling during baking
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
	glCheckError();

	bakeShader->unbind();
	glCheckError();

	// Restore previous state
	glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
	glDepthMask(depthMask);
	glDepthFunc(depthFunc);
	glEnable(GL_CULL_FACE);
	glCheckError();

	// Cleanup FBO resources
	glDeleteRenderbuffers(1, &captureRBO);
	glDeleteFramebuffers(1, &captureFBO);
}

void HDRSkybox::createBakeShader() {
	bakeShader = new mgl::ShaderProgram();
	bakeShader->addShader(GL_VERTEX_SHADER, "hdr_to_cubemap-vs.glsl");
	bakeShader->addShader(GL_FRAGMENT_SHADER, "hdr_to_cubemap-fs.glsl");

	bakeShader->addAttribute(mgl::POSITION_ATTRIBUTE, mgl::Mesh::POSITION);
	bakeShader->addUniform(mgl::EQUIRECTANGULAR_SAMPLER);
	bakeShader->addUniform(mgl::PROJECTION_MATRIX);
	bakeShader->addUniform(mgl::VIEW_MATRIX);
	bakeShader->addUniform(mgl::MODEL_MATRIX);
	bakeShader->create();
}

bool HDRSkybox::init() {
	if (filename.empty()) {
		std::cout << "No HDRI texture filename provided for HDRSkybox." << std::endl;
		return false;
	}

	if (!loadHDRITexture(filename)) {
		return false;
	}
	
	createBakeShader();
	glCheckError();

	unitCube = new mgl::Mesh();
	unitCube->create("..\\assets\\models\\UnitCube\\unit_cube.obj");

	sceneNode = std::make_unique<mgl::SceneNode>(
		"hdr_skybox_cube",
		unitCube,
		bakeShader
	);

	mgl::Mesh* skyboxMesh = new mgl::Mesh();
	skyboxMesh->create("..\\assets\\models\\UnitCube\\unit_cube.obj");

	skyboxNode = std::make_unique<mgl::SceneNode>(
		"hdr_skybox",
		skyboxMesh,
		shaderProgram
	);

	equiToCubemap(4096);


	return true;
}

void HDRSkybox::setShaderProgram(mgl::ShaderProgram* shader) {
	shaderProgram = shader;
	if(sceneNode) {
		sceneNode->setShaderProgram(shader);
	}
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

void HDRSkybox::render(const mgl::Camera& camera) {
	if (!shaderProgram || !envCubemap || !sceneNode) {
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

	glUniform1f(shaderProgram->Uniforms[mgl::GAMMA].index, 2.2);
	glUniform1i(shaderProgram->Uniforms[mgl::CUBEMAP_SAMPLER].index, mgl::CUBEMAP_UNIT_INDEX);
	glActiveTexture(mgl::CUBEMAP_TEXTURE_UNIT);
	glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);

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

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	shaderProgram->unbind();

	glEnable(GL_CULL_FACE);

	glCullFace(cullFaceMode);
	glDepthFunc(depthMode);
}