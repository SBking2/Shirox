#include "PCH.h"
#include "VulkanSwapchain.h"

namespace srx
{
	void VulkanSwapchain::CreateSwapchain()
	{
		std::unordered_set<uint32_t> queue_indices_set;
		queue_indices_set.insert(_context->device_info.graphic_queue_index);
		queue_indices_set.insert(_context->device_info.present_queue_index);

		std::vector<uint32_t> queue_indedices_vec;
		for (const auto& it : queue_indices_set)
			queue_indedices_vec.push_back(it);

		VkSwapchainCreateInfoKHR create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		create_info.surface = _context->GetSurface();
		create_info.minImageCount = swapchain_info.img_count;
		create_info.imageFormat = swapchain_info.format.format;
		create_info.imageColorSpace = swapchain_info.format.colorSpace;
		create_info.imageExtent = swapchain_info.extent;
		create_info.imageArrayLayers = 1;
		create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		//此处要决定哪些队列簇会使用到swapchain的图片
		create_info.imageSharingMode = queue_indedices_vec.size() > 1 ? VK_SHARING_MODE_CONCURRENT : VK_SHARING_MODE_EXCLUSIVE;
		create_info.queueFamilyIndexCount = queue_indedices_vec.size();
		create_info.pQueueFamilyIndices = queue_indedices_vec.data();

		create_info.preTransform = _context->device_info.capabilities.currentTransform;
		create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		create_info.presentMode = swapchain_info.present_mode;
		create_info.clipped = VK_TRUE;
		create_info.oldSwapchain = VK_NULL_HANDLE;

		if (vkCreateSwapchainKHR(_context->GetDevice(), &create_info, nullptr, &_swapchain) != VK_SUCCESS)
			Log::LogError("failed to create swapchain KHR!");
	}

	void VulkanSwapchain::Init(Ref<const VulkanContext> context)
	{
		_context = context;
		ChooseSwapchainInfo();
		CreateSwapchain();
	}

	void VulkanSwapchain::Destroy()
	{
		vkDestroySwapchainKHR(_context->GetDevice(), _swapchain, nullptr);
	}

	void VulkanSwapchain::ChooseSwapchainInfo()
	{
		//选择format
		{
			if (_context->device_info.formats.size() == 1 && _context->device_info.formats[0].format == VK_FORMAT_UNDEFINED)
			{
				swapchain_info.format = { VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
			}

			bool is_have_format = false;
			for (const auto& format : _context->device_info.formats)
			{
				if (format.format == VK_FORMAT_B8G8R8A8_UNORM && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
				{
					swapchain_info.format = format;
					is_have_format = true;
					break;
				}
			}

			if (!is_have_format) swapchain_info.format = _context->device_info.formats[0];
		}

		//选择mode
		{
			bool is_have_mode = false;
			for (const auto& mode : _context->device_info.present_modes)
			{
				if (mode == VK_PRESENT_MODE_MAILBOX_KHR)
				{
					swapchain_info.present_mode = mode;
					is_have_mode = true;
					break;
				}
			}
			if (!is_have_mode) swapchain_info.present_mode = VK_PRESENT_MODE_FIFO_KHR;	//所有设备都支持这个
		}

		//选择extent
		{
			if (_context->device_info.capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
			{
				swapchain_info.extent = _context->device_info.capabilities.currentExtent;
			}
			else
			{
				uint32_t width, height;
				_context->GetWindow()->GetWdithAndHeight(width, height);

				swapchain_info.extent = { width, height };
				swapchain_info.extent.width = std::max(_context->device_info.capabilities.minImageExtent.width
					, std::min(_context->device_info.capabilities.maxImageExtent.width, swapchain_info.extent.width));

				swapchain_info.extent.height = std::max(_context->device_info.capabilities.minImageExtent.height
					, std::min(_context->device_info.capabilities.maxImageExtent.height, swapchain_info.extent.height));
			}
		}

		swapchain_info.img_count = _context->device_info.capabilities.minImageCount + 1;
		if (_context->device_info.capabilities.maxImageCount > 0 &&
			swapchain_info.img_count > _context->device_info.capabilities.maxImageCount)
			swapchain_info.img_count = _context->device_info.capabilities.maxImageCount;
	}
}