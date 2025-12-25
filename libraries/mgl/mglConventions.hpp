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

const char OBJECT_COLOR[] = "objectColor";
const char MODEL_MATRIX[] = "ModelMatrix";
const char NORMAL_MATRIX[] = "NormalMatrix";
const char VIEW_MATRIX[] = "ViewMatrix";
const char PROJECTION_MATRIX[] = "ProjectionMatrix";
const char TEXTURE_MATRIX[] = "TextureMatrix";
const char CAMERA_BLOCK[] = "Camera";

const char POSITION_ATTRIBUTE[] = "inPosition";
const char NORMAL_ATTRIBUTE[] = "inNormal";
const char TEXCOORD_ATTRIBUTE[] = "inTexcoord";
const char TANGENT_ATTRIBUTE[] = "inTangent";
const char BITANGENT_ATTRIBUTE[] = "inBitangent";
const char COLOR_ATTRIBUTE[] = "inColor";

constexpr unsigned int ALBEDO_TEXTURE_UNIT = GL_TEXTURE0;
constexpr unsigned int ALBEDO_UNIT_INDEX = 0;

constexpr unsigned int ROUGHNESS_TEXTURE_UNIT = GL_TEXTURE1;
constexpr unsigned int ROUGHNESS_UNIT_INDEX = 1;

constexpr unsigned int METALLIC_TEXTURE_UNIT = GL_TEXTURE2;
constexpr unsigned int METALLIC_UNIT_INDEX = 2;

constexpr unsigned int NORMAL_TEXTURE_UNIT = GL_TEXTURE3;
constexpr unsigned int NORMAL_UNIT_INDEX = 3;

////////////////////////////////////////////////////////////////////////////////
} // namespace mgl

#endif /* MGL_CONVENTIONS_HPP */
