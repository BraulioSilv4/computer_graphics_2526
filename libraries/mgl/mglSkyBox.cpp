#include "mglSkyBox.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <mglConventions.hpp>
#include "mglCubeMapTexture.hpp"

SkyBox::SkyBox() {
	cubeMapTexture = NULL;
	mesh = NULL;
}

bool SkyBox::init(
	const std::string& directory,
	const std::string& posXFilename,
	const std::string& negXFilename,
	const std::string& posYFilename,
	const std::string& negYFilename,
	const std::string& posZFilename,
	const std::string& negZFilename
) {
	cubeMapTexture = new CubeMapTexture(
		directory,
		posXFilename,
		negXFilename,
		posYFilename,
		negYFilename,
		posZFilename,
		negZFilename
	);
	if (!cubeMapTexture->load()) {
		std::cout << "Failed to load skybox cubemap texture." << std::endl;
		return false;
	}
	mesh = new mgl::Mesh();
	mesh->create("..\\assets\\skybox\\box.obj");
	return true;
}

void SkyBox::setShaderProgram(mgl::ShaderProgram* shader) {
	shaderProgram = shader;
}

mgl::ShaderProgram* SkyBox::getShaderProgram() {
	return shaderProgram;
}

CubeMapTexture* SkyBox::getCubeMapTexture() const {
	return cubeMapTexture;
}

void SkyBox::setCubeMapTexture(CubeMapTexture* texture) {
	cubeMapTexture = texture;
}

void SkyBox::render(const mgl::Camera& camera) {
	GLint cullFaceMode;
	glGetIntegerv(GL_CULL_FACE_MODE, &cullFaceMode);
	GLint depthMode;
	glGetIntegerv(GL_DEPTH_FUNC, &depthMode);

	glCullFace(GL_FRONT);
	glDepthFunc(GL_LEQUAL);

	shaderProgram->bind();
	
	glm::mat4 view = glm::mat4(glm::mat3(camera.getViewMatrix()));
	glm::mat4 projection = camera.getProjectionMatrix();

	glUniformMatrix4fv(
		shaderProgram->Uniforms[mgl::VIEW_MATRIX].index,
		1,
		GL_FALSE,
		glm::value_ptr(view)
	);

	glUniformMatrix4fv(
		shaderProgram->Uniforms[mgl::PROJECTION_MATRIX].index,
		1,
		GL_FALSE,
		glm::value_ptr(projection)
	);

	cubeMapTexture->bind(mgl::CUBEMAP_TEXTURE_UNIT);
	mesh->draw();

	shaderProgram->unbind();
	glCullFace(cullFaceMode);
	glDepthFunc(depthMode);
}