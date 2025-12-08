#include "mglOrbitCamera.hpp"


namespace mgl{


void OrbitCamera::rotate(float dx, float dy) {
	if (dx == 0 && dy == 0) return;

	glm::vec3 forward = glm::normalize(position - center);
	glm::vec3 right = glm::normalize(glm::cross(forward, up));

	glm::quat rotX = glm::angleAxis(glm::radians(dx * sensitivity), up);
	glm::quat rotY = glm::angleAxis(glm::radians(dy * sensitivity), right);

	position = center + (rotY * rotX * (position - center));

	updateView();
}

void OrbitCamera::move(float dx, float dy) {
	glm::vec3 forward = glm::normalize(position - center);
	glm::vec3 right = glm::normalize(glm::cross(forward, up));

	/* Move more slowly when we are close to the center of the camera */
	float moveSpeed = moveSpeedFactor * glm::length(position - center);

	glm::vec3 offsetHorizontal = (dx * moveSpeed * right);
	glm::vec3 offsetVertical = (-dy * moveSpeed * forward);

	/* Summing only the vertical offset to the position to zoom into the center of orbit */
	position += offsetHorizontal + offsetVertical;
	center += offsetHorizontal;

	updateView();
}

void OrbitCamera::zoom(float offset) {
	glm::vec3 forward = glm::normalize(position - center);
	position -= offset * zoomSpeedFactor * forward;
	updateView();
}

void OrbitCamera::setTarget(const glm::vec3& _center, const glm::vec3& _position) {
	center = _center;
	position = _position;
	updateView();
}

void OrbitCamera::setCenter(glm::vec3 _center) {
	center = _center;
}

void OrbitCamera::setProjection(const glm::mat4& proj) {
	camera->setProjectionMatrix(proj);
}

void OrbitCamera::updateView() {
	camera->setViewMatrix(glm::lookAt(position, center, up));
}

glm::vec3 OrbitCamera::getPosition() const {
	return position;
}

glm::vec3 OrbitCamera::getCenter() const {
	return center;
}

}