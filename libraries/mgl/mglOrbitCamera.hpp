#ifndef MGL_ORBIT_CAMERA
#define MGL_ORBIT_CAMERA

#include "mglCamera.hpp"
#include <iostream>

namespace mgl {

	class OrbitCamera {
	private:
		mgl::Camera* camera;
		glm::vec3 center;
		glm::vec3 position;
		glm::vec3 up;

		float sensitivity = 0.25f;
		float zoomSpeedFactor = 0.25f;
		float moveSpeedFactor = 0.001;

	public:
		OrbitCamera(mgl::Camera* _camera, glm::vec3 _center, glm::vec3 _position, glm::vec3 _up) {
			camera = _camera;
			center = _center;
			position = _position;
			up = _up;
			updateView();
		}

		void rotate(float dx, float dy);

		void move(float dx, float dy);

		void zoom(float offset);

		/* Getters and setters */
		void setTarget(const glm::vec3& _center, const glm::vec3& _position);

		void setCenter(glm::vec3 _center);

		void setProjection(const glm::mat4& proj);

		void updateView();

		glm::vec3 getPosition() const;
		glm::vec3 getCenter() const;
	};

}

#endif // !MGL_ORBIT_CAMERA
