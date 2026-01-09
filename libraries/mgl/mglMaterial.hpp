#ifndef MGL_MATERIAL_HPP
#define MGL_MATERIAL_HPP

#include "mglTexture.hpp"
#include <glm/vec3.hpp> 


class Material {
public:
	struct Properties {
		glm::vec4* baseColor = new glm::vec4(1.0, 1.0, 1.0, 1.0);
		bool hasBaseColor = false; 
		bool isOpaque = true;
		bool isDoubleSided = false;
		float metallic = 1.0f; // Pm
		float roughness = 1.0f; // Pr
	};

	struct Textures {
		Texture* texAlbedo = NULL;
		Texture* texRoughness = NULL;
		Texture* texMetallic = NULL;
		Texture* texNormalMap = NULL;
		Texture* texARM = NULL;
	};

	std::string name;

	GLuint baseColorTexture;
	glm::vec3 color = glm::vec3(0.0f, 0.0f, 0.0f);
	Properties matProps;
	Textures matTex;
};

#endif // !MGL_MATERIAL_HPP
