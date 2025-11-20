#pragma once
#include "Renderer/RendererAPI.h"
#include "VulkanContext.h"
#include "VulkanSwapchain.h"
namespace srx
{
	class VulkanRenderer final : public RendererAPI
	{
	public:
		////////////////////////////////////////////////// 开发使用，不是最终代码 ///////////////////////////////////////////////////////
		struct Vertex
		{
			glm::vec3 position;
			glm::vec3 color;
		};
		struct RenderData
		{
			std::vector<Vertex> vertices =
			{
				{{-0.5f, -0.5f, 0.0f}, { 0.5f, 0.0f, 0.0f }},
				{{0.5f, -0.5f, 0.0f}, { 0.0f, 0.5f, 0.0f }},
				{{0.0f, 0.5f, 0.0f}, { 0.0f, 0.0f, 0.5f }}
			};

			std::vector<uint32_t> indices =
			{
				0, 1, 2
			};

			VkBuffer vertex_buffer;
			VkDeviceMemory vertex_memory;

			VkBuffer index_buffer;
			VkDeviceMemory index_memory;
		};
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		void Init() override;
		void BeginRenderPass(const Ref<RenderCommandBuffer>& render_command_buffer, const Ref<RenderPass>& render_pass) override;
		void RenderTriangle(const Ref<RenderCommandBuffer>& render_command_buffer, const Ref<RenderPass>& render_pass) override;
		void EndRenderPass(const Ref<RenderCommandBuffer>& render_command_buffer, const Ref<RenderPass>& render_pass) override;
	private:
		inline static RenderData* s_RenderData = nullptr;
	};
}