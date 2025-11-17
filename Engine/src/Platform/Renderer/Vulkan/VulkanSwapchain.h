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
		void Destory();
		uint32_t AcquireImageIndex();
		void Present();
	private:
		void FindQueueIndex(VkPhysicalDevice physical_device);
		void FindColorForamatAndColorSpace(VkPhysicalDevice physical_device);
		void PickPresentMode(VkPhysicalDevice physical_device);
		void CreateImages();
		void CreateRenderPass();
		void CreateFrameBuffers();
		void CreateCommandBuffer();
	private:
		VkSwapchainKHR _Swapchain = nullptr;
		VkSwapchainKHR _OldSwapchain = nullptr;
		VkSurfaceKHR _Surface;

		std::vector<VkImage> _Images;
		std::vector<VkImageView> _ImageViews;
		std::vector<VkFramebuffer> _FrameBuffers;
		VkRenderPass _RenderPass = nullptr;
		VkCommandPool _CommandPool = nullptr;
		std::vector<VkCommandBuffer> _CommandBuffers;

		uint32_t _GraphicQueueIndex;
		uint32_t _PresentQueueIndex;
		VkFormat _ColorFormat;
		VkPresentModeKHR _PresentMode;
		VkColorSpaceKHR _ColorSpace;

		uint32_t _Width, _Height;
	};
}