#pragma once
#include "Renderer/RendererAPI.h"
#include "VulkanContext.h"
#include "VulkanSwapchain.h"
namespace srx
{
	class VulkanRenderer final : public RendererAPI
	{
	public:
		void BeginRenderPass(const Ref<RenderCommandBuffer>& render_command_buffer, const Ref<RenderPass>& render_pass);
		void RenderTriangle(const Ref<RenderCommandBuffer>& render_command_buffer, const Ref<RenderPass>& render_pass);
		void EndRenderPass(const Ref<RenderCommandBuffer>& render_command_buffer, const Ref<RenderPass>& render_pass);
	private:
	};
}