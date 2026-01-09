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
  glm::vec4 LightColor = glm::vec4(20.0f, 10.0f, 10.0f, 1.0f);
  glm::vec3 LightPosition = glm::vec3(3.0f, 0.0f, 3.0f);

  const GLuint UBO_BP = 0;
  mgl::Camera *Camera = nullptr;
  std::unique_ptr<mgl::OrbitCamera> boxCamera;
  mgl::OrbitCamera* activeCamera = nullptr;
  GLint ModelMatrixId;
  mgl::Manager<mgl::Mesh, std::string> MeshManager;
  mgl::Registry<std::string, mgl::SceneNode*> NodeRegistry;

  /* ImGui */
  mgl::GUI* gui = nullptr;
  
  /* Shader Programs */ 
  mgl::ShaderProgram* Shaders = nullptr;
  mgl::ShaderProgram* SkyboxShaders = nullptr;
  mgl::ShaderProgram* frameBufferShader = nullptr;
  
  /* Frame Buffer */
  FrameBuffer* frameBuffer = nullptr;

  /* Scene Nodes*/
  std::unique_ptr<mgl::SceneNode> sceneRoot = nullptr;
  std::unique_ptr<mgl::SceneNode> hdrCube = nullptr;

  /* Textures */ 
  HDRSkybox* hdrSkybox = nullptr;

  const std::string modelPath = "..\\assets\\models\\Camera\\Camera_01_4k.gltf";

  void createCamera();
  void createMeshes();
  void createSceneGraph();
  void createShaderPrograms();
  void createFrameBuffers();
  void createGUI();

  void updateUnifroms();
  void drawScene(double elapsed, bool opaquePass);
};

///////////////////////////////////////////////////////////////////////// MESHES

void MyApp::createMeshes() {
    std::string mesh_path = modelPath;
    mgl::Mesh* m = new mgl::Mesh();
    m->generateSmoothNormals();
    m->calculateTangentSpace();
    m->joinIdenticalVertices();
    m->create(mesh_path);

    MeshManager.add(std::unique_ptr<mgl::Mesh>(m));

	hdrSkybox = new HDRSkybox("..\\assets\\HDR\\belfast_sunset_puresky_4k.hdr", nullptr);
}

///////////////////////////////////////////////////////////////////////// SHADER

void MyApp::createShaderPrograms() {
    SkyboxShaders = new mgl::ShaderProgram();
    SkyboxShaders->addShader(GL_VERTEX_SHADER, "skybox-vs.glsl");
    SkyboxShaders->addShader(GL_FRAGMENT_SHADER, "skybox-fs.glsl");

    SkyboxShaders->addAttribute(mgl::POSITION_ATTRIBUTE, mgl::Mesh::POSITION);
    SkyboxShaders->addUniform(mgl::EQUIRECTANGULAR_SAMPLER);
    SkyboxShaders->addUniform(mgl::PROJECTION_MATRIX);
    SkyboxShaders->addUniform(mgl::VIEW_MATRIX);

    SkyboxShaders->create();

    SkyboxShaders->bind();
    glUniform1i(SkyboxShaders->Uniforms[mgl::CUBEMAP_SAMPLER].index, mgl::CUBEMAP_UNIT_INDEX);
    glUniform1i(SkyboxShaders->Uniforms[mgl::EQUIRECTANGULAR_SAMPLER].index, mgl::EQUIRECTANGULAR_UNIT_INDEX);
    SkyboxShaders->unbind();

    hdrSkybox->setShaderProgram(SkyboxShaders);
    hdrSkybox->init(); // loads the HDRI texture and creates the cubemap


    Shaders = new mgl::ShaderProgram();
    Shaders->addShader(GL_VERTEX_SHADER, "cube-vs.glsl");
    Shaders->addShader(GL_FRAGMENT_SHADER, "pbr-fs.glsl");

    Shaders->addAttribute(mgl::POSITION_ATTRIBUTE, mgl::Mesh::POSITION);
    if (MeshManager.get(modelPath)->hasNormals()) {
        Shaders->addAttribute(mgl::NORMAL_ATTRIBUTE, mgl::Mesh::NORMAL);
    }
    if (MeshManager.get(modelPath)->hasTexcoords()) {
        Shaders->addAttribute(mgl::TEXCOORD_ATTRIBUTE, mgl::Mesh::TEXCOORD);
    }
    if (MeshManager.get(modelPath)->hasTangentsAndBitangents()) {
        Shaders->addAttribute(mgl::TANGENT_ATTRIBUTE, mgl::Mesh::TANGENT);
    }

    Shaders->addUniform(mgl::DIFFUSE_SAMPLER);
    Shaders->addUniform(mgl::NORMAL_SAMPLER);
    Shaders->addUniform(mgl::ARM_SAMPLER);
    Shaders->addUniform(mgl::BASE_COLOR);
    Shaders->addUniform(mgl::ROUGHNESS_FACTOR);
    Shaders->addUniform(mgl::METALLIC_FACTOR);
    Shaders->addUniform(mgl::IRRADIANCE_SAMPLER);
    Shaders->addUniform(mgl::PREFILTERED_ENV_SAMPLER);
    Shaders->addUniform(mgl::BRDDF_LUT_SAMPLER);
	Shaders->addUniform(mgl::ENABLE_AMBIENT_OCCLUSION);

    Shaders->addUniform(mgl::ENABLE_NORMAL_MAPPING);

    Shaders->addUniform(mgl::MODEL_MATRIX);
    Shaders->addUniform(mgl::CAMERA_POSITION);
    Shaders->addUniformBlock(mgl::CAMERA_BLOCK, UBO_BP);
    Shaders->create();

    /* Assigning texture units to samplers */
    Shaders->bind();
    glUniform1i(Shaders->Uniforms[mgl::DIFFUSE_SAMPLER].index, mgl::ALBEDO_UNIT_INDEX);
    glUniform1i(Shaders->Uniforms[mgl::ARM_SAMPLER].index, mgl::ARM_UNIT_INDEX);
    glUniform1i(Shaders->Uniforms[mgl::NORMAL_SAMPLER].index, mgl::NORMAL_UNIT_INDEX);

    hdrSkybox->bindIrradianceMap(mgl::IRRADIANCE_TEXTURE_UNIT);
    hdrSkybox->bindBRDFLUTTexture(mgl::BRDDF_LUT_TEXTURE_UNIT);
    hdrSkybox->bindPrefilteredEnvMap(mgl::PREFILTERED_ENV_TEXTURE_UNIT);
    glUniform1i(Shaders->Uniforms[mgl::IRRADIANCE_SAMPLER].index, mgl::IRRADIANCE_UNIT_INDEX);
    glUniform1i(Shaders->Uniforms[mgl::BRDDF_LUT_SAMPLER].index, mgl::BRDDF_LUT_UNIT_INDEX);
    glUniform1i(Shaders->Uniforms[mgl::PREFILTERED_ENV_SAMPLER].index, mgl::PREFILTERED_ENV_UNIT_INDEX);
 
    Shaders->unbind();

    ModelMatrixId = Shaders->Uniforms[mgl::MODEL_MATRIX].index;
}

