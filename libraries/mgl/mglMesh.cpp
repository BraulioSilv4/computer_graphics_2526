////////////////////////////////////////////////////////////////////////////////
//
// Mesh Loader Class
//
// Copyright (c)2022-25 by Carlos Martinho
//
////////////////////////////////////////////////////////////////////////////////

#include "./mglMesh.hpp"

#include <iostream>
#include <mglConventions.hpp>

namespace mgl {

////////////////////////////////////////////////////////////////////////////////

Mesh::Mesh() {
  NormalsLoaded = false;
  TexcoordsLoaded = false;
  TangentsAndBitangentsLoaded = false;
  VaoId = -1;
  AssimpFlags = aiProcess_Triangulate;
}

Mesh::~Mesh() { destroyBufferObjects(); }

void Mesh::setAssimpFlags(unsigned int flags) { AssimpFlags = flags; }

void Mesh::joinIdenticalVertices() {
  AssimpFlags |= aiProcess_JoinIdenticalVertices;
}

void Mesh::generateNormals() { AssimpFlags |= aiProcess_GenNormals; }

void Mesh::generateSmoothNormals() {
  AssimpFlags |= aiProcess_GenSmoothNormals;
}

void Mesh::generateTexcoords() { AssimpFlags |= aiProcess_GenUVCoords; }

void Mesh::calculateTangentSpace() {
  AssimpFlags |= aiProcess_CalcTangentSpace;
}

void Mesh::flipUVs() { AssimpFlags |= aiProcess_FlipUVs; }

bool Mesh::hasNormals() { return NormalsLoaded; }

bool Mesh::hasTexcoords() { return TexcoordsLoaded; }

bool Mesh::hasTangentsAndBitangents() { return TangentsAndBitangentsLoaded; }

////////////////////////////////////////////////////////////////////////////////

void Mesh::processMesh(const aiMesh *mesh) {
  NormalsLoaded = mesh->HasNormals();
  
  // Check if mesh has texture coordinates in the primary (0th) UV channel
  TexcoordsLoaded = mesh->HasTextureCoords(0);
  TangentsAndBitangentsLoaded = mesh->HasTangentsAndBitangents();

  for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
    const aiVector3D &aiPosition = mesh->mVertices[i];
    Positions.push_back(glm::vec3(aiPosition.x, aiPosition.y, aiPosition.z));
    if (NormalsLoaded) {
      const aiVector3D &aiNormal = mesh->mNormals[i];
      Normals.push_back(glm::vec3(aiNormal.x, aiNormal.y, aiNormal.z));
    }
    if (TexcoordsLoaded) {
      const aiVector3D &aiTexcoord = mesh->mTextureCoords[0][i];
      Texcoords.push_back(glm::vec2(aiTexcoord.x, aiTexcoord.y));
    }
    if (TangentsAndBitangentsLoaded) {
      const aiVector3D &aiTangent = mesh->mTangents[i];
      Tangents.push_back(glm::vec3(aiTangent.x, aiTangent.y, aiTangent.z));

#ifdef CREATE_BITANGENT
      const aiVector3D &aiBitangent = mesh->mBitangents[i];
      Bitangents.push_back(
          glm::vec3(aiBitangent.x, aiBitangent.y, aiBitangent.z));
#endif
    }
  }

  for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
    // Assuming all mesh faces are triangles
    const aiFace &face = mesh->mFaces[i];
    Indices.push_back(face.mIndices[0]);
    Indices.push_back(face.mIndices[1]);
    Indices.push_back(face.mIndices[2]);
  }
}

void Mesh::clear() {
  Positions.clear();
  Normals.clear();
  Texcoords.clear();
  Tangents.clear();
#ifdef CREATE_BITANGENT
  Bitangents.clear();
#endif
  Indices.clear();
  Meshes.clear();
}

