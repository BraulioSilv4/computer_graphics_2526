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
  void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods, double elapsed) override;

private:
  const GLuint UBO_BP = 0;
  mgl::ShaderProgram *Shaders = nullptr;
  mgl::Camera *Camera = nullptr;
  GLint ModelMatrixId;
  mgl::Manager<mgl::Mesh, std::string> MeshManager;
  mgl::Registry<std::string, mgl::SceneNode*> NodeRegistry;
  mgl::AnimationGroup Animations;

  std::unique_ptr<mgl::SceneNode> sceneRoot = nullptr;

  void createCamera();
  void createMeshes();
  void createAnimation();
  void createSceneGraph();
  void createShaderPrograms();
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

///////////////////////////////////////////////////////////////////////// CREATE ANIMATION

void MyApp::createAnimation() {
    Animations.addAnimation(
        std::make_unique<mgl::Animation>(
            NodeRegistry.get(mgl::MAJOR_TRIANGLE_1),
            0.5f,
            glm::vec3(0.043611f, 1.180088, 0.835187f),
            glm::vec3(1.0f, 1.0f, 1.0f),
            glm::quat(0.428f, 0.563f, -0.428f, -0.563f)
        )
    );

    Animations.addAnimation(
        std::make_unique<mgl::Animation>(
            NodeRegistry.get(mgl::MAJOR_TRIANGLE_2),
            0.5f,
            glm::vec3(0.042847f, 1.57046f, 0.072364f),
            glm::vec3(1.0f, 1.0f, 1.0f),
            glm::quat(0.684f, -0.181f, 0.684f, -0.181f)
        )
    );

    Animations.addAnimation(
        std::make_unique<mgl::Animation>(
            NodeRegistry.get(mgl::MINI_TRIANGLE_2),
            0.5f,
            glm::vec3(0.041369f, 0.869238f, 0.287992f),
            glm::vec3(1.0f, 1.0f, 1.0f),
            glm::quat(0.701f, 0.095f, -0.701f, -0.095f)
        )
    );

    Animations.addAnimation(
        std::make_unique<mgl::Animation>(
            NodeRegistry.get(mgl::MINI_TRIANGLE_1),
            0.5f,
            glm::vec3(0.04304f, 2.23194f, 0.034844f),
            glm::vec3(1.0f, 1.0f, 1.0f),
            glm::quat(0.684f, -0.181f, 0.684f, -0.181f)
        )
    );

    Animations.addAnimation(
        std::make_unique<mgl::Animation>(
            NodeRegistry.get(mgl::CUBE),
            0.5f,
            glm::vec3(0.043611f, 1.86635f, -0.60512f),
            glm::vec3(1.0f, 1.0f, 1.0f),
            glm::quat(-0.356f, 0.611f, -0.356f, 0.611f)
        )
    );

    Animations.addAnimation(
        std::make_unique<mgl::Animation>(
            NodeRegistry.get(mgl::PARALLELOGRAM),
            0.5f,
            glm::vec3(0.043153f, 2.23836f, -1.12787f),
            glm::vec3(1.0f, 1.0f, 1.0f),
            glm::quat(-0.684f, 0.181f, 0.684f, -0.181f)
        )
    );

    Animations.addAnimation(
        std::make_unique<mgl::Animation>(
            NodeRegistry.get(mgl::TRIANGLE),
            0.5f,
            glm::vec3(0.039279f, 3.06282f, -1.59544),
            glm::vec3(1.0f, 1.0f, 1.0f),
            glm::quat(0.419f, 0.570f, 0.438f, 0.555f)
        )
    );
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
    NodeRegistry.add(mgl::TABLE, sceneRoot.get());

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

    /* Pickagram Closed Configuration (Positions Obtained in Blender) */
    /* Scale whole scene down to be in clip space */
    NodeRegistry.get(mgl::TABLE)->setScale(glm::vec3(0.5f, 0.5f, 0.5f));
    NodeRegistry.get(mgl::TABLE)->setPosition(glm::vec3(0.0f, 0.0f, 0.0f));

    NodeRegistry.get(mgl::TRIANGLE)->setPosition(glm::vec3(-0.6f, 0.77f, 0.6f));
    NodeRegistry.get(mgl::TRIANGLE)->setRotation(glm::radians(90.f), glm::vec3(1.0f, 0.0f, 0.0f));

    NodeRegistry.get(mgl::PARALLELOGRAM)->setPosition(glm::vec3(-0.72f, 0.77f, -0.25f));
    NodeRegistry.get(mgl::PARALLELOGRAM)->setRotation(glm::radians(90.f), glm::vec3(1.0f, 0.0f, 0.0f));

    NodeRegistry.get(mgl::CUBE)->setPosition(glm::vec3(0.0f, 0.77f, 0.47f));
    NodeRegistry.get(mgl::CUBE)->setRotation(glm::radians(90.f), glm::vec3(1.0f, 0.0f, 0.0f));

    NodeRegistry.get(mgl::MINI_TRIANGLE_1)->setPosition(glm::vec3(-0.31f, 0.77f, 0.0f));
    NodeRegistry.get(mgl::MINI_TRIANGLE_1)->setRotation(glm::radians(90.f), glm::vec3(1.0f, 0.0f, 0.0f));
    
    NodeRegistry.get(mgl::MINI_TRIANGLE_2)->setPosition(glm::vec3(0.47f, 0.77f, 0.78f));
    NodeRegistry.get(mgl::MINI_TRIANGLE_2)->setRotation(glm::radians(90.f), glm::vec3(1.0f, 0.0f, 0.0f));

    NodeRegistry.get(mgl::MAJOR_TRIANGLE_1)->setPosition(glm::vec3(0.62f, 0.77f, 0.0f));
    NodeRegistry.get(mgl::MAJOR_TRIANGLE_1)->setRotation(glm::radians(90.f), glm::vec3(1.0f, 0.0f, 0.0f));

    NodeRegistry.get(mgl::MAJOR_TRIANGLE_2)->setPosition(glm::vec3(0.0f, 0.77f, -0.62f));
    NodeRegistry.get(mgl::MAJOR_TRIANGLE_2)->setRotation(glm::radians(90.f), glm::vec3(1.0f, 0.0f, 0.0f));
}


///////////////////////////////////////////////////////////////////////// CAMERA

// Eye(5,5,5) Center(0,0,0) Up(0,1,0)
const glm::mat4 ViewMatrix1 =
    glm::lookAt(glm::vec3(-5.0f, 5.0f, -5.0f), glm::vec3(0.0f, 0.0f, 0.0f),
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
    //NodeRegistry.get(mgl::TRIANGLE)->transformRotate(glm::radians(1.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    Shaders->bind();
    sceneRoot->drawSceneGraph();
    Shaders->unbind();
}

////////////////////////////////////////////////////////////////////// CALLBACKS

void MyApp::initCallback(GLFWwindow *win) {
  createMeshes();
  createShaderPrograms(); // after mesh;
  createSceneGraph();
  createAnimation();
  createCamera();
}

void MyApp::windowSizeCallback(GLFWwindow *win, int winx, int winy) {
  glViewport(0, 0, winx, winy);
  // change projection matrices to maintain aspect ratio
}

void MyApp::displayCallback(GLFWwindow *win, double elapsed) { 
    drawScene(elapsed); 
}

void MyApp::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods, double elapsed) {
    std::cout << "Key" << key << "\n" << "Scancode" << scancode << "\n" << "Action" << action << "\n"
        << "mods" << mods << std::endl;
    if (scancode == 333) {
        Animations.play(elapsed);
    }
    if (scancode == 331) {
        Animations.play(elapsed, true);
    }
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
