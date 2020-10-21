#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>

class OrbitCamera
{
public:
	OrbitCamera();
	glm::mat4 getViewMatrix() const {
		return glm::lookAt(position, target, up);
	}
	float getFOV() const { return FOV; }
	void setFOV(float fov) { FOV = fov; }
	const glm::vec3& getLook() { return look; }
	const glm::vec3& getRight() { return right; }
	const glm::vec3& getUp() { return up; }
	const glm::vec3& getPosition() { return position; }
	void rotate(float yaw, float pitch); //in degrees
	void setLookAt(const glm::vec3& target) { this->target = target; }
	void setRadius(float radius) 
	{ 
		this->radius = glm::clamp(radius, 2.0f, 80.0f);
	}
private:
	void updateCameraVectors();

	float pitch, yaw; //euler angler (in radians)
	float FOV; //in degrees
	glm::vec3 position, target, up, look, right;
	const glm::vec3 WORLD_UP{ 0.0f, 1.0f, 0.0f };
	float radius;
};
