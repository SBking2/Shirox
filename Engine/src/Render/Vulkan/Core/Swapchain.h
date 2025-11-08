#pragma once
#include "Device.h"
#include "Surface.h"
#include <vector>
#define GLFW_INCLUED_VULKAN
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
namespace ev
{
	class Swapchain final
	{
	public:
		struct SwapchianInfo
		{
			VkExtent2D extent;
			VkSurfaceFormatKHR surface_format;
			VkPresentModeKHR present_mode;
			std::vector<uint32_t> queue_indices;
			uint32_t img_count;
		};
		void Init(const Device& device
			, const Surface& surface, const VkSurfaceFormatKHR& surface_format
			, const VkExtent2D& extent, const VkPresentModeKHR& present_mode
			, const std::vector<uint32_t>& queue_indices, uint32_t image_count);
		void Destroy(const Device& device);
		void ReCreate(const Device& device, const Surface& surface);

		inline const VkSwapchainKHR& GetSwapchain() const { return _swapchain; }
		inline const std::vector<VkImageView>& GetImgView() const { return _img_views; }
		inline uint32_t GetImgCount() const { return _img_views.size(); }
	private:
		void CreateImageView(const Device& device);
	public:
		SwapchianInfo swapchain_info;
	private:
		VkSwapchainKHR _swapchain;
		std::vector<VkImage> _imgs;
		std::vector<VkImageView> _img_views;
	};
}