///////////////////////////////////////////////////////////////////////// SCENE GRAPH

void MyApp::createSceneGraph() {
    std::string rootName = "model";

    auto root = std::make_unique<mgl::SceneNode>(
        rootName,
        MeshManager.get(modelPath),
        Shaders
    );


    sceneRoot = std::move(root);
    NodeRegistry.add(mgl::CUBE, sceneRoot.get());
}

///////////////////////////////////////////////////////////////////////// CAMERA

void MyApp::createCamera() {
    Camera = new mgl::Camera(UBO_BP);

    glm::vec3 CamCenter = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 InitialCamPos = glm::vec3(5.0f, 5.0f, 5.0f);
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

    boxCamera = std::make_unique<mgl::OrbitCamera>(Camera, CamCenter, InitialCamPos, up);
    boxCamera->setProjection(Pesp_Pro);

    /* Setting default active camera to box camera */
    activeCamera = boxCamera.get();
}

/////////////////////////////////////////////////////////////////////////// FRAMEBUFFER

void MyApp::createFrameBuffers() {
    int width, height;
    glfwGetFramebufferSize(glfwGetCurrentContext(), &width, &height);

    frameBuffer = new FrameBuffer(width, height);
    frameBuffer->create();

    frameBufferShader = new mgl::ShaderProgram();
    frameBufferShader->addShader(GL_VERTEX_SHADER, "framebuffer-vs.glsl");
    frameBufferShader->addShader(GL_FRAGMENT_SHADER, "framebuffer-fs.glsl");

    frameBufferShader->addAttribute(mgl::POSITION_ATTRIBUTE, 0);
    frameBufferShader->addAttribute(mgl::TEXCOORD_ATTRIBUTE, 1);
    frameBufferShader->addUniform(mgl::SCREEN_TEXTUERE_SAMPLER);
    frameBufferShader->addUniform(mgl::GAMMA);
	frameBufferShader->addUniform(mgl::EXPOSURE);

    frameBufferShader->create();

    frameBufferShader->bind();
    glUniform1i(frameBufferShader->Uniforms[mgl::SCREEN_TEXTUERE_SAMPLER].index, mgl::SCREEN_TEXTURE_UNIT_INDEX);
    frameBufferShader->unbind();

    frameBuffer->setShaderProgram(frameBufferShader);
}

/////////////////////////////////////////////////////////////////////////// GUI

