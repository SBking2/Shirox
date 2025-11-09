#pragma once
namespace srx
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