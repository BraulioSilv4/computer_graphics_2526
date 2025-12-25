////////////////////////////////////////////////////////////////////////////////
//
// Mesh Loader Class
//
// Copyright (c)2022-25 by Carlos Martinho
//
////////////////////////////////////////////////////////////////////////////////

#ifndef MGL_MESH_HPP
#define MGL_MESH_HPP

#include <GL/glew.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>

#include "mglDrawable.hpp"
#include "mglInstanceManager.hpp"
#include "mglTexture.hpp"
#include "mglMaterial.hpp"

namespace mgl {

class Mesh;

#define CREATE_BITANGENT

/////////////////////////////////////////////////////////////////////////// Mesh

class Mesh : public IDrawable, public IManageable<std::string> {
public:
  static const GLuint INDEX = 0;
  static const GLuint POSITION = 1;
  static const GLuint NORMAL = 2;
  static const GLuint TEXCOORD = 3;
  static const GLuint TANGENT = 4;
#ifdef CREATE_BITANGENT
  static const GLuint BITANGENT = 5;
#endif

  Mesh();
  ~Mesh();
  // No copy and assignment constructor to prevent copying OpenGL resources
  //Mesh(const Mesh &&) = delete;
  //Mesh &operator=(const Mesh &&) = delete;
  //// Move constructor and assignment to allow transfer of OpenGL resources
  //Mesh(Mesh &) noexcept;
  //Mesh &operator=(Mesh &) noexcept;

  void setAssimpFlags(unsigned int flags);
  void joinIdenticalVertices();
  void generateNormals();
  void generateSmoothNormals();
  void generateTexcoords();
  void calculateTangentSpace();
  void flipUVs();

  void create(const std::string &filename);
  void draw() override;

  bool hasNormals();
  bool hasTexcoords();
  bool hasTangentsAndBitangents();
  std::string getID() const override;

private:
  std::string id;
  GLuint VaoId;
  unsigned int AssimpFlags;
  bool NormalsLoaded, TexcoordsLoaded, TangentsAndBitangentsLoaded;

  struct MeshData {
    unsigned int nIndices = 0;
    unsigned int baseIndex = 0;
    unsigned int baseVertex = 0;
    unsigned int materialIndex = 0;
  };

  std::vector<MeshData> Meshes;
  std::vector<Material> Materials;
  std::vector<Texture*> Textures;

  std::vector<glm::vec3> Positions;
  std::vector<glm::vec3> Normals;
  std::vector<glm::vec2> Texcoords;
  std::vector<glm::vec3> Tangents;
#ifdef CREATE_BITANGENT
  std::vector<glm::vec3> Bitangents;
#endif
  std::vector<unsigned int> Indices;

  void clear();
  
  void processScene(const aiScene *scene);
  void processMesh(const aiMesh *mesh);

  void loadMaterials(const aiScene *scene, const std::string& filename);
  
  void loadTextures(const std::string& directory, const aiMaterial* material, int materialIdx);

  /* PBR texture loading */
  void loadAlbedoTex(const std::string& directory, const aiMaterial* material, int materialIdx);
  void loadAlbedoTexFromFile(const std::string& directory, const aiString& path, int materialIdx);
  
  void loadRoughnessTex(const std::string& directory, const aiMaterial* material, int materialIdx);
  void loadRoughnessTexFromFile(const std::string& directory, const aiString& path, int materialIdx);
  
  void loadMetallicTex(const std::string& directory, const aiMaterial* material, int materialIdx);
  void loadMetallicTexFromFile(const std::string& directory, const aiString& path, int materialIdx);
  
  void loadNormalMapTex(const std::string& directory, const aiMaterial* material, int materialIdx);
  void loadNormalMapTexFromFile(const std::string& directory, const aiString& path, int materialIdx);

  void loadMaterialParameters(const aiMaterial* material, int materialIdx);

  void bindMaterialsTextures(int materialIdx);

  void createBufferObjects();
  void destroyBufferObjects();
};

////////////////////////////////////////////////////////////////////////////////
} // namespace mgl

#endif /* MGL_MESH_HPP  */