void MyApp::createGUI() {
	gui = new mgl::GUI();
	gui->initGUI();
	gui->enableKeyboardNavigationGUI(true);
}

/////////////////////////////////////////////////////////////////////////// DRAW

void MyApp::drawScene(double elapsed, bool opaquePass) {
    Shaders->bind();

    hdrSkybox->bindIrradianceMap(mgl::IRRADIANCE_TEXTURE_UNIT);
    hdrSkybox->bindBRDFLUTTexture(mgl::BRDDF_LUT_TEXTURE_UNIT);
    hdrSkybox->bindPrefilteredEnvMap(mgl::PREFILTERED_ENV_TEXTURE_UNIT);
    glUniform1i(Shaders->Uniforms[mgl::IRRADIANCE_SAMPLER].index, mgl::IRRADIANCE_UNIT_INDEX);
    glUniform1i(Shaders->Uniforms[mgl::PREFILTERED_ENV_SAMPLER].index, mgl::PREFILTERED_ENV_UNIT_INDEX);
    glUniform1i(Shaders->Uniforms[mgl::BRDDF_LUT_SAMPLER].index, mgl::BRDDF_LUT_UNIT_INDEX);

    /* Maybe change this to update the uniform inside OrbitCamera updateView function TODO() */
    glm::vec3 camPos = activeCamera->getPosition();
    glUniform3fv(Shaders->Uniforms[mgl::CAMERA_POSITION].index, 1, glm::value_ptr(camPos));

    sceneRoot->drawSceneGraph(glm::mat4(1.0f), opaquePass);

    Shaders->unbind();
}

/////////////////////////////////////////////////////////////////////////// UPDATE UNIFORMS

void MyApp::updateUnifroms() {
	frameBuffer->bind();
	frameBuffer->setExposure(gui->getState().exposure);
	frameBuffer->setGamma(gui->getState().gamma);
	frameBuffer->unbind();

	Shaders->bind();
    glUniform1i(Shaders->Uniforms[mgl::ENABLE_NORMAL_MAPPING].index, gui->getState().enableNormalMapping);
	glUniform1i(Shaders->Uniforms[mgl::ENABLE_AMBIENT_OCCLUSION].index, gui->getState().enableAmbientOcclusion);
	Shaders->unbind();
}

////////////////////////////////////////////////////////////////////// CALLBACKS

void MyApp::initCallback(GLFWwindow *win) {
  createMeshes();
  createShaderPrograms(); // after mesh;
  createSceneGraph();
  createCamera();
  createGUI();
  createFrameBuffers();
}

void MyApp::windowSizeCallback(GLFWwindow *win, int winx, int winy) {
  glViewport(0, 0, winx, winy);

  if (frameBuffer != nullptr) {
	  frameBuffer->resize(winx, winy);
  }

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

void MyApp::displayCallback(GLFWwindow* win, double elapsed) {
    gui->generateBuffersGUI();
	gui->renderWindowGUI();
	gui->processGlobalCallbacksGUI();

	updateUnifroms();

    frameBuffer->bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
    drawScene(elapsed, true);

    SkyboxShaders->bind();
    hdrSkybox->render(*activeCamera->getCamera());
    SkyboxShaders->unbind();

	drawScene(elapsed, false);

    frameBuffer->unbind();

    frameBuffer->render();

	gui->renderGUI();
}

void MyApp::cursorCallback(GLFWwindow* window, double xpos, double ypos) {
    if (gui->guiWantsKeyboard() || gui->guiWantsMouse()) return;

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
    if (gui->guiWantsKeyboard() || gui->guiWantsMouse()) return;

    if (key == GLFW_KEY_P) {
        if (action == GLFW_PRESS) {
            ortho_mode = !ortho_mode;
            ortho_mode ? activeCamera->setProjection(Ortho_Pro) : activeCamera->setProjection(Pesp_Pro);
        }
    }

    if (key == GLFW_KEY_O && action == GLFW_PRESS) {
		gui->enableWindow(!gui->getState().showWindow);
    }
}

void MyApp::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    if (gui->guiWantsKeyboard() || gui->guiWantsMouse()) return;

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
    if (gui->guiWantsKeyboard() || gui->guiWantsMouse()) return;
    //xoffset parece ser inutil para o nosso projeto
    //aproximar/afastar do centro da camera
    activeCamera->zoom(yoffset);
}

/////////////////////////////////////////////////////////////////////////// MAIN

int main(int argc, char *argv[]) {
  mgl::Engine &engine = mgl::Engine::getInstance();
  engine.setApp(new MyApp());
  engine.setOpenGL(4, 6);
  engine.setWindow(800, 600, "PBR", 0, 1);
  engine.init();
  engine.run();
  exit(EXIT_SUCCESS);
}

////////////////////////////////////////////////////////////////////////////////
