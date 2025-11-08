#pragma once
#include "Device.h"
#include "Swapchain.h"
#include "RenderPass.h"
#define GLFW_INCLUED_VULKAN
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
namespace ev
{
	class RenderPipeline final
	{
	public:
		void Init(const Device& device, const RenderPass& render_pass, const Swapchain& swapchain, VkDescriptorSetLayout& descriptor_layout);
		void Destroy(const Device& device);
		inline const VkPipeline& GetPipeline() const { return _pipeline; }
		inline const VkPipelineLayout& GetPipelineLayout() const { return _pipeline_layout; }
	private:
		VkPipeline _pipeline;
		VkPipelineLayout _pipeline_layout;
	};
}