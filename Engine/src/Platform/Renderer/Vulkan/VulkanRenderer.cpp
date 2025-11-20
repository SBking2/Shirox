#include "PCH.h"
#include "VulkanRenderer.h"
#include "VulkanRenderCommandBuffer.h"
namespace srx
{
	void VulkanRenderer::Init()
	{
		s_RenderData = new RenderData();
		
		VulkanContext* context = VulkanContext::GetSpecificContext<VulkanContext>();

		////////////////////////////////////////////////////////////////////////////////////////////////////
		/////////						Debug使用的vertexBuffe和IndexBuffer
		////////////////////////////////////////////////////////////////////////////////////////////////////

		//////////////////////////////////////////////////////创建Vertex Buffer////////////////////////////////////////////////////
		VkBufferCreateInfo vertex_info = {};
		vertex_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		vertex_info.size = sizeof(Vertex) * s_RenderData->vertices.size();
		vertex_info.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
		vertex_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		vkCreateBuffer(context->GetVkDevice(), &vertex_info, nullptr, &s_RenderData->vertex_buffer);

		VkMemoryRequirements vertex_mem_requirements;
		vkGetBufferMemoryRequirements(context->GetVkDevice(), s_RenderData->vertex_buffer, &vertex_mem_requirements);

		VkMemoryAllocateInfo vertex_memory_info = {};
		vertex_memory_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		vertex_memory_info.allocationSize = vertex_mem_requirements.size;
		vertex_memory_info.memoryTypeIndex = ;
		vkAllocateMemory(context->GetVkDevice(), &vertex_memory_info, nullptr, &s_RenderData->vertex_memory);

		//////////////////////////////////////////////////////创建index Buffer////////////////////////////////////////////////////
		VkBufferCreateInfo index_info = {};
		index_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		index_info.size = sizeof(uint32_t) * s_RenderData->indices.size();
		index_info.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
		index_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		vkCreateBuffer(context->GetVkDevice(), &index_info, nullptr, &s_RenderData->index_buffer);

		VkMemoryRequirements index_mem_requirements;
		vkGetBufferMemoryRequirements(context->GetVkDevice(), s_RenderData->index_buffer, &index_mem_requirements);

		VkMemoryAllocateInfo index_memory_info = {};
		index_memory_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		index_memory_info.allocationSize = index_mem_requirements.size;
		index_memory_info.memoryTypeIndex = ;
		vkAllocateMemory(context->GetVkDevice(), &index_memory_info, nullptr, &s_RenderData->index_memory);
	}

	void VulkanRenderer::BeginRenderPass(const Ref<RenderCommandBuffer>& render_command_buffer, const Ref<RenderPass>& render_pass)
	{
		VulkanRenderCommandBuffer* cmd_buffer = (VulkanRenderCommandBuffer*)render_command_buffer.get();

		VkRenderPassBeginInfo renderpass_info = {};
		renderpass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderpass_info.renderPass = _render_pass.GetRenderPass();
		renderpass_info.framebuffer = _frame_buffers[i]->GetFrameBuffer();
		renderpass_info.renderArea.offset = { 0, 0 };
		renderpass_info.renderArea.extent = _swapchain.swapchain_info.extent;

		std::array<VkClearValue, 2> clear_values = {};
		clear_values[0].color = { 0.1f, 0.1f, 0.1f, 1.0f };
		clear_values[1].depthStencil = { 1.0f , 0 };
		renderpass_info.clearValueCount = static_cast<uint32_t>(clear_values.size());
		renderpass_info.pClearValues = clear_values.data();

		/////////////////////////////////////////////绑定RenderPass///////////////////////////////////////////
		vkCmdBeginRenderPass(cmd_buffer->GetVkCommandBuffer(), &renderpass_info, VK_SUBPASS_CONTENTS_INLINE);

		/////////////////////////////////////////////绑定Pipeline///////////////////////////////////////////
		vkCmdBindPipeline(cmd_buffer->GetVkCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, _pipeline.GetPipeline());
	}

	void VulkanRenderer::RenderTriangle(const Ref<RenderCommandBuffer>& render_command_buffer, const Ref<RenderPass>& render_pass)
	{
		VulkanRenderCommandBuffer* cmd_buffer = (VulkanRenderCommandBuffer*)render_command_buffer.get();

		VkBuffer vertex_buffers[] = { _vertex_buffer->GetBuffer() };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(cmd_buffer->GetVkCommandBuffer(), 0, 1, vertex_buffers, offsets);
		vkCmdBindIndexBuffer(cmd_buffer->GetVkCommandBuffer(), _index_buffer->GetBuffer(), 0, VK_INDEX_TYPE_UINT32);

		//绘制
		vkCmdDrawIndexed(cmd_buffer->GetVkCommandBuffer(), static_cast<uint32_t>(s_RenderData->indices.size()), 1, 0, 0, 0);
	}

	void VulkanRenderer::EndRenderPass(const Ref<RenderCommandBuffer>& render_command_buffer, const Ref<RenderPass>& render_pass)
	{
		VulkanRenderCommandBuffer* cmd_buffer = (VulkanRenderCommandBuffer*)render_command_buffer.get();

		/////////////////////////////////////////////结束RenderPass///////////////////////////////////////////
		vkCmdEndRenderPass(cmd_buffer->GetVkCommandBuffer());
	}
}