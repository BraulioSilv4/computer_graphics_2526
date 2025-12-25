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
  void cursorCallback(GLFWwindow* window, double xpos, double ypos) override;
  void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods, double elapsed) override;
  void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) override;
  void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) override;


private:
  glm::vec2 MouseInput = glm::vec2(0); 
  glm::vec2 LeftInput = glm::vec2(0);
  bool RightPressed = false;
  bool LeftPressed = false;
  bool ortho_mode = false;
  glm::mat4 Ortho_Pro = glm::ortho(-2.0f, 2.0f, -2.0f, 2.0f, 0.1f, 100.0f);
  glm::mat4 Pesp_Pro = glm::perspective(glm::radians(30.0f), 640.0f / 480.0f, 0.1f, 100.0f);

  const GLuint UBO_BP = 0;
  mgl::Camera *Camera = nullptr;
  std::unique_ptr<mgl::OrbitCamera> boxCamera;
  mgl::OrbitCamera* activeCamera = nullptr;
  mgl::ShaderProgram *Shaders = nullptr;
  GLint ModelMatrixId;
  mgl::Manager<mgl::Mesh, std::string> MeshManager;
  mgl::Registry<std::string, mgl::SceneNode*> NodeRegistry;
  std::unique_ptr<mgl::SceneNode> sceneRoot = nullptr;

  void createCamera();
  void createMeshes();
  void createSceneGraph();
  void createShaderPrograms();
  void drawScene(double elapsed);
};

///////////////////////////////////////////////////////////////////////// MESHES

void MyApp::createMeshes() {
    std::string mesh_path = "..\\assets\\models\\Cube\\cube_with_materials.obj";
    mgl::Mesh* m = new mgl::Mesh();
    m->generateSmoothNormals();
    m->calculateTangentSpace();
    m->joinIdenticalVertices();
    m->flipUVs();
    m->create(mesh_path);

    MeshManager.add(std::unique_ptr<mgl::Mesh>(m));
}

///////////////////////////////////////////////////////////////////////// SHADER

void MyApp::createShaderPrograms() {
  Shaders = new mgl::ShaderProgram();
  Shaders->addShader(GL_VERTEX_SHADER, "cube-vs.glsl");
  Shaders->addShader(GL_FRAGMENT_SHADER, "cube-fs.glsl");

  // TODO() Change this Meshes[0]
  Shaders->addAttribute(mgl::POSITION_ATTRIBUTE, mgl::Mesh::POSITION);
  if (MeshManager.get("..\\assets\\models\\Cube\\cube_with_materials.obj")->hasNormals()) {
    Shaders->addAttribute(mgl::NORMAL_ATTRIBUTE, mgl::Mesh::NORMAL);
  }
  if (MeshManager.get("..\\assets\\models\\Cube\\cube_with_materials.obj")->hasTexcoords()) {
    Shaders->addAttribute(mgl::TEXCOORD_ATTRIBUTE, mgl::Mesh::TEXCOORD);
  }
  if (MeshManager.get("..\\assets\\models\\Cube\\cube_with_materials.obj")->hasTangentsAndBitangents()) {
    Shaders->addAttribute(mgl::TANGENT_ATTRIBUTE, mgl::Mesh::TANGENT);
  }

  Shaders->addUniform(mgl::DIFFUSE_SAMPLER);
  Shaders->addUniform(mgl::NORMAL_SAMPLER);
  Shaders->addUniform(mgl::METAL_SAMPLER);
  Shaders->addUniform(mgl::ROUGH_SAMPLER);
  //Shaders->addUniform(mgl::HEIGHT_SAMPLER);

  Shaders->addUniform(mgl::OBJECT_COLOR);
  Shaders->addUniform(mgl::MODEL_MATRIX);
  Shaders->addUniformBlock(mgl::CAMERA_BLOCK, UBO_BP);
  Shaders->create();

  /* Assigning texture units to samplers */
  Shaders->bind();
  glUniform1i(Shaders->Uniforms[mgl::DIFFUSE_SAMPLER].index, mgl::ALBEDO_UNIT_INDEX);
  glUniform1i(Shaders->Uniforms[mgl::ROUGH_SAMPLER].index, mgl::ROUGHNESS_UNIT_INDEX);
  glUniform1i(Shaders->Uniforms[mgl::METAL_SAMPLER].index, mgl::METALLIC_UNIT_INDEX);
  glUniform1i(Shaders->Uniforms[mgl::NORMAL_SAMPLER].index, mgl::NORMAL_UNIT_INDEX);
  Shaders->unbind();
  ModelMatrixId = Shaders->Uniforms[mgl::MODEL_MATRIX].index;
}

