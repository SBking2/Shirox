#pragma once
#include "Device.h"
#define GLFW_INCLUED_VULKAN
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
namespace srx
{
	class DescriptorPool final
	{
	public:
		void Init(const Device& device);
		void Destroy(const Device& device);
		inline const VkDescriptorPool& GetPool() const { return _pool; }
	private:
		VkDescriptorPool _pool;
	};
}