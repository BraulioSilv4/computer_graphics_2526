#include "mglTexture.hpp"
#include <iostream>

#include <GL/glew.h>

#define STB_IMAGE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG
#include "stb_image.h"

bool Texture::load() {
	/* Image loading from file */
	stbi_set_flip_vertically_on_load(1);
	int width = 0, height = 0, bpp = 0;

	unsigned char* image_data = stbi_load(fileName.c_str(), &width, &height, &bpp, 0);
	if (!image_data) {
		std::cout << "Failed to load texture in: " << fileName.c_str() <<
			"\nreason is: " << stbi_failure_reason() << std::endl;
		exit(0);
	}

	/* Generating texture object */
	glGenTextures(1, &textureObject);
	glBindTexture(textureTarget, textureObject);
	if (textureTarget == GL_TEXTURE_2D) {
		glTexImage2D(textureTarget, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image_data);
	}
	else {
		std::cout << "Not 2D texture" << std::endl;
		exit(0);
	}

	glTexParameterf(textureTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameterf(textureTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(textureTarget, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(textureTarget, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glGenerateMipmap(textureTarget);

	glBindTexture(textureTarget, 0);
	stbi_image_free(image_data);

	return true;
}

void Texture::bind(GLenum textureUnit) {
	glActiveTexture(textureUnit);
	glBindTexture(textureTarget, textureObject);
}
