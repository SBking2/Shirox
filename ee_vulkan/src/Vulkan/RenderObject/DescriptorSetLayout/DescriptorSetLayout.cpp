#include "DescriptorSetLayout.h"
#include <array>
#include <stdexcept>
namespace ev
{
	std::shared_ptr<DescriptorSetLayout> DescriptorSetLayout::Create(const Device& device)
	{
		std::shared_ptr<DescriptorSetLayout> layout = std::make_shared<DescriptorSetLayout>();
		layout->Init(device);
		return layout;
	}

	void DescriptorSetLayout::Destroy(const Device& device)
	{
		vkDestroyDescriptorSetLayout(device.GetLogicalDevice(), _layout, nullptr);
	}

	void DescriptorSetLayout::Init(const Device& device)
	{
		//uniform
		VkDescriptorSetLayoutBinding ubo_layout_binding = {};
		ubo_layout_binding.binding = 0;
		ubo_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		ubo_layout_binding.descriptorCount = 1;
		ubo_layout_binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;		//表明在顶点着色器阶段使用

		//sampler
		VkDescriptorSetLayoutBinding sampler_layout_binding = {};
		sampler_layout_binding.binding = 1;
		sampler_layout_binding.descriptorCount = 1;
		sampler_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		sampler_layout_binding.pImmutableSamplers = nullptr;
		sampler_layout_binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;	//在片段着色器阶段使用

		std::array<VkDescriptorSetLayoutBinding, 2> bindings = {
			ubo_layout_binding, sampler_layout_binding
		};

		VkDescriptorSetLayoutCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		create_info.bindingCount = static_cast<uint32_t>(bindings.size());
		create_info.pBindings = bindings.data();

		if (vkCreateDescriptorSetLayout(device.GetLogicalDevice(), &create_info, nullptr, &_layout) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create descriptor set layout!");
		}
	}
}