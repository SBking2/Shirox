#include "PCH.h"
#include "VulkanRenderCommandBuffer.h"
#include "VulkanContext.h"
namespace srx
{
	VulkanRenderCommandBuffer::VulkanRenderCommandBuffer()
	{
		VulkanContext* context = (VulkanContext*)RendererContext::GetContext().get();

		VkCommandPoolCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;

		VkResult result = vkCreateCommandPool(context->GetVkDevice(), &create_info, nullptr, &_CommandPool);
	}

	VulkanRenderCommandBuffer::~VulkanRenderCommandBuffer()
	{

	}

	void VulkanRenderCommandBuffer::Begin()
	{

	}

	void VulkanRenderCommandBuffer::End()
	{

	}

	void VulkanRenderCommandBuffer::Submit()
	{

	}
}