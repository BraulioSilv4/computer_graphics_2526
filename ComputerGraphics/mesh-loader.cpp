////////////////////////////////////////////////////////////////////////////////
//
//  Loading meshes from external files
//
// Copyright (c) 2023-25 by Carlos Martinho
//
// INTRODUCES:
// MODEL DATA, ASSIMP, mglMesh.hpp
//
////////////////////////////////////////////////////////////////////////////////

#define GLM_ENABLE_EXPERIMENTAL

#include "../mgl/mgl.hpp"

#include <iostream>

////////////////////////////////////////////////////////////////////////// MYAPP

class MyApp : public mgl::App {
public:
  void initCallback(GLFWwindow *win) override;
  void displayCallback(GLFWwindow *win, double elapsed) override;
  void windowSizeCallback(GLFWwindow *win, int width, int height) override;

private:
  const GLuint UBO_BP = 0;
  mgl::ShaderProgram *Shaders = nullptr;
  mgl::Camera *Camera = nullptr;
  GLint ModelMatrixId;
  Manager<mgl::Mesh, std::string> MeshManager;
  Registry<std::string, mgl::SceneNode*> NodeRegistry;

  std::unique_ptr<mgl::SceneNode> sceneRoot = nullptr;

  void createMeshes();
  void createSceneGraph();
  void createShaderPrograms();
  void createCamera();
  void drawScene(double elapsed);
};

///////////////////////////////////////////////////////////////////////// MESHES

void MyApp::createMeshes() {
  std::string mesh_dir = "..\\assets\\models\\PickagramParts\\";
  std::vector<std::string> mesh_files = {
    "Table.obj",
	"Cube1.obj",
	"MajorTriangle1.obj",
	"MajorTriangle2.obj",
    "MiniTriangle1.obj",
    "MiniTriangle2.obj",
    "Parallelogram.obj",
    "Triangle1.obj"
  };

  for (const auto str : mesh_files) {
    auto path = mesh_dir + str; 
    mgl::Mesh* m = new mgl::Mesh();
	m->joinIdenticalVertices();
    m->create(path);

    MeshManager.add(std::unique_ptr<mgl::Mesh>(m));
  }
}

///////////////////////////////////////////////////////////////////////// SHADER

void MyApp::createShaderPrograms() {
  Shaders = new mgl::ShaderProgram();
  Shaders->addShader(GL_VERTEX_SHADER, "cube-vs.glsl");
  Shaders->addShader(GL_FRAGMENT_SHADER, "cube-fs.glsl");

  // TODO() Change this Meshes[0]
  Shaders->addAttribute(mgl::POSITION_ATTRIBUTE, mgl::Mesh::POSITION);
  if (MeshManager.get("..\\assets\\models\\PickagramParts\\Table.obj")->hasNormals()) {
    Shaders->addAttribute(mgl::NORMAL_ATTRIBUTE, mgl::Mesh::NORMAL);
  }
  if (MeshManager.get("..\\assets\\models\\PickagramParts\\Table.obj")->hasTexcoords()) {
    Shaders->addAttribute(mgl::TEXCOORD_ATTRIBUTE, mgl::Mesh::TEXCOORD);
  }
  if (MeshManager.get("..\\assets\\models\\PickagramParts\\Table.obj")->hasTangentsAndBitangents()) {
    Shaders->addAttribute(mgl::TANGENT_ATTRIBUTE, mgl::Mesh::TANGENT);
  }

  Shaders->addUniform(mgl::MODEL_MATRIX);
  Shaders->addUniformBlock(mgl::CAMERA_BLOCK, UBO_BP);
  Shaders->create();

  ModelMatrixId = Shaders->Uniforms[mgl::MODEL_MATRIX].index;
}


///////////////////////////////////////////////////////////////////////// SCENE GRAPH

