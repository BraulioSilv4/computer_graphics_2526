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
  glm::vec4 LightColor = glm::vec4(20.0f, 10.0f, 10.0f, 1.0f);
  glm::vec3 LightPosition = glm::vec3(3.0f, 0.0f, 3.0f);

  const GLuint UBO_BP = 0;
  mgl::Camera *Camera = nullptr;
  std::unique_ptr<mgl::OrbitCamera> boxCamera;
  mgl::OrbitCamera* activeCamera = nullptr;
  GLint ModelMatrixId;
  mgl::Manager<mgl::Mesh, std::string> MeshManager;
  mgl::Registry<std::string, mgl::SceneNode*> NodeRegistry;

  // debug shader to visualize arbitrary textures (BRDF LUT)
  mgl::ShaderProgram* debugShader = nullptr;
  bool showBRDF = false; // toggle display of BRDF LUT
  
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

  void createCamera();
  void createMeshes();
  void createSceneGraph();
  void createShaderPrograms();
  void createFrameBuffers();
  void drawScene(double elapsed);
};

///////////////////////////////////////////////////////////////////////// MESHES

void MyApp::createMeshes() {
    std::string mesh_path = "..\\assets\\models\\Sphere\\metal_sphere.obj";
    mgl::Mesh* m = new mgl::Mesh();
    m->generateSmoothNormals();
    m->calculateTangentSpace();
    m->joinIdenticalVertices();
    m->flipUVs();
    m->create(mesh_path);

    std::string brick_path = "..\\assets\\models\\Sphere\\plastic_sphere.obj";
    mgl::Mesh* brick = new mgl::Mesh();
    brick->generateSmoothNormals();
    brick->calculateTangentSpace();
    brick->joinIdenticalVertices();
    brick->flipUVs();
    brick->create(brick_path);

	std::string plastic_path = "..\\assets\\models\\Sphere\\moss.obj";
	mgl::Mesh* plastic = new mgl::Mesh();
	plastic->generateSmoothNormals();
	plastic->calculateTangentSpace();
	plastic->joinIdenticalVertices();
	plastic->flipUVs();
	plastic->create(plastic_path);

    MeshManager.add(std::unique_ptr<mgl::Mesh>(m));
	MeshManager.add(std::unique_ptr<mgl::Mesh>(brick));
	MeshManager.add(std::unique_ptr<mgl::Mesh>(plastic));

	hdrSkybox = new HDRSkybox("..\\assets\\HDR\\pine_attic_8k.hdr", nullptr);
}

///////////////////////////////////////////////////////////////////////// SHADER

