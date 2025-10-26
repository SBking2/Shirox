#include "Utils.h"
#include <glm/gtx/quaternion.hpp> 
namespace ev
{
	glm::mat4 Utils::Ai2GlmMat4(const aiMatrix4x4& ai_matrix)
	{
		glm::mat4 result =
		{
			ai_matrix.a1, ai_matrix.a2, ai_matrix.a3, ai_matrix.a4,
			ai_matrix.b1, ai_matrix.b2, ai_matrix.b3, ai_matrix.b4,
			ai_matrix.c1, ai_matrix.c2, ai_matrix.c3, ai_matrix.c4,
			ai_matrix.d1, ai_matrix.d2, ai_matrix.d3, ai_matrix.d4
		};
		return glm::transpose(result);
	}
	glm::vec3 Utils::Ai2GlmVec3(const aiVector3D& vec)
	{
		return glm::vec3(vec.x, vec.y, vec.z);
	}
	glm::quat Utils::Ai2GlmQuat(const aiQuaternion& quat)
	{
		return glm::quat(quat.w, quat.x, quat.y, quat.z);
	}
}