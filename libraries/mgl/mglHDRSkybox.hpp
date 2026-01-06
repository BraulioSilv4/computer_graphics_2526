#ifndef MGL_HDRSKYBOX_HPP
#define MGL_HDRSKYBOX_HPP

#include <string>
#include <mglHDRITexture.hpp>
#include <mglShader.hpp>
#include <mglMesh.hpp>
#include <mglSceneNode.hpp>
#include <mglCamera.hpp>

class HDRSkybox {
public:
	HDRSkybox(const std::string& _filename = "", mgl::ShaderProgram* _shaderProgram = nullptr);

	~HDRSkybox();
	
	bool init();

	void setShaderProgram(mgl::ShaderProgram* shader);
	mgl::ShaderProgram* getShaderProgram();
	
	HDRITexture* getHDRITexture() const;
	void setHDRITexture(const std::string& _filename);
	bool loadHDRITexture(const std::string& hdrFilename);
	
	void render(const mgl::Camera& camera);

	void equiToCubemap(unsigned int cubemapSize = 512);

private:
	std::string filename;
	HDRITexture* hdriTexture = NULL;
	mgl::ShaderProgram* bakeShader = nullptr;				// Shader to convert equirectangular to cubemap
	mgl::ShaderProgram* shaderProgram = nullptr;			// Shader to render the skybox
	mgl::Mesh* unitCube = nullptr;
	std::unique_ptr<mgl::SceneNode> sceneNode = nullptr;
	std::unique_ptr<mgl::SceneNode> skyboxNode = nullptr;
	unsigned int envCubemap = 0;

	void createBakeShader();
};

#endif /* MGL_HDRSKYBOX_HPP */