void MyApp::createShaderPrograms() {
  SkyboxShaders = new mgl::ShaderProgram();
  SkyboxShaders->addShader(GL_VERTEX_SHADER, "skybox-vs.glsl");
  SkyboxShaders->addShader(GL_FRAGMENT_SHADER, "skybox-fs.glsl");

  SkyboxShaders->addAttribute(mgl::POSITION_ATTRIBUTE, mgl::Mesh::POSITION);
  SkyboxShaders->addUniform(mgl::EQUIRECTANGULAR_SAMPLER);
  SkyboxShaders->addUniform(mgl::CUBEMAP_SAMPLER);
  SkyboxShaders->addUniform(mgl::PROJECTION_MATRIX);
  SkyboxShaders->addUniform(mgl::VIEW_MATRIX);

  SkyboxShaders->create();

  SkyboxShaders->bind();
  glUniform1i(SkyboxShaders->Uniforms[mgl::CUBEMAP_SAMPLER].index, mgl::CUBEMAP_UNIT_INDEX);
  glUniform1i(SkyboxShaders->Uniforms[mgl::EQUIRECTANGULAR_SAMPLER].index, mgl::EQUIRECTANGULAR_UNIT_INDEX);
  glUniform1i(SkyboxShaders->Uniforms[mgl::CUBEMAP_SAMPLER].index, mgl::CUBEMAP_UNIT_INDEX);
  SkyboxShaders->unbind();

  hdrSkybox->setShaderProgram(SkyboxShaders);
  hdrSkybox->init(); // loads the HDRI texture and creates the cubemap

  Shaders = new mgl::ShaderProgram();
  Shaders->addShader(GL_VERTEX_SHADER, "cube-vs.glsl");
  Shaders->addShader(GL_FRAGMENT_SHADER, "pbr-fs.glsl");

  Shaders->addAttribute(mgl::POSITION_ATTRIBUTE, mgl::Mesh::POSITION);
  if (MeshManager.get("..\\assets\\models\\Sphere\\metal_sphere.obj")->hasNormals()) {
    Shaders->addAttribute(mgl::NORMAL_ATTRIBUTE, mgl::Mesh::NORMAL);
  }
  if (MeshManager.get("..\\assets\\models\\Sphere\\metal_sphere.obj")->hasTexcoords()) {
    Shaders->addAttribute(mgl::TEXCOORD_ATTRIBUTE, mgl::Mesh::TEXCOORD);
  }
  if (MeshManager.get("..\\assets\\models\\Sphere\\metal_sphere.obj")->hasTangentsAndBitangents()) {
    Shaders->addAttribute(mgl::TANGENT_ATTRIBUTE, mgl::Mesh::TANGENT);
  }

  Shaders->addUniform(mgl::DIFFUSE_SAMPLER);
  Shaders->addUniform(mgl::NORMAL_SAMPLER);
  Shaders->addUniform(mgl::METAL_SAMPLER);
  Shaders->addUniform(mgl::ROUGH_SAMPLER);
  Shaders->addUniform(mgl::CUBEMAP_SAMPLER);
  Shaders->addUniform(mgl::IRRADIANCE_SAMPLER);
  Shaders->addUniform(mgl::PREFILTERED_ENV_SAMPLER);
  Shaders->addUniform(mgl::BRDDF_LUT_SAMPLER);


  Shaders->addUniform(mgl::MODEL_MATRIX);
  Shaders->addUniform(mgl::CAMERA_POSITION);
  Shaders->addUniformBlock(mgl::CAMERA_BLOCK, UBO_BP);
  Shaders->create();

  /* Assigning texture units to samplers */
  Shaders->bind();
  glUniform1i(Shaders->Uniforms[mgl::DIFFUSE_SAMPLER].index, mgl::ALBEDO_UNIT_INDEX);
  glUniform1i(Shaders->Uniforms[mgl::ROUGH_SAMPLER].index, mgl::ROUGHNESS_UNIT_INDEX);
  glUniform1i(Shaders->Uniforms[mgl::METAL_SAMPLER].index, mgl::METALLIC_UNIT_INDEX);
  glUniform1i(Shaders->Uniforms[mgl::NORMAL_SAMPLER].index, mgl::NORMAL_UNIT_INDEX);
  glUniform1i(Shaders->Uniforms[mgl::CUBEMAP_SAMPLER].index, mgl::CUBEMAP_UNIT_INDEX);

  hdrSkybox->bindIrradianceMap(mgl::IRRADIANCE_TEXTURE_UNIT);
  hdrSkybox->bindBRDFLUTTexture(mgl::BRDDF_LUT_TEXTURE_UNIT);
  hdrSkybox->bindPrefilteredEnvMap(mgl::PREFILTERED_ENV_TEXTURE_UNIT);
  glUniform1i(Shaders->Uniforms[mgl::IRRADIANCE_SAMPLER].index, mgl::IRRADIANCE_UNIT_INDEX);
  glUniform1i(Shaders->Uniforms[mgl::BRDDF_LUT_SAMPLER].index, mgl::BRDDF_LUT_UNIT_INDEX);
  glUniform1i(Shaders->Uniforms[mgl::PREFILTERED_ENV_SAMPLER].index, mgl::PREFILTERED_ENV_UNIT_INDEX);
 
  Shaders->unbind();

  ModelMatrixId = Shaders->Uniforms[mgl::MODEL_MATRIX].index;


  debugShader = new mgl::ShaderProgram();
  debugShader->addShader(GL_VERTEX_SHADER, "framebuffer-vs.glsl");
  debugShader->addShader(GL_FRAGMENT_SHADER, "show-texture-fs.glsl");
  debugShader->addAttribute(mgl::POSITION_ATTRIBUTE, 0);
  debugShader->addAttribute(mgl::TEXCOORD_ATTRIBUTE, 1);
  debugShader->addUniform("tex"); // we'll set this to the BRDF LUT sampler unit
  debugShader->create();

  debugShader->bind();
  glUniform1i(debugShader->Uniforms["tex"].index, mgl::BRDDF_LUT_UNIT_INDEX);
  debugShader->unbind();
}

///////////////////////////////////////////////////////////////////////// SCENE GRAPH

