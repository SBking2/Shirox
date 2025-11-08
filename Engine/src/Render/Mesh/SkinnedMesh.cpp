#include "SkinnedMesh.h"
#include "Core/Utils/Utils.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <stdexcept>
#include <glm/gtx/quaternion.hpp>
namespace srx
{
	void SkinnedMesh::LoadMesh(const aiScene* scene, aiMesh* mesh)
	{
		_scene = scene;
		_global_inverse_transform = glm::inverse(Utils::Ai2GlmMat4(_scene->mRootNode->mTransformation));

		if (mesh == nullptr)
		{
			throw std::runtime_error("failed to load mesh!");
		}

		_vertices.resize(mesh->mNumVertices);
		for (int i = 0; i < mesh->mNumVertices; i++)
		{
			_vertices[i].position = glm::vec3(
				mesh->mVertices[i].x,
				mesh->mVertices[i].y,
				mesh->mVertices[i].z
			);
			_vertices[i].color = glm::vec3(1.0f);
			if (mesh->mTextureCoords[0])
			{
				_vertices[i].tex_coord.x = mesh->mTextureCoords[0][i].x;
				_vertices[i].tex_coord.y = 1.0f - mesh->mTextureCoords[0][i].y;
			}
		}

		_indices.clear();
		for (int i = 0; i < mesh->mNumFaces; i++)
		{
			for (int j = 0; j < mesh->mFaces[i].mNumIndices; j++)
			{
				_indices.emplace_back(mesh->mFaces[i].mIndices[j]);
			}
		}

		_bone_mapping.clear();
		_bone_infos.clear();
		for (int i = 0; i < mesh->mNumBones; i++)
		{
			aiBone* bone = mesh->mBones[i];

			std::string bone_name = bone->mName.C_Str();

			if (_bone_mapping.find(bone_name) == _bone_mapping.end())
			{
				_bone_mapping[bone_name] = _bone_infos.size();
				_bone_infos.emplace_back(Utils::Ai2GlmMat4(bone->mOffsetMatrix));
			}

			for (int j = 0; j < bone->mNumWeights; j++)
			{
				AddVertexBoneWeight(bone->mWeights[j].mVertexId
					, _bone_mapping[bone->mName.C_Str()], bone->mWeights[j].mWeight);
			}
		}
	}
	void SkinnedMesh::LoadAnimation(aiAnimation* anim)
	{
		_anim = anim;
	}
	void SkinnedMesh::UpdateAnimation(float time, std::vector<glm::mat4>& bone_transforms)
	{
		bone_transforms.resize(_bone_infos.size());

		float ticks_per_second = _anim->mTicksPerSecond;
		float time_in_ticks = time * ticks_per_second;
		float animation_time = fmod(time_in_ticks, _anim->mDuration);

		glm::mat4 parent = glm::mat4(1.0f);
		UpdateBoneTransform(animation_time, _scene->mRootNode, parent);

		for (int i = 0; i < _bone_infos.size(); i++)
			bone_transforms[i] = _bone_infos[i].final_transform;
	}

