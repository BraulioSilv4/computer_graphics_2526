#ifndef MGL_HDR_HPP
#define MGL_HDR_HPP

#include <iostream>
#include <GL/glew.h>

class HDRITexture {
public:
	HDRITexture(const std::string& _filename);
	
	~HDRITexture();

	bool load();

	void bind(GLenum textureUnit);

	void unbind();

private:
	std::string filename;
	GLuint textureObject;
};

#endif /* MGL_HDR_HPP */