#ifndef MGL_SKYBOX_HPP
#define MGL_SKYBOX_HPP

#include <string>
#include <mglOrbitCamera.hpp>
#include "mglCubeMapTexture.hpp"
#include "mglMesh.hpp"
#include "mglShader.hpp"

class SkyBox {
public:
	SkyBox();

	~SkyBox();

	bool init(
		const std::string& directory,
		const std::string& posXFilename,
		const std::string& negXFilename,
		const std::string& posYFilename,
		const std::string& negYFilename,
		const std::string& posZFilename,
		const std::string& negZFilename
	);

	void setShaderProgram(mgl::ShaderProgram* shader);
	mgl::ShaderProgram* getShaderProgram();	

	CubeMapTexture* getCubeMapTexture() const;
	void setCubeMapTexture(CubeMapTexture* texture);

	void render(const mgl::Camera& camera);

private:
	mgl::ShaderProgram* shaderProgram = nullptr;
	CubeMapTexture* cubeMapTexture = NULL;
	mgl::Mesh* mesh = NULL;
};

#endif /* MGL_SKYBOX_HPP */