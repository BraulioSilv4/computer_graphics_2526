#include "mglCubeMapTexture.hpp"
#include <stb_image.h>
#include <iostream>

static const int typesSize = 6;

static const GLenum types[6] = {
	GL_TEXTURE_CUBE_MAP_POSITIVE_X,
	GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
	GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
	GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
	GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
	GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
};


CubeMapTexture::CubeMapTexture(
	const std::string& Directory,
	const std::string& PosXFilename,
	const std::string& NegXFilename,
	const std::string& PosYFilename,
	const std::string& NegYFilename,
	const std::string& PosZFilename,
	const std::string& NegZFilename
) {
	fileNames[0] = Directory + PosXFilename;
	fileNames[1] = Directory + NegXFilename;
	fileNames[2] = Directory + PosYFilename;
	fileNames[3] = Directory + NegYFilename;
	fileNames[4] = Directory + NegZFilename;
	fileNames[5] = Directory + PosZFilename;

	textureObject = 0;
}

bool CubeMapTexture::load() {
	/* Generate Handle for the cubemap texture */
	glGenTextures(1, &textureObject);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureObject);

	for (int i = 0; i < typesSize; i++) {
		stbi_set_flip_vertically_on_load(1);
		int width, height, bpp;
		void* pData = NULL; 

		unsigned char* image_data = stbi_load(fileNames[i].c_str(), &width, &height, &bpp, 0);

		if (!image_data) {
			std::cout << "Couldn't load cubemap texture " << fileNames[i].c_str() << "\nReason: " << stbi_failure_reason() << std::endl;
			exit(0);
		}

		std::cout << "Loaded Cubemap texture " << fileNames[i].c_str() << std::endl;

		pData = image_data;

		glTexImage2D(types[i], 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, pData);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		stbi_image_free(image_data);
	}

	return true;
}

void CubeMapTexture::bind(GLenum TextureUnit) {
	glActiveTexture(TextureUnit);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureObject);
}
