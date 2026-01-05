#ifndef MGL_CUBEMAP_TEXTURE_HPP
#define MGL_CUBEMAP_TEXTURE_HPP

#include <string>
#include <GL/glew.h>

class CubeMapTexture {
public:
	CubeMapTexture(
		const std::string& Directory,
		const std::string& PosXFilename,
		const std::string& NegXFilename,
		const std::string& PosYFilename,
		const std::string& NegYFilename,
		const std::string& PosZFilename,
		const std::string& NegZFilename
	);

	~CubeMapTexture();

	bool load();

	void bind(GLenum TextureUnit);

private:
	std::string fileNames[6];
	GLuint textureObject;
};

#endif /* MGL_CUBEMAP_TEXTURE_HPP */