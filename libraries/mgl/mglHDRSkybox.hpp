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

	void equiToCubemap(
		unsigned int cubemapSize = 512, 
		unsigned int irradianceSize = 32, 
		unsigned int specBaseMip = 128
	);

	void bindIrradianceMap(GLenum textureUnit);
	void bindPrefilteredEnvMap(GLenum textureUnit);
	void bindBRDFLUTTexture(GLenum textureUnit);
	void initQuad();
	void drawQuad();

private:
	std::string filename;
	HDRITexture* hdriTexture = NULL;
	mgl::ShaderProgram* bakeShader = nullptr;				// Shader to convert equirectangular to cubemap
	mgl::ShaderProgram* convoluteShader = nullptr;			// Shader to convolute cubemap for irradiance map
	mgl::ShaderProgram* prefilterShader = nullptr;			// Shader to prefilter cubemap for specular IBL
	mgl::ShaderProgram* shaderProgram = nullptr;			// Shader to render the skybox
	mgl::ShaderProgram* brdfLUTShader = nullptr;			// Shader to generate the BRDF LUT
	mgl::Mesh* unitCube = nullptr;
	unsigned int envCubemap = 0;
	unsigned int irradianceMap = 0;
	unsigned int prefilterMap = 0;							// Prefiltered environment map for specular IBL 
	unsigned int brdfLUTTexture = 0;						// BRDF lookup texture

	unsigned int quadVAO = 0;
	unsigned int quadVBO = 0;

	void createBakeShaders();
};

#endif /* MGL_HDRSKYBOX_HPP */
