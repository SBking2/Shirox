#include "PCH.h"
#include "DescriptorPool.h"
#include <array>
#include <stdexcept>
namespace srx
{
	void DescriptorPool::Init(const Device& device)
	{
		std::array<VkDescriptorPoolSize, 2> pool_sizes = {};
		pool_sizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		pool_sizes[0].descriptorCount = 1;

		pool_sizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		pool_sizes[1].descriptorCount = 1;

		VkDescriptorPoolCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		create_info.poolSizeCount = static_cast<uint32_t>(pool_sizes.size());
		create_info.pPoolSizes = pool_sizes.data();

		create_info.maxSets = 1;

		if (vkCreateDescriptorPool(device.GetLogicalDevice(), &create_info, nullptr, &_pool) != VK_SUCCESS)
			throw std::runtime_error("failed to create descriptor pool!");
	}

	void DescriptorPool::Destroy(const Device& device)
	{
		vkDestroyDescriptorPool(device.GetLogicalDevice(), _pool, nullptr);
	}
}