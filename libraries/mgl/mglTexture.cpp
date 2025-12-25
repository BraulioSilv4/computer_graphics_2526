#include "mglTexture.hpp"
#include <iostream>

#include <GL/glew.h>

#define STB_IMAGE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG
#include "stb_image.h"

bool Texture::load(bool isSRGB) {
	/* Image loading from file */
	stbi_set_flip_vertically_on_load(1);
	int width = 0, height = 0, bpp = 0;

	unsigned char* imageData = stbi_load(fileName.c_str(), &width, &height, &bpp, 0);
	if (!imageData) {
		std::cout << "Failed to load texture in: " << fileName.c_str() <<
			"\nreason is: " << stbi_failure_reason() << std::endl;
		return false;
	}

	/* Generating texture object */
	glGenTextures(1, &textureObject);
	glBindTexture(textureTarget, textureObject);

	GLenum internalFormat = GL_NONE;

	if (textureTarget == GL_TEXTURE_2D) {
		switch (bpp) {
		case 1: {
			/* Single channel for textures like Roughness, Metallic, Displacement */
			glTexImage2D(textureTarget, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, imageData);
			GLint mask[] = { GL_RED, GL_RED, GL_RED, GL_RED };
			glTexParameteriv(textureTarget, GL_TEXTURE_SWIZZLE_RGBA, mask);
			break;
		}

		case 3:
			/* 3 channels for Albedo and Normal maps */
			internalFormat = isSRGB ? GL_SRGB8 : GL_RGB8;
			glTexImage2D(textureTarget, 0, internalFormat, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData);
			break;

		default:
			std::cout << "Texture with n-channels not handled..." << std::endl;
			return false;

		}
	} else if(textureTarget == GL_TEXTURE_3D) {
		std::cout << "Cubemaps not implemented yet..";
		return false;
	}

	glTexParameterf(textureTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameterf(textureTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(textureTarget, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(textureTarget, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(textureTarget, GL_TEXTURE_BASE_LEVEL, 0);

	glGenerateMipmap(textureTarget);

	glBindTexture(textureTarget, 0);
	stbi_image_free(imageData);

	return true;
}

void Texture::bind(GLenum textureUnit) {
	glActiveTexture(textureUnit);
	glBindTexture(textureTarget, textureObject);
}
