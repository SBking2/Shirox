#include "Swapchain.h"
#include "Device.h"
#include <set>
#include <stdexcept>
namespace ev
{
	void Swapchain::Init(const Device& device
		, const Surface& surface, const VkSurfaceFormatKHR& surface_format
		, const VkExtent2D& extent, const VkPresentModeKHR& present_mode
		, const std::vector<uint32_t>& queue_indices, uint32_t image_count)
	{
		swapchain_info.extent = extent;
		swapchain_info.surface_format = surface_format;
		swapchain_info.present_mode = present_mode;
		swapchain_info.queue_indices = queue_indices;
		swapchain_info.img_count = image_count;
		ReCreate(device, surface);
	}

	void Swapchain::Destroy(const Device& device)
	{
		for (const auto& img : _img_views)
			vkDestroyImageView(device.GetLogicalDevice(), img, nullptr);

		vkDestroySwapchainKHR(device.GetLogicalDevice(), _swapchain, nullptr);
	}

	void Swapchain::ReCreate(const Device& device, const Surface& surface)
	{
		VkSwapchainCreateInfoKHR create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		create_info.surface = surface.GetSurface();
		create_info.minImageCount = swapchain_info.img_count;
		create_info.imageFormat = swapchain_info.surface_format.format;
		create_info.imageColorSpace = swapchain_info.surface_format.colorSpace;
		create_info.imageExtent = swapchain_info.extent;
		create_info.imageArrayLayers = 1;
		create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		//此处要决定哪些队列簇会使用到swapchain的图片
		create_info.imageSharingMode = swapchain_info.queue_indices.size() > 1 ? VK_SHARING_MODE_CONCURRENT : VK_SHARING_MODE_EXCLUSIVE;
		create_info.queueFamilyIndexCount = swapchain_info.queue_indices.size();
		create_info.pQueueFamilyIndices = swapchain_info.queue_indices.data();

		create_info.preTransform = device.device_info.capabilities.currentTransform;
		create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		create_info.presentMode = swapchain_info.present_mode;
		create_info.clipped = VK_TRUE;
		create_info.oldSwapchain = VK_NULL_HANDLE;

		if (vkCreateSwapchainKHR(device.GetLogicalDevice(), &create_info, nullptr, &_swapchain) != VK_SUCCESS)
			throw std::runtime_error("failed to create swapchain KHR");

		//获取交换链的图片
		uint32_t swapchain_img_count;
		vkGetSwapchainImagesKHR(device.GetLogicalDevice(), _swapchain, &swapchain_img_count, nullptr);
		_imgs.resize(swapchain_img_count);
		vkGetSwapchainImagesKHR(device.GetLogicalDevice(), _swapchain, &swapchain_img_count, _imgs.data());

		CreateImageView(device);
	}

	void Swapchain::CreateImageView(const Device& device)
	{
		_img_views.resize(_imgs.size());
		for (int i = 0; i < _img_views.size(); i++)
		{
			VkImageViewCreateInfo create_info = {};
			create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			create_info.image = _imgs[i];
			create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
			create_info.format = swapchain_info.surface_format.format;

			create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

			create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			create_info.subresourceRange.baseMipLevel = 0;
			create_info.subresourceRange.levelCount = 1;
			create_info.subresourceRange.baseArrayLayer = 0;
			create_info.subresourceRange.layerCount = 1;

			if (vkCreateImageView(device.GetLogicalDevice(), &create_info, nullptr, &_img_views[i])
				!= VK_SUCCESS)
			{
				throw std::runtime_error("failed to create image view!");
			}
		}
	}
}