void Mesh::processScene(const aiScene *scene) {
  Meshes.resize(scene->mNumMeshes);
  Materials.resize(scene->mNumMaterials);

  unsigned int n_vertices = 0;
  unsigned int n_indices = 0;
  for (unsigned int i = 0; i < Meshes.size(); i++) {
    // Assuming all mesh faces are triangles
    Meshes[i].materialIndex = scene->mMeshes[i]->mMaterialIndex;
    Meshes[i].nIndices = scene->mMeshes[i]->mNumFaces * 3;
    Meshes[i].baseVertex = n_vertices;
    Meshes[i].baseIndex = n_indices;

    n_vertices += scene->mMeshes[i]->mNumVertices;
    n_indices += Meshes[i].nIndices;
  }

  Positions.reserve(n_vertices);
  Normals.reserve(n_vertices);
  Texcoords.reserve(n_vertices);
  Indices.reserve(n_indices);

  for (unsigned int i = 0; i < Meshes.size(); i++) {
    processMesh(scene->mMeshes[i]);
  }
}

std::string getFilePathFromDirectory(const std::string& directory, const aiString& file) {
    std::string filePath(file.data);
    return directory + "\\" + filePath;
}

std::string getDirectoryFromFile(const std::string filename) {
    std::string::size_type slashIndex = filename.find_last_of("/\\");
    switch (slashIndex) {
        case std::string::npos:
            return ".";
            break;

        case 0:
            return filename.substr(0, 1);
            break;

        default:
            return filename.substr(0, slashIndex);
    }
}

void Mesh::loadMaterials(const aiScene* scene, const std::string& filename) {
    /* "..\\assets\\models\\Cube\\cube_with_materials.obj" */
    std::string directory = getDirectoryFromFile(filename);

    for (unsigned int i = 0; i < scene->mNumMaterials; i++) {
        /* Container for multiple textues such as diffuse, normal map, roughness etc */
        const aiMaterial* material = scene->mMaterials[i];
        Materials[i].name = material->GetName().C_Str();

        loadTextures(directory, material, i);
        loadMaterialParameters(material, i);
    }
}

void Mesh::loadTextures(const std::string& directory, const aiMaterial* material, int idx) {
    /* Currently only works with obj files */
    loadAlbedoTex(directory, material, idx);
    loadRoughnessTex(directory, material, idx);
    loadMetallicTex(directory, material, idx);
    loadNormalMapTex(directory, material, idx);
}

