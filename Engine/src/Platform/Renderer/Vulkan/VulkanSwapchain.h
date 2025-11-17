#pragma once
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
namespace srx
{
	/*class VulkanSwapchain final
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
	};*/

	class VulkanSwapchain final
	{
	public:
		VulkanSwapchain(GLFWwindow* window);
		virtual ~VulkanSwapchain() = default;
		void Create(uint32_t width, uint32_t height);
	private:
		void FindQueueIndex(VkPhysicalDevice physical_device);
		void FindColorForamatAndColorSpace(VkPhysicalDevice physical_device);	//在确定物理设备的时候就可以调用
	private:
		VkSwapchainKHR _Swapchain = nullptr;
		VkSurfaceKHR _Surface;
		uint32_t _GraphicQueueIndex;
		uint32_t _PresentQueueIndex;
		VkFormat _ColorFormat;
		VkColorSpaceKHR _ColorSpace;
	};
}