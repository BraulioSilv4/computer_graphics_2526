////////////////////////////////////////////////////////////////////////////////
//
// Shader Naming Conventions
//
// Copyright (c)2022-25 by Carlos Martinho
//
////////////////////////////////////////////////////////////////////////////////

#ifndef MGL_CONVENTIONS_HPP
#define MGL_CONVENTIONS_HPP

namespace mgl {

////////////////////////////////////////////////////////////////////////////////



const char DIFFUSE_SAMPLER[] = "diffSampler";
const char NORMAL_SAMPLER[] = "normalSampler";
const char ROUGH_SAMPLER[] = "roughSampler";
const char METAL_SAMPLER[] = "metalSampler";
const char HEIGHT_SAMPLER[] = "heightSampler";
const char ARM_SAMPLER[] = "armSampler";
const char CUBEMAP_SAMPLER[] = "cubeMapSampler";
const char SCREEN_TEXTUERE_SAMPLER[] = "screenSampler";
const char EQUIRECTANGULAR_SAMPLER[] = "equirectangularSampler";
const char ENVIRONMENT_SAMPLER[] = "environmentSampler";
const char IRRADIANCE_SAMPLER[] = "irradianceSampler";
const char BRDDF_LUT_SAMPLER[] = "brdfLUTSampler";
const char PREFILTERED_ENV_SAMPLER[] = "prefilteredEnvSampler";

const char OBJECT_COLOR[] = "objectColor";
const char MODEL_MATRIX[] = "ModelMatrix";
const char NORMAL_MATRIX[] = "NormalMatrix";
const char VIEW_MATRIX[] = "ViewMatrix";
const char PROJECTION_MATRIX[] = "ProjectionMatrix";
const char TEXTURE_MATRIX[] = "TextureMatrix";
const char CAMERA_BLOCK[] = "Camera";
const char CAMERA_POSITION[] = "CameraPosition";
const char LIGHT_COLOR[] = "LightColor";
const char LIGHT_POSITION[] = "LightPosition";
const char CUBEMAP_RESOLUTION[] = "cubemapRes";
const char ROUGHNESS[] = "roughness";
const char GAMMA[] = "gamma";
const char EXPOSURE[] = "exposure";

const char POSITION_ATTRIBUTE[] = "inPosition";
const char NORMAL_ATTRIBUTE[] = "inNormal";
const char TEXCOORD_ATTRIBUTE[] = "inTexcoord";
const char TANGENT_ATTRIBUTE[] = "inTangent";
const char BITANGENT_ATTRIBUTE[] = "inBitangent";
const char COLOR_ATTRIBUTE[] = "inColor";

const char ENABLE_NORMAL_MAPPING[] = "enableNormalMapping";

constexpr unsigned int ENVIRONMENT_TEXTURE_UNIT = GL_TEXTURE0;
constexpr unsigned int ENVIRONMENT_UNIT_INDEX = 0;

constexpr unsigned int EQUIRECTANGULAR_TEXTURE_UNIT = GL_TEXTURE0;
constexpr unsigned int EQUIRECTANGULAR_UNIT_INDEX = 0;

constexpr unsigned int SCREEN_TEXTURE_UNIT = GL_TEXTURE0;
constexpr unsigned int SCREEN_TEXTURE_UNIT_INDEX = 0;

constexpr unsigned int ALBEDO_TEXTURE_UNIT = GL_TEXTURE0;
constexpr unsigned int ALBEDO_UNIT_INDEX = 0;

constexpr unsigned int ROUGHNESS_TEXTURE_UNIT = GL_TEXTURE1;
constexpr unsigned int ROUGHNESS_UNIT_INDEX = 1;

constexpr unsigned int METALLIC_TEXTURE_UNIT = GL_TEXTURE2;
constexpr unsigned int METALLIC_UNIT_INDEX = 2;

constexpr unsigned int NORMAL_TEXTURE_UNIT = GL_TEXTURE3;
constexpr unsigned int NORMAL_UNIT_INDEX = 3;

constexpr unsigned int HEIGHT_TEXTURE_UNIT = GL_TEXTURE4;
constexpr unsigned int HEIGHT_UNIT_INDEX = 4;

constexpr unsigned int CUBEMAP_TEXTURE_UNIT = GL_TEXTURE5;
constexpr unsigned int CUBEMAP_UNIT_INDEX = 5;

constexpr unsigned int IRRADIANCE_TEXTURE_UNIT = GL_TEXTURE6;
constexpr unsigned int IRRADIANCE_UNIT_INDEX = 6;

constexpr unsigned int BRDDF_LUT_TEXTURE_UNIT = GL_TEXTURE7;
constexpr unsigned int BRDDF_LUT_UNIT_INDEX = 7;

constexpr unsigned int PREFILTERED_ENV_TEXTURE_UNIT = GL_TEXTURE8;
constexpr unsigned int PREFILTERED_ENV_UNIT_INDEX = 8;

constexpr unsigned int ARM_TEXTURE_UNIT = GL_TEXTURE9;
constexpr unsigned int ARM_UNIT_INDEX = 9;

////////////////////////////////////////////////////////////////////////////////
} // namespace mgl

#endif /* MGL_CONVENTIONS_HPP */