///////////////////////////////////////////////////////////////////////// SCENE GRAPH

void MyApp::createSceneGraph() {
    std::string rootName = "cube_with_materials.obj";

    auto root = std::make_unique<mgl::SceneNode>(
        rootName,
        MeshManager.get("..\\assets\\models\\Cube\\cube_with_materials.obj"),
        Shaders
    );

    sceneRoot = std::move(root);
    NodeRegistry.add(mgl::CUBE, sceneRoot.get());
}

///////////////////////////////////////////////////////////////////////// CAMERA

void MyApp::createCamera() {
    Camera = new mgl::Camera(UBO_BP);

    glm::vec3 BoxCenter = glm::vec3(0.0f, 0.2f, 0.0f);
    glm::vec3 BoxPosition = glm::vec3(5.0f, 5.0f, 5.0f);
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

    boxCamera = std::make_unique<mgl::OrbitCamera>(Camera, BoxCenter, BoxPosition, up);
    boxCamera->setProjection(Pesp_Pro);

    /* Setting default active camera to box camera */
    activeCamera = boxCamera.get();
}

/////////////////////////////////////////////////////////////////////////// DRAW


void MyApp::drawScene(double elapsed) {
    Shaders->bind();
    sceneRoot->drawSceneGraph();
    sceneRoot->transformRotate(glm::radians(1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    sceneRoot->transformRotate(glm::radians(0.5f), glm::vec3(1.0f, 0.0f, 0.0f));
    sceneRoot->transformRotate(glm::radians(0.2f), glm::vec3(0.0f, 0.0f, 1.0f));
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
  if (ortho_mode) {
      float ratio = (float)winx / winy;
      Ortho_Pro = glm::ortho(-2.0f, 2.0f * 3 / 4 * ratio, -2.0f * 3 / 4 * ratio, 2.0f, 0.1f, 100.0f);
      Camera->setProjectionMatrix(Ortho_Pro);
  }
  else {
      float ratio = (float)winx / winy;
      Pesp_Pro = glm::perspective(glm::radians(30.0f), ratio, 0.1f, 100.0f);
      Camera->setProjectionMatrix(Pesp_Pro);
  }
}

void MyApp::displayCallback(GLFWwindow *win, double elapsed) { 
    drawScene(elapsed); 
}

void MyApp::cursorCallback(GLFWwindow* window, double xpos, double ypos) {
    float dx = xpos - MouseInput.x;
    float dy = ypos - MouseInput.y;
    if (RightPressed) {
        activeCamera->rotate(dx, dy);
    }
    if (LeftPressed) {
        activeCamera->move(dx, dy);
    }
    MouseInput = glm::vec2(xpos, ypos);
}

void MyApp::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods, double elapsed) {
    if (key == GLFW_KEY_P) {
        if (action == GLFW_PRESS) {
            ortho_mode = !ortho_mode;
            ortho_mode ? activeCamera->setProjection(Ortho_Pro) : activeCamera->setProjection(Pesp_Pro);
        }
    }
}

void MyApp::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            LeftPressed = true;
            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);
            LeftInput = glm::vec2(xpos, ypos);
        }

        if (action == GLFW_RELEASE) {
            LeftPressed = false;
        }
    }
    else {
        if (action == GLFW_PRESS) {
            RightPressed = true;
            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);
            MouseInput = glm::vec2(xpos, ypos);
        }

        if (action == GLFW_RELEASE) {
            RightPressed = false;
        }
    }
}

void MyApp::scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    //xoffset parece ser inutil para o nosso projeto
    //aproximar/afastar do centro da camera
    activeCamera->zoom(yoffset);
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
