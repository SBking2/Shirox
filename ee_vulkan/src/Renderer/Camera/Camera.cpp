#include "Camera.h"
#include "Utils/Utils.h"
#include <GLM/gtx/quaternion.hpp>
namespace ev
{
	glm::mat4 Camera::GetViewMatrix()
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::toMat4(rotation);

		return glm::inverse(transform);
	}
}