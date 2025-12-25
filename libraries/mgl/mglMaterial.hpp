#ifndef MGL_MATERIAL_HPP
#define MGL_MATERIAL_HPP

#include "mglTexture.hpp"
#include <glm/vec3.hpp> 


class Material {
public:
	struct Properties {
		float metallic = 0.0f; // Pm
		float roughness = 0.0f; // Pr
		float sheen = 0.0f; // Ps
		float clearcoat = 0.0f; // Pc
		float clearcoatRoughness = 0.0f; // Pcr
	};

	struct Textures {
		Texture* texAlbedo = NULL;
		Texture* texRoughness = NULL;
		Texture* texMetallic = NULL;
		Texture* texNormalMap = NULL;
	};

	std::string name;

	glm::vec3 color = glm::vec3(0.0f, 0.0f, 0.0f);
	Properties matProps;
	Textures matTex;
};

#endif // !MGL_MATERIAL_HPP
