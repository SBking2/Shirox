#include "PCH.h"
#include "VulkanRenderCommandBuffer.h"
#include "VulkanContext.h"
namespace srx
{
	VulkanRenderCommandBuffer::VulkanRenderCommandBuffer()
	{
		VulkanContext* context = (VulkanContext*)RendererContext::GetContext().get();
		
		////////////////////////////////////////////////////////// 创建cmd pool
		VkCommandPoolCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;

		VkResult pool_result = vkCreateCommandPool(context->GetVkDevice(), &create_info, nullptr, &_CommandPool);
		SRX_ASSERT(pool_result == VK_SUCCESS, "创建命令池失败!");

		////////////////////////////////////////////////////////// 分配cmd buffer
		VkCommandBufferAllocateInfo cmd_buffer_allocate_info = {};
		cmd_buffer_allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;

		VkResult cmd_result = vkAllocateCommandBuffers(context->GetVkDevice(), &cmd_buffer_allocate_info, &_CommandBuffer);
		SRX_ASSERT(cmd_result == VK_SUCCESS, "分配命令缓冲区失败!");
	}

	VulkanRenderCommandBuffer::~VulkanRenderCommandBuffer()
	{
		VulkanContext* context = (VulkanContext*)RendererContext::GetContext().get();
		vkDestroyCommandPool(context->GetVkDevice(), _CommandPool, nullptr);
	}

	/// <summary>
	/// 开启命令录制
	/// </summary>
	void VulkanRenderCommandBuffer::Begin()
	{
		VkCommandBufferBeginInfo begin_info = {};
		begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		vkBeginCommandBuffer(_CommandBuffer, &begin_info);
	}

	/// <summary>
	/// 结束命令录制
	/// </summary>
	void VulkanRenderCommandBuffer::End()
	{
		vkEndCommandBuffer(_CommandBuffer);
	}

	/// <summary>
	/// 执行cmd
	/// </summary>
	void VulkanRenderCommandBuffer::Submit()
	{
		VulkanContext* context = RendererContext::GetSpecificContext<VulkanContext>();

		////////////////////////////////////////////////////////// 提交
		VkSubmitInfo submit_info = {};
		submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		vkQueueSubmit(context->GetDevice()->GetGraphicQueue(), 1, &submit_info, VK_NULL_HANDLE);
	}
}