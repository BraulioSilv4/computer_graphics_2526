#ifndef MGL_TEXTURE_HPP
#define MGL_TEXTURE_HPP

#include <GL/glew.h>
#include <string>
	
class Texture {
public:
	Texture(GLenum _textureTarget, const std::string& _fileName) {
		fileName = _fileName;
		textureTarget = _textureTarget;
	}

	/* Loads the texture from file */
	bool load();

	/* Bind texture to activate it */
	void bind(GLenum textureUnit);

private:
	std::string fileName;
	GLenum textureTarget;
	GLuint textureObject;
};

#endif