#include "mglHDRITexture.hpp"

#include "stb_image.h"

HDRITexture::HDRITexture(const std::string& _filename) {
	filename = _filename;
}

HDRITexture::~HDRITexture() {
	glDeleteTextures(1, &textureObject);
}

bool HDRITexture::load() {
	glGenTextures(1, &textureObject);
	glBindTexture(GL_TEXTURE_2D, textureObject);

	stbi_set_flip_vertically_on_load(true);
	int width, height, bpp;
	float* data = stbi_loadf(filename.c_str(), &width, &height, &bpp, 0);
	
	if(!data) {
		std::cout << "Error loading HDR image: " << filename << std::endl;
		exit(0);
	}

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	stbi_image_free(data);

	glBindTexture(GL_TEXTURE_2D, 0);

	return true;
}

void HDRITexture::bind(GLenum textureUnit) {
	glActiveTexture(textureUnit);
	glBindTexture(GL_TEXTURE_2D, textureObject);
}

void HDRITexture::unbind() {
	glBindTexture(GL_TEXTURE_2D, 0);
}