#pragma once
#include "Device.h"
#define GLFW_INCLUED_VULKAN
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
namespace srx
{
	class CommandPool final
	{
	public:
		void Init(const Device& device);
		void Destroy(const Device& device);
		inline const VkCommandPool& GetCommandPool() const { return _command_pool; }
	private:
		VkCommandPool _command_pool;
	};
}