void MyApp::createSceneGraph() {
    std::string rootName = "Table.obj";

    auto root = std::make_unique<mgl::SceneNode>(
        rootName,
        MeshManager.get("..\\assets\\models\\PickagramParts\\Table.obj"),
        Shaders
    );

    sceneRoot = std::move(root);
    NodeRegistry.add("Table.obj", sceneRoot.get());

    for (const auto& mesh : MeshManager) {
        std::string name = mesh->getID().substr(mesh->getID().find_last_of("\\") + 1);

        if (name == rootName) continue;

        auto node = std::make_unique<mgl::SceneNode>(
            name,
            mesh.get()
        );

        NodeRegistry.add(name, node.get());
        sceneRoot->addChild(std::move(node));
    }
}


///////////////////////////////////////////////////////////////////////// CAMERA

// Eye(5,5,5) Center(0,0,0) Up(0,1,0)
const glm::mat4 ViewMatrix1 =
    glm::lookAt(glm::vec3(5.0f, 5.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f),
                glm::vec3(0.0f, 1.0f, 0.0f));

// Eye(-5,-5,-5) Center(0,0,0) Up(0,1,0)
const glm::mat4 ViewMatrix2 =
    glm::lookAt(glm::vec3(-5.0f, -5.0f, -5.0f), glm::vec3(0.0f, 0.0f, 0.0f),
                glm::vec3(0.0f, 1.0f, 0.0f));

// Orthographic LeftRight(-2,2) BottomTop(-2,2) NearFar(1,10)
const glm::mat4 ProjectionMatrix1 =
    glm::ortho(-2.0f, 2.0f, -2.0f, 2.0f, 1.0f, 10.0f);

// Perspective Fovy(30) Aspect(640/480) NearZ(1) FarZ(10)
const glm::mat4 ProjectionMatrix2 =
    glm::perspective(glm::radians(30.0f), 640.0f / 480.0f, 1.0f, 10.0f);

void MyApp::createCamera() {
  Camera = new mgl::Camera(UBO_BP);
  Camera->setViewMatrix(ViewMatrix1);
  Camera->setProjectionMatrix(ProjectionMatrix2);
}

/////////////////////////////////////////////////////////////////////////// DRAW


void MyApp::drawScene(double elapsed) {
    std::cout << elapsed << std::endl;
    NodeRegistry.get(TABLE)->transformRotate(glm::radians(-1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    NodeRegistry.get(CUBE)->transformTranslate(glm::vec3(0.0f, 0.01f, 0.0f));
    NodeRegistry.get(MAJOR_TRIANGLE_1)->transformTranslate(glm::vec3(0.0f, 0.01f, 0.0f));
    NodeRegistry.get(MAJOR_TRIANGLE_2)->transformTranslate(glm::vec3(0.0f, 0.01f, 0.0f));
    NodeRegistry.get(MINI_TRIANGLE_1)->transformTranslate(glm::vec3(0.0f, 0.01f, 0.0f));
    NodeRegistry.get(MINI_TRIANGLE_2)->transformTranslate(glm::vec3(0.0f, 0.01f, 0.0f));
    NodeRegistry.get(TRIANGLE)->transformTranslate(glm::vec3(0.0f, 0.01f, 0.0f));
    NodeRegistry.get(PARALLELOGRAM)->transformTranslate(glm::vec3(0.0f, 0.01f, 0.0f));


    Shaders->bind();
    sceneRoot->drawSceneGraph();
    Shaders->unbind();
}

////////////////////////////////////////////////////////////////////// CALLBACKS

void MyApp::initCallback(GLFWwindow *win) {
  createMeshes();
  createShaderPrograms(); // after mesh;
  createSceneGraph();
  createCamera();
}

void MyApp::windowSizeCallback(GLFWwindow *win, int winx, int winy) {
  glViewport(0, 0, winx, winy);
  // change projection matrices to maintain aspect ratio
}

void MyApp::displayCallback(GLFWwindow *win, double elapsed) { 
    drawScene(elapsed); 
}

/////////////////////////////////////////////////////////////////////////// MAIN

int main(int argc, char *argv[]) {
  mgl::Engine &engine = mgl::Engine::getInstance();
  engine.setApp(new MyApp());
  engine.setOpenGL(4, 6);
  engine.setWindow(800, 600, "Mesh Loader", 0, 1);
  engine.init();
  engine.run();
  exit(EXIT_SUCCESS);
}

////////////////////////////////////////////////////////////////////////////////
