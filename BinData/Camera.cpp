#include "Camera.h"

static constexpr float DEF_FOV = 45.0f;

OrbitCamera::OrbitCamera()
	: position(glm::vec3(0.0f, 0.0f, 0.0f)),
	target(glm::vec3(0.0f, 0.0f, 0.0f)),
	up(glm::vec3(0.0f, 1.0f, 0.0f)),
	right(glm::vec3(0.0f, 0.0f, 0.0f)),
	yaw(0.0f), pitch(0.0f), FOV(DEF_FOV), radius(10.0f)
{
}

void OrbitCamera::rotate(float yaw, float pitch)
{
	this->yaw = glm::radians(yaw);
	this->pitch = glm::radians(pitch);

	this->pitch = glm::clamp(this->pitch, -glm::pi<float>() / 2.0f + 0.1f,
		glm::pi<float>() / 2.0f - 0.1f);
	updateCameraVectors();
}

void OrbitCamera::updateCameraVectors()
{
	position.x = target.x + radius * cosf(pitch) * sinf(yaw);
	position.y = target.y + radius * sinf(pitch);
	position.z = target.z + radius * cosf(pitch) * cosf(yaw);
}
