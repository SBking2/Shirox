#include "PCH.h"
#include "CommandPool.h"
#include <stdexcept>
namespace srx
{
	void CommandPool::Init(const Device& device)
	{
		VkCommandPoolCreateInfo command_pool_create_info = {};
		command_pool_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		command_pool_create_info.queueFamilyIndex = device.device_info.graphic_queue_index;
		command_pool_create_info.flags = 0;

		if (vkCreateCommandPool(device.GetLogicalDevice(), &command_pool_create_info, nullptr, &_command_pool)
			!= VK_SUCCESS)
			throw std::runtime_error("failed to create command pool!");
	}
	void CommandPool::Destroy(const Device& device)
	{
		vkDestroyCommandPool(device.GetLogicalDevice(), _command_pool, nullptr);
	}
}