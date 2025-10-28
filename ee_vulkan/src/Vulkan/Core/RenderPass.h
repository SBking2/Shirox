#pragma once
#include "Device.h"
#include "SwapChain.h"
#define GLFW_INCLUED_VULKAN
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
namespace ev
{
	class RenderPass final
	{
	public:
		void Init(const Device& device, const Swapchain& swapchain);
		void Destroy(const Device& device);
		inline const VkRenderPass& GetRenderPass() const { return _render_pass; }
	private:
		VkRenderPass _render_pass;
	};
}