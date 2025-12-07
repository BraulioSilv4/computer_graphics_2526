#ifndef MGL_SCENENODE_HPP
#define MGL_SCENENODE_HPP

#define GLM_ENABLE_EXPERIMENTAL

#include <vector>
#include <memory>
#include <string>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/quaternion.hpp> 
#include <glm/gtx/quaternion.hpp>

#include "mglInstanceManager.hpp"
#include "mglConventions.hpp"
#include "mglShader.hpp"
#include "mglMesh.hpp"

namespace mgl {
    class SceneNode : public IManageable<std::string> {
    private:
        SceneNode* parent = nullptr;
		std::vector<std::unique_ptr<SceneNode>> children;
		
        // Local transformation relative to parent
        glm::mat4 localTransform = glm::mat4(1.0f);

        // Transformation to world coordinates
		glm::mat4 worldTransform = glm::mat4(1.0f);

        // Absolute values
        glm::vec3 translation = glm::vec3(0.0f);
        glm::vec3 scaling = glm::vec3(1.0f);
        glm::quat orientation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);

        void updateLocalTransform() {
            glm::mat4 t = glm::translate(glm::mat4(1.0f), translation);
            glm::mat4 r = glm::toMat4(orientation);
            glm::mat4 s = glm::scale(glm::mat4(1.0f), scaling);

            localTransform = t * r * s;
        }

    public:
        using NodeCallback = std::function<void(SceneNode*)>;

        std::string name;
        mgl::Mesh* mesh;
        mgl::ShaderProgram* shaderProgram = nullptr;
        NodeCallback preDrawCallback = [](SceneNode* n) {};
        NodeCallback postDrawCallback = [](SceneNode* n) {};

        SceneNode(std::string _name, mgl::Mesh* _mesh, mgl::ShaderProgram* _shaderProgram = nullptr) {
            name = _name;
            mesh = _mesh;
			shaderProgram = _shaderProgram;
        }

        ~SceneNode();

        void drawNodeMesh();

        //                                                  when root parent is world (world = eye * local)
        void drawSceneGraph(const glm::mat4& parentTransform = glm::mat4(1.0f));

		/* Child Management Methods */
        void addChild(std::unique_ptr<SceneNode> child);
      
        bool hasShaderProgram() const;
        
        /* Relative Transformation Methods.
        * These methods change the node relative to its current values.
        */
		void transformTranslate(const glm::vec3& transl);
		void transformScale(const glm::vec3& scale);
		void transformRotate(const glm::quat& q);
        void transformRotate(float rads, const glm::vec3& norm_axis);

        /* Getters and Setters */
        
        /* Absolute Transformation Methods.
        * These methods set the node values directly.
        */
        void setPosition(const glm::vec3& position);
        void setScale(const glm::vec3& scale);
        void setRotation(const glm::quat& q);
        void setRotation(float rads, const glm::vec3& norm_axis);

        glm::vec3 getTranslation() const;
        glm::vec3 getScale() const;
        glm::quat getOrientation() const;

        void setMesh(mgl::Mesh* m);
		mgl::Mesh* getMesh() const;

		glm::mat4 getLocalTransform() const;

        void setShaderProgram(mgl::ShaderProgram* sp);
        mgl::ShaderProgram* getShaderProgram() const; 

        std::string getID() const override;
    };
}

#endif // !MGL_SCENENODE_HPP
