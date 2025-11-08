#pragma once
#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
namespace ev
{
	class Camera final
	{
	public:
		Camera() = default;
		glm::mat4 GetViewMatrix();
		glm::vec3 position;
		glm::quat rotation;
	};
}