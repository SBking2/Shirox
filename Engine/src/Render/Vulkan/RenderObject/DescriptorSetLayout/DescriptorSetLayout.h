#pragma once
#include "Render/Vulkan/Core/Device.h"
#define GLFW_INCLUED_VULKAN
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <memory>
namespace ev
{
	class DescriptorSetLayout final
	{
	public:
		static std::shared_ptr<DescriptorSetLayout> Create(const Device& device);
		void Destroy(const Device& device);
		inline const VkDescriptorSetLayout& GetLayout() const { return _layout; }
		inline VkDescriptorSetLayout* GetLayoutP() { return &_layout; }
	private:
		void Init(const Device& device);
	private:
		VkDescriptorSetLayout _layout;
	};
}