void MyApp::createSceneGraph() {
    std::string rootName = "cube_with_materials.obj";

    auto root = std::make_unique<mgl::SceneNode>(
        rootName,
        MeshManager.get("..\\assets\\models\\Sphere\\metal_sphere.obj"),
        Shaders
    );

    auto brickChild = std::make_unique<mgl::SceneNode>(
        "brick_sphere.obj",
        MeshManager.get("..\\assets\\models\\Sphere\\plastic_sphere.obj"),
        Shaders
    );

    auto plasticChild = std::make_unique<mgl::SceneNode>(
        "plastic_sphere.obj",
        MeshManager.get("..\\assets\\models\\Sphere\\moss.obj"),
        Shaders
	);

	NodeRegistry.add("moss.obj", plasticChild.get());
    NodeRegistry.add("plastic_sphere.obj", brickChild.get());
	root->addChild(std::move(brickChild));
	root->addChild(std::move(plasticChild));

    sceneRoot = std::move(root);
    NodeRegistry.add(mgl::CUBE, sceneRoot.get());
	NodeRegistry.get("plastic_sphere.obj")->setPosition(glm::vec3(2.0f, 0.0f, 0.0f));
	NodeRegistry.get("moss.obj")->setPosition(glm::vec3(-2.0f, 0.0f, 0.0f));
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

/////////////////////////////////////////////////////////////////////////// DRAW


void MyApp::drawScene(double elapsed) {
    if (!showBRDF) {
        Shaders->bind();
        /* Maybe change this to update the uniform inside OrbitCamera updateView function TODO() */
        glm::vec3 camPos = activeCamera->getPosition();
        glUniform3fv(Shaders->Uniforms[mgl::CAMERA_POSITION].index, 1, glm::value_ptr(camPos));

        sceneRoot->drawSceneGraph();
        sceneRoot->transformRotate(glm::radians(0.2f), glm::vec3(0.0f, 1.0f, 0.0f));

        Shaders->unbind();

        SkyboxShaders->bind();
        hdrSkybox->render(*activeCamera->getCamera());
        SkyboxShaders->unbind();
    }
    else {
        // Debug mode: show BRDF LUT on-screen
        // Ensure BRDF LUT texture is bound to its texture unit first
        hdrSkybox->bindBRDFLUTTexture(mgl::BRDDF_LUT_TEXTURE_UNIT);

        debugShader->bind();
        // hdrSkybox provides a drawQuad() that draws the fullscreen quad (it was used to bake the LUT)
        hdrSkybox->drawQuad();
        debugShader->unbind();
    }
}

////////////////////////////////////////////////////////////////////// CALLBACKS

void MyApp::initCallback(GLFWwindow *win) {
  createMeshes();
  createShaderPrograms(); // after mesh;
  createSceneGraph();
  createCamera();
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
    frameBuffer->bind();
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

    drawScene(elapsed);

    frameBuffer->unbind();

    frameBuffer->render();
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

    if (key == GLFW_KEY_L && action == GLFW_PRESS) {
        showBRDF = !showBRDF;
    }

    if (key == GLFW_KEY_0 && action == GLFW_PRESS) {
		float gamma = frameBuffer->getGamma();
		frameBuffer->setGamma(gamma + 0.1f);
		std::cout << "Gamma: " << frameBuffer->getGamma() << std::endl;
    }
    if (key == GLFW_KEY_9 && action == GLFW_PRESS) {
		float gamma = frameBuffer->getGamma();
		gamma -= 0.1f;
		frameBuffer->setGamma(gamma);
        if (gamma < 0.0f) gamma = 0.1f;
		std::cout << "Gamma: " << gamma << std::endl;
    }
	if (key == GLFW_KEY_8 && action == GLFW_PRESS) {
        float exposure = frameBuffer->getExposure();
        frameBuffer->setExposure(exposure + 0.1f);
		std::cout << "Exposure: " << frameBuffer->getExposure() << std::endl;
    }
    if (key == GLFW_KEY_7 && action == GLFW_PRESS) {
        float exposure = frameBuffer->getExposure();
        exposure -= 0.1f;
        frameBuffer->setExposure(exposure);
        if (exposure < 0.0f) exposure = 0.1f;
        std::cout << "Exposure: " << exposure << std::endl;
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
  engine.setWindow(800, 600, "PBR", 0, 1);
  engine.init();
  engine.run();
  exit(EXIT_SUCCESS);
}

////////////////////////////////////////////////////////////////////////////////
