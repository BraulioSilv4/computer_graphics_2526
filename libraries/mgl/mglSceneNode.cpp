#include "mglSceneNode.hpp"
#include <glm/ext.hpp> // glm::value_ptr
#include <iostream>

namespace mgl {

	SceneNode::~SceneNode() {}

	void SceneNode::drawNodeMesh() {
		this->preDrawCallback(this);

		ShaderProgram* shaderProgram = SceneNode::getShaderProgram();
		if(shaderProgram) {
			GLint modelMatrixLocation = glGetUniformLocation(shaderProgram->ProgramId, MODEL_MATRIX);
			glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, glm::value_ptr(worldTransform));
			mesh->draw();
		}

		this->postDrawCallback(this);
	}

	void SceneNode::drawSceneGraph(const glm::mat4& parentTransform) {
		// Model Matrix
		worldTransform = parentTransform * localTransform;

		this->drawNodeMesh();

		for (const auto& child : this->children) {
			child->drawSceneGraph(worldTransform);
		}
	};

	void SceneNode::addChild(std::unique_ptr<SceneNode> child) {
		child->parent = this;
		children.push_back(std::move(child));
	}



	/* Relative Transformation Methods.
	* These methods change the node relative to its current values.
	*/
	void SceneNode::transformTranslate(const glm::vec3& transl) {
		translation += transl;
		updateLocalTransform();
	}

	void SceneNode::transformScale(const glm::vec3& scale) {
		scaling *= scale;
		updateLocalTransform();
	}

	void SceneNode::transformRotate(const glm::quat& q) {
		orientation = orientation * q;
		// preventing possible floating point errors
		orientation = glm::normalize(orientation);
		updateLocalTransform();
	}

	void SceneNode::transformRotate(float rads, const glm::vec3& norm_axis) {
		glm::quat q = glm::angleAxis(rads, norm_axis);
		this->transformRotate(q);
	}

	

	/* Absolute Transformation Methods.
	* These methods set the node values directly.
	*/
	void SceneNode::setPosition(const glm::vec3& position) {
		translation = position;
		updateLocalTransform();
	}

	void SceneNode::setScale(const glm::vec3& scale) {
		scaling = scale;
		updateLocalTransform();
	}

	void SceneNode::setRotation(const glm::quat& q) {
		orientation = q;
		std::cout << glm::to_string(q) << std::endl;
		updateLocalTransform();
	}

	void SceneNode::setRotation(float rads, const glm::vec3& norm_axis) {
		glm::quat q = glm::angleAxis(rads, norm_axis);
		this->setRotation(q);
	}


	bool SceneNode::hasShaderProgram() const {
		return shaderProgram != nullptr;
	}


	/* Getters and Setters */
	glm::vec3 SceneNode::getTranslation() const {
		return translation;
	}

	glm::vec3 SceneNode::getScale() const {
		return scaling;
	}

	glm::quat SceneNode::getOrientation() const {
		return orientation;
	}

	void SceneNode::setMesh(Mesh* m) {
		mesh = m;
	}
	Mesh* SceneNode::getMesh() const {
		return mesh;
	}

	glm::mat4 SceneNode::getLocalTransform() const {
		return localTransform;
	}

	void SceneNode::setShaderProgram(ShaderProgram* sp) {
		shaderProgram = sp;
	}
	ShaderProgram* SceneNode::getShaderProgram() const {
		if (hasShaderProgram()) {
			return shaderProgram;
		}
		else {
			SceneNode* p = this->parent;
			while (p != nullptr) {
				if (p->hasShaderProgram()) {
					return p->shaderProgram;
				}
				p = p->parent;
			}
		}
	}

	std::string SceneNode::getID() const {
		return name;
	};
} // namespace mgl