	static glm::mat4 interpolate_translation(float time, const aiNodeAnim* pNodeAnim)
	{
		aiVector3D translation;

		if (pNodeAnim->mNumPositionKeys == 1)
		{
			translation = pNodeAnim->mPositionKeys[0].mValue;
		}
		else
		{
			uint32_t frameIndex = 0;
			for (uint32_t i = 0; i < pNodeAnim->mNumPositionKeys - 1; i++)
			{
				if (time < (float)pNodeAnim->mPositionKeys[i + 1].mTime)
				{
					frameIndex = i;
					break;
				}
			}

			aiVectorKey currentFrame = pNodeAnim->mPositionKeys[frameIndex];
			aiVectorKey nextFrame = pNodeAnim->mPositionKeys[(frameIndex + 1) % pNodeAnim->mNumPositionKeys];

			float delta = (time - (float)currentFrame.mTime) / (float)(nextFrame.mTime - currentFrame.mTime);

			const aiVector3D& start = currentFrame.mValue;
			const aiVector3D& end = nextFrame.mValue;

			translation = (start + delta * (end - start));
		}

		aiMatrix4x4 mat;
		aiMatrix4x4::Translation(translation, mat);
		return Utils::Ai2GlmMat4(mat);
	}
	static glm::mat4 interpolate_rotation(float time, const aiNodeAnim* pNodeAnim)
	{
		aiQuaternion rotation;

		if (pNodeAnim->mNumRotationKeys == 1)
		{
			rotation = pNodeAnim->mRotationKeys[0].mValue;
		}
		else
		{
			uint32_t frameIndex = 0;
			for (uint32_t i = 0; i < pNodeAnim->mNumRotationKeys - 1; i++)
			{
				if (time < (float)pNodeAnim->mRotationKeys[i + 1].mTime)
				{
					frameIndex = i;
					break;
				}
			}

			aiQuatKey currentFrame = pNodeAnim->mRotationKeys[frameIndex];
			aiQuatKey nextFrame = pNodeAnim->mRotationKeys[(frameIndex + 1) % pNodeAnim->mNumRotationKeys];

			float delta = (time - (float)currentFrame.mTime) / (float)(nextFrame.mTime - currentFrame.mTime);

			const aiQuaternion& start = currentFrame.mValue;
			const aiQuaternion& end = nextFrame.mValue;

			aiQuaternion::Interpolate(rotation, start, end, delta);
			rotation.Normalize();
		}

		aiMatrix4x4 mat(rotation.GetMatrix());
		return Utils::Ai2GlmMat4(mat);
	}
	static glm::mat4 interpolate_scale(float time, const aiNodeAnim* pNodeAnim)
	{
		aiVector3D scale;

		if (pNodeAnim->mNumScalingKeys == 1)
		{
			scale = pNodeAnim->mScalingKeys[0].mValue;
		}
		else
		{
			uint32_t frameIndex = 0;
			for (uint32_t i = 0; i < pNodeAnim->mNumScalingKeys - 1; i++)
			{
				if (time < (float)pNodeAnim->mScalingKeys[i + 1].mTime)
				{
					frameIndex = i;
					break;
				}
			}

			aiVectorKey currentFrame = pNodeAnim->mScalingKeys[frameIndex];
			aiVectorKey nextFrame = pNodeAnim->mScalingKeys[(frameIndex + 1) % pNodeAnim->mNumScalingKeys];

			float delta = (time - (float)currentFrame.mTime) / (float)(nextFrame.mTime - currentFrame.mTime);

			const aiVector3D& start = currentFrame.mValue;
			const aiVector3D& end = nextFrame.mValue;

			scale = (start + delta * (end - start));
		}

		aiMatrix4x4 mat;
		aiMatrix4x4::Scaling(scale, mat);
		return Utils::Ai2GlmMat4(mat);
	}

	void SkinnedMesh::UpdateBoneTransform(float anim_time, const aiNode* node, const glm::mat4& parent)
	{
		std::string name(node->mName.C_Str());
		glm::mat4 node_transform(Utils::Ai2GlmMat4(node->mTransformation));
		const aiNodeAnim* anim_node = FindAnimNode(name);
		if (anim_node != nullptr)
		{
			glm::mat4 translation_matrx = interpolate_translation(anim_time, anim_node);
			glm::mat4 rotation_matrx = interpolate_rotation(anim_time, anim_node);
			glm::mat4 scale_matrx = interpolate_scale(anim_time, anim_node);

			node_transform = translation_matrx * rotation_matrx * scale_matrx;
		}

		glm::mat4 global_transform = parent * node_transform;

		if (_bone_mapping.find(name) != _bone_mapping.end())
		{
			uint32_t index = _bone_mapping[name];
			_bone_infos[index].final_transform =
				_global_inverse_transform * global_transform * _bone_infos[index].offset_matrix;
		}

		for (int i = 0; i < node->mNumChildren; i++)
			UpdateBoneTransform(anim_time, node->mChildren[i], global_transform);
	}

	aiNodeAnim* SkinnedMesh::FindAnimNode(const std::string& name)
	{
		for (int i = 0; i < _anim->mNumChannels; i++)
		{
			if (name == _anim->mChannels[i]->mNodeName.C_Str())
				return _anim->mChannels[i];
		}
		return nullptr;
	}

	void SkinnedMesh::AddVertexBoneWeight(uint32_t index, uint32_t bone_id, float weight)
	{
		for (uint32_t i = 0; i < 4; i++)
		{
			if (_vertices[index].weights[i] == 0.0f)
			{
				_vertices[index].bone_ids[i] = bone_id;
				_vertices[index].weights[i] = weight;
				return;
			}
		}
	}
}
