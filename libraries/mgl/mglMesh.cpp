////////////////////////////////////////////////////////////////////////////////
//
// Mesh Loader Class
//
// Copyright (c)2022-25 by Carlos Martinho
//
////////////////////////////////////////////////////////////////////////////////

#include "./mglMesh.hpp"

#include <iostream>

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
  Textures.resize(scene->mNumMaterials);

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

void Mesh::loadMaterials(const aiScene* scene, const std::string& filename) {
    /* "..\\assets\\models\\Cube\\cube_with_materials.obj" */
    std::string::size_type slashIndex = filename.find_last_of("/\\");
    std::string directory;

    if (slashIndex == std::string::npos) {
        directory = ".";
    }
    else if (slashIndex == 0) {
        directory = filename[slashIndex];
    }
    else {
        directory = filename.substr(0, slashIndex);
    }

    for (unsigned int i = 0; i < scene->mNumMaterials; i++) {
        /* Container for multiple textues such as diffuse, normal map, roughness etc */
        const aiMaterial* material = scene->mMaterials[i];

        Textures[i] = NULL;

        if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
            aiString path;

            if (material->GetTexture(aiTextureType_DIFFUSE, 0, &path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {
                std::string texturePath(path.data);

                if (texturePath.substr(0, 2) == ".\\") {
                    texturePath = texturePath.substr(2, texturePath.size() - 2);
                }

                std::string fullPath = directory + "\\" + texturePath;

                Textures[i] = new Texture(GL_TEXTURE_2D, fullPath.c_str());

                if (!Textures[i]->load()) {
                    std::cout << "Error loading texture: " << fullPath.c_str() << std::endl;
                    delete Textures[i];
                    Textures[i] = NULL;
                } else {
                    std::cout << "Loaded texture: " << fullPath.c_str() << std::endl;
                }
            }
        }
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

void Mesh::draw() {
    glBindVertexArray(VaoId);

    for (MeshData &mesh : Meshes) {
        unsigned int materialIndex = mesh.materialIndex;

        if (Textures[materialIndex]) {
            Textures[materialIndex]->bind(GL_TEXTURE0); /* Texture to texture0_unit */
        }

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
