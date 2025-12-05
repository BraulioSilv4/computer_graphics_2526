#include "mglSceneNode.hpp"
#include <glm/ext.hpp> // glm::value_ptr

#include <iostream>
namespace mgl {

	SceneNode::~SceneNode() {}

	void SceneNode::drawNodeMesh() {
		this->preDrawCallback(this);

		ShaderProgram* shaderProgram = SceneNode::getShaderProgram();
		GLint modelMatrixLocation = glGetUniformLocation(shaderProgram->ProgramId, MODEL_MATRIX);
		glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, glm::value_ptr(*modelMatrix));
		mesh->draw();

		this->postDrawCallback(this);
	}

	void SceneNode::drawSceneGraph() {
		drawNodeMesh();
		for (const auto& child : children) {
			child->drawSceneGraph();
		}
	};

	void SceneNode::addChild(std::unique_ptr<SceneNode> child) {
		child->parent = this;
		children.push_back(std::move(child));
	}

	bool SceneNode::hasShaderProgram() const {
		return shaderProgram != nullptr;
	}

	/* Getters and Setters */
	void SceneNode::setMesh(Mesh* m) {
		mesh = m;
	}
	Mesh* SceneNode::getMesh() const {
		return mesh;
	}

	void SceneNode::setModelMatrix(glm::mat4* mm) {
		modelMatrix = mm;
	}
	glm::mat4* SceneNode::getModelMatrix() const {
		return modelMatrix;
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

} // namespace mgl