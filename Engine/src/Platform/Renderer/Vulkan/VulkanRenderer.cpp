#include "PCH.h"
#include "VulkanRenderer.h"
#include "VulkanRenderCommandBuffer.h"
namespace srx
{
	void VulkanRenderer::BeginRenderPass(const Ref<RenderCommandBuffer>& render_command_buffer, const Ref<RenderPass>& render_pass)
	{
		VulkanRenderCommandBuffer* cmd_buffer = (VulkanRenderCommandBuffer*)render_command_buffer.get();
	}

	void VulkanRenderer::RenderTriangle(const Ref<RenderCommandBuffer>& render_command_buffer, const Ref<RenderPass>& render_pass)
	{

	}

	void VulkanRenderer::EndRenderPass(const Ref<RenderCommandBuffer>& render_command_buffer, const Ref<RenderPass>& render_pass)
	{

	}
}