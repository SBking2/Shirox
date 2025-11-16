#pragma once
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
namespace srx
{
	class SkinnedMesh
	{
	public:
		void LoadMesh(const aiScene* scene, aiMesh* mesh);
		void LoadAnimation(aiAnimation* anim);
		void UpdateAnimation(float time, std::vector<glm::mat4>& bone_transforms);
		inline uint32_t GetVerticesSize() { return _vertices.size(); }
		inline void* GetVerticesData() { return _vertices.data(); }
		inline uint32_t GetIndicesSize() { return _indices.size(); }
		inline void* GetIndicesData() { return _indices.data(); }
	private:
		void UpdateBoneTransform(float anim_time, const aiNode* node, const glm::mat4& parent);
		aiNodeAnim* FindAnimNode(const std::string& name);
		void AddVertexBoneWeight(uint32_t index, uint32_t bone_id, float weight);
	public:
		struct Vertex
		{
			glm::vec3 position;
			glm::vec3 color;
			glm::vec2 tex_coord;
			glm::ivec4 bone_ids;
			glm::vec4 weights;
		};
	private:
		struct BoneInfo
		{
			glm::mat4 offset_matrix;
			glm::mat4 final_transform;
			BoneInfo(const glm::mat4& offset) :offset_matrix(offset), final_transform() {  }
		};

	private:
		std::unordered_map<std::string, uint32_t> _bone_mapping;
		std::vector<BoneInfo> _bone_infos;
		glm::mat4 _global_inverse_transform;
		std::vector<Vertex> _vertices;
		std::vector<uint32_t> _indices;

		const aiScene* _scene;
		aiAnimation* _anim;
	};
}