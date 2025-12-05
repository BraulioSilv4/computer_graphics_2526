#ifndef MGL_SCENENODE_HPP
#define MGL_SCENENODE_HPP

#include <vector>
#include <memory>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "mglMesh.hpp"
#include "mglShader.hpp"
#include "mglConventions.hpp"

namespace mgl {

    class SceneNode {
    protected:
        SceneNode* parent = nullptr;
		std::vector<std::unique_ptr<SceneNode>> children;

    public:
        using NodeCallback = std::function<void(SceneNode*)>;

        Mesh* mesh;
        glm::mat4* modelMatrix;
        mgl::ShaderProgram* shaderProgram = nullptr;
        NodeCallback preDrawCallback = [](SceneNode* n) {};
        NodeCallback postDrawCallback = [](SceneNode* n) {};

        SceneNode(Mesh* _mesh, glm::mat4* _modelMatrix, mgl::ShaderProgram* _shaderProgram = nullptr) {
            mesh = _mesh;
            modelMatrix = _modelMatrix;
			shaderProgram = _shaderProgram;
        }

        ~SceneNode();

        void drawNodeMesh();

        void drawSceneGraph();

        void addChild(std::unique_ptr<SceneNode> child);
        
        bool hasShaderProgram() const;
        
        /* Getters and Setters */
        void setMesh(Mesh* m);
		Mesh* getMesh() const;

		void setModelMatrix(glm::mat4* mm);
		glm::mat4* getModelMatrix() const;

        void setShaderProgram(ShaderProgram* sp);
        ShaderProgram* getShaderProgram() const;  
    };
}

#endif // !MGL_SCENENODE_HPP
