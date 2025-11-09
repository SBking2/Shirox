#pragma once
#include "VulkanContext.h"
#define GLFW_INCLUED_VULKAN
#include <GLFW/glfw3.h>
namespace srx
{
	class VulkanSwapchain final
	{
	public:
		struct SwapchainInfo
		{
			VkExtent2D extent;
			VkSurfaceFormatKHR format;
			VkPresentModeKHR present_mode;
			uint32_t img_count;
		};
		void Init(Ref<const VulkanContext> context);
		void Destroy();
	private:
		void ChooseSwapchainInfo();
		void CreateSwapchain();
	public:
		SwapchainInfo swapchain_info;
	private:
		Ref<const VulkanContext> _context;
		VkSwapchainKHR _swapchain;
	};
}