void Mesh::loadAlbedoTex(const std::string& directory, const aiMaterial* material, int idx) {
    if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
        aiString path;

        if (material->GetTexture(aiTextureType_DIFFUSE, 0, &path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {
            loadAlbedoTexFromFile(directory, path, idx);
        }
    }
}

void Mesh::loadAlbedoTexFromFile(const std::string& directory, const aiString& path, int idx) {
    std::string texPath = getFilePathFromDirectory(directory, path);
    Materials[idx].matTex.texAlbedo = new Texture(GL_TEXTURE_2D, texPath.c_str());

    bool isSRGB = true;

    if (!Materials[idx].matTex.texAlbedo->load(isSRGB)) {
        std::cout << "Error loading texture: " << texPath << std::endl;
        exit(0);
    }

    std::cout << "Loaded Albedo Texture: " << texPath.c_str();
}

void Mesh::loadRoughnessTex(const std::string& directory, const aiMaterial* material, int idx) {
    if (material->GetTextureCount(aiTextureType_DIFFUSE_ROUGHNESS) > 0) {
        aiString path;

        if (material->GetTexture(aiTextureType_DIFFUSE_ROUGHNESS, 0, &path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {
            loadRoughnessTexFromFile(directory, path, idx);
        }
    }
}

void Mesh::loadRoughnessTexFromFile(const std::string& directory, const aiString& path, int idx) {
    std::string texPath = getFilePathFromDirectory(directory, path);
    Materials[idx].matTex.texRoughness = new Texture(GL_TEXTURE_2D, texPath.c_str());

    bool isSRGB = false;

    if (!Materials[idx].matTex.texRoughness->load(isSRGB)) {
        std::cout << "Error loading texture: " << texPath << std::endl;
        exit(0);
    }

    std::cout << "Loaded Roughness Texture: " << texPath.c_str();
}

void Mesh::loadMetallicTex(const std::string& directory, const aiMaterial* material, int idx) {
    if (material->GetTextureCount(aiTextureType_METALNESS) > 0) {
        aiString path;

        if (material->GetTexture(aiTextureType_METALNESS, 0, &path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {
            loadMetallicTexFromFile(directory, path, idx);
        }
    }
}

void Mesh::loadMetallicTexFromFile(const std::string& directory, const aiString& path, int idx) {
    std::string texPath = getFilePathFromDirectory(directory, path);
    Materials[idx].matTex.texMetallic = new Texture(GL_TEXTURE_2D, texPath.c_str());

    bool isSRGB = false;

    if (!Materials[idx].matTex.texMetallic->load(isSRGB)) {
        std::cout << "Error loading texture: " << texPath << std::endl;
        exit(0);
    }

    std::cout << "Loaded Metallic Texture: " << texPath.c_str();
}

void Mesh::loadNormalMapTex(const std::string& directory, const aiMaterial* material, int idx) {
    int numNormals = material->GetTextureCount(aiTextureType_NORMALS);
    int numHeight = material->GetTextureCount(aiTextureType_HEIGHT);

    aiString path;
    if (numNormals > 0) {
        if (material->GetTexture(aiTextureType_NORMALS, 0, &path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {
            loadNormalMapTexFromFile(directory, path, idx);
        }
    }
    else if (numHeight > 0) {
        if (material->GetTexture(aiTextureType_HEIGHT, 0, &path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {
            loadNormalMapTexFromFile(directory, path, idx);
        }
    }
}

void Mesh::loadNormalMapTexFromFile(const std::string& directory, const aiString& path, int idx) {
    std::string texPath = getFilePathFromDirectory(directory, path);
    Materials[idx].matTex.texNormalMap = new Texture(GL_TEXTURE_2D, texPath.c_str());

    bool isSRGB = false;

    if (!Materials[idx].matTex.texNormalMap->load(isSRGB)) {
        std::cout << "Error loading texture: " << texPath << std::endl;
        exit(0);
    }

    std::cout << "Loaded Normal Map Texture: " << texPath.c_str();
}

void Mesh::loadMaterialParameters(const aiMaterial* material, int idx) {
    if (material->Get(AI_MATKEY_METALLIC_FACTOR, Materials[idx].matProps.metallic) == AI_SUCCESS) {
        std::cout << "Pm " << Materials[idx].matProps.metallic << std::endl;
    }

    if (material->Get(AI_MATKEY_ROUGHNESS_FACTOR, Materials[idx].matProps.roughness) == AI_SUCCESS) {
        std::cout << "Pr " << Materials[idx].matProps.roughness << std::endl;
    }

    if (material->Get(AI_MATKEY_CLEARCOAT_FACTOR, Materials[idx].matProps.clearcoat) == AI_SUCCESS) {
        std::cout << "Pc " << Materials[idx].matProps.clearcoat << std::endl;
    }

    if (material->Get(AI_MATKEY_CLEARCOAT_FACTOR, Materials[idx].matProps.clearcoatRoughness) == AI_SUCCESS) {
        std::cout << "Pcr " << Materials[idx].matProps.clearcoatRoughness<< std::endl;
    }
}

void Mesh::create(const std::string &filename) {
    /* Clear previous mesh */
    clear();

    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(filename, AssimpFlags);

    /* 
        Currently I have a version of Assimp that throws an error when trying to parse aniso in .mtl file.
        It has been fixed in the master branch. TODO() update assimp.
    */
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cerr << "Error while loading:" << importer.GetErrorString() << std::endl;
        exit(EXIT_FAILURE);
    }

    id = filename;
    processScene(scene);
    loadMaterials(scene, filename);
    createBufferObjects();
}

void Mesh::createBufferObjects() {
    GLuint boId[6];

    /* Generate vertex array for this mesh and activate it */
    glGenVertexArrays(1, &VaoId);
    glBindVertexArray(VaoId);
  
    /* Create six different buffers for data */
    glGenBuffers(6, boId);

    /* 
        1 - Activate the VBO for position data 
        2 - Copy data from the CPU to the VBO in the GPU
        3 - Enables this attribute to be sent to the shaders
        4 - How to parse this attribute
    */
    glBindBuffer(GL_ARRAY_BUFFER, boId[POSITION]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Positions[0]) * Positions.size(), &Positions[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(POSITION);
    glVertexAttribPointer(POSITION, 3, GL_FLOAT, GL_FALSE, 0, 0);

    if (NormalsLoaded) {
        glBindBuffer(GL_ARRAY_BUFFER, boId[NORMAL]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Normals[0]) * Normals.size(), &Normals[0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(NORMAL);
        glVertexAttribPointer(NORMAL, 3, GL_FLOAT, GL_FALSE, 0, 0);
    }

    if (TexcoordsLoaded) {
        glBindBuffer(GL_ARRAY_BUFFER, boId[TEXCOORD]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Texcoords[0]) * Texcoords.size(), &Texcoords[0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(TEXCOORD);
        glVertexAttribPointer(TEXCOORD, 2, GL_FLOAT, GL_FALSE, 0, 0);
    }

    if (TangentsAndBitangentsLoaded) {
        glBindBuffer(GL_ARRAY_BUFFER, boId[TANGENT]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Tangents[0]) * Tangents.size(), &Tangents[0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(TANGENT);
        glVertexAttribPointer(TANGENT, 3, GL_FLOAT, GL_FALSE, 0, 0);

    #ifdef CREATE_BITANGENT
        glBindBuffer(GL_ARRAY_BUFFER, boId[BITANGENT]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Bitangents[0]) * Bitangents.size(), &Bitangents[0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(BITANGENT);
        glVertexAttribPointer(BITANGENT, 3, GL_FLOAT, GL_FALSE, 0, 0);
    #endif
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, boId[INDEX]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices[0]) * Indices.size(), &Indices[0], GL_STATIC_DRAW);
  
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDeleteBuffers(6, boId);
}

void Mesh::destroyBufferObjects() {
  glBindVertexArray(VaoId);
  glDisableVertexAttribArray(POSITION);
  glDisableVertexAttribArray(NORMAL);
  glDisableVertexAttribArray(TEXCOORD);
  glDisableVertexAttribArray(TANGENT);
#ifdef CREATE_BITANGENT
  glDisableVertexAttribArray(BITANGENT);
#endif
  glDeleteVertexArrays(1, &VaoId);
  glBindVertexArray(0);
}

void Mesh::bindMaterialsTextures(int materialIdx) {
    if (Materials[materialIdx].matTex.texAlbedo) {
        Materials[materialIdx].matTex.texAlbedo->bind(ALBEDO_TEXTURE_UNIT); /* Albedo to texture0_unit */
    }

    if (Materials[materialIdx].matTex.texMetallic) {
        Materials[materialIdx].matTex.texMetallic->bind(METALLIC_TEXTURE_UNIT); /* Metallic to texture1_unit */
    }

    if (Materials[materialIdx].matTex.texNormalMap) {
        Materials[materialIdx].matTex.texNormalMap->bind(NORMAL_TEXTURE_UNIT); /* Normal map to texture2_unit */
    }

    if (Materials[materialIdx].matTex.texRoughness) {
        Materials[materialIdx].matTex.texRoughness->bind(ROUGHNESS_TEXTURE_UNIT); /* Roughness map to texture3_unit */
    }
}

void Mesh::draw() {
    glBindVertexArray(VaoId);

    for (MeshData &mesh : Meshes) {
        unsigned int materialIndex = mesh.materialIndex;

        bindMaterialsTextures(materialIndex);

        glDrawElementsBaseVertex(
            GL_TRIANGLES, 
            mesh.nIndices, 
            GL_UNSIGNED_INT,
            reinterpret_cast<void *>((sizeof(unsigned int) * mesh.baseIndex)),
            mesh.baseVertex
        );
        // GLenum mode, GLsizei count, GLenum type, void *indices, GLint basevertex
    }
  
    glBindVertexArray(0);
}

std::string Mesh::getID() const {
    return id;
}

////////////////////////////////////////////////////////////////////////////////
} // namespace mgl
