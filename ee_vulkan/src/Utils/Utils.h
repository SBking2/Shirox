#pragma once
#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtc/quaternion.hpp>
#include <assimp/scene.h>
namespace ev
{
	class Utils final
	{
	public:
		static glm::mat4 Ai2GlmMat4(const aiMatrix4x4& ai_matrix);
		static glm::vec3 Ai2GlmVec3(const aiVector3D& vec);
		static glm::quat Ai2GlmQuat(const aiQuaternion& quat);
	};
}