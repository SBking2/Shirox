#include "PCH.h"
#include "VulkanSwapchain.h"
#include "VulkanContext.h"
namespace srx
{
	//void VulkanSwapchain::CreateSwapchain()
	//{
	//	std::unordered_set<uint32_t> queue_indices_set;
	//	queue_indices_set.insert(_context->device_info.graphic_queue_index);
	//	queue_indices_set.insert(_context->device_info.present_queue_index);

	//	std::vector<uint32_t> queue_indedices_vec;
	//	for (const auto& it : queue_indices_set)
	//		queue_indedices_vec.push_back(it);

	//	VkSwapchainCreateInfoKHR create_info = {};
	//	create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	//	create_info.surface = _context->GetSurface();
	//	create_info.minImageCount = swapchain_info.img_count;
	//	create_info.imageFormat = swapchain_info.format.format;
	//	create_info.imageColorSpace = swapchain_info.format.colorSpace;
	//	create_info.imageExtent = swapchain_info.extent;
	//	create_info.imageArrayLayers = 1;
	//	create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	//	//此处要决定哪些队列簇会使用到swapchain的图片
	//	create_info.imageSharingMode = queue_indedices_vec.size() > 1 ? VK_SHARING_MODE_CONCURRENT : VK_SHARING_MODE_EXCLUSIVE;
	//	create_info.queueFamilyIndexCount = queue_indedices_vec.size();
	//	create_info.pQueueFamilyIndices = queue_indedices_vec.data();

	//	create_info.preTransform = _context->device_info.capabilities.currentTransform;
	//	create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	//	create_info.presentMode = swapchain_info.present_mode;
	//	create_info.clipped = VK_TRUE;
	//	create_info.oldSwapchain = VK_NULL_HANDLE;

	//	if (vkCreateSwapchainKHR(_context->GetDevice(), &create_info, nullptr, &_swapchain) != VK_SUCCESS)
	//		Log::LogError("failed to create swapchain KHR!");
	//}

	//void VulkanSwapchain::Init(Ref<const VulkanContext> context)
	//{
	//	_context = context;
	//	ChooseSwapchainInfo();
	//	CreateSwapchain();
	//}

	//void VulkanSwapchain::Destroy()
	//{
	//	vkDestroySwapchainKHR(_context->GetDevice(), _swapchain, nullptr);
	//}

	//void VulkanSwapchain::ChooseSwapchainInfo()
	//{
	//	//选择format
	//	{
	//		if (_context->device_info.formats.size() == 1 && _context->device_info.formats[0].format == VK_FORMAT_UNDEFINED)
	//		{
	//			swapchain_info.format = { VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
	//		}

	//		bool is_have_format = false;
	//		for (const auto& format : _context->device_info.formats)
	//		{
	//			if (format.format == VK_FORMAT_B8G8R8A8_UNORM && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
	//			{
	//				swapchain_info.format = format;
	//				is_have_format = true;
	//				break;
	//			}
	//		}

	//		if (!is_have_format) swapchain_info.format = _context->device_info.formats[0];
	//	}

	//	//选择mode
	//	{
	//		bool is_have_mode = false;
	//		for (const auto& mode : _context->device_info.present_modes)
	//		{
	//			if (mode == VK_PRESENT_MODE_MAILBOX_KHR)
	//			{
	//				swapchain_info.present_mode = mode;
	//				is_have_mode = true;
	//				break;
	//			}
	//		}
	//		if (!is_have_mode) swapchain_info.present_mode = VK_PRESENT_MODE_FIFO_KHR;	//所有设备都支持这个
	//	}

	//	//选择extent
	//	{
	//		if (_context->device_info.capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
	//		{
	//			swapchain_info.extent = _context->device_info.capabilities.currentExtent;
	//		}
	//		else
	//		{
	//			uint32_t width, height;
	//			_context->GetWindow()->GetWdithAndHeight(width, height);

	//			swapchain_info.extent = { width, height };
	//			swapchain_info.extent.width = std::max(_context->device_info.capabilities.minImageExtent.width
	//				, std::min(_context->device_info.capabilities.maxImageExtent.width, swapchain_info.extent.width));

	//			swapchain_info.extent.height = std::max(_context->device_info.capabilities.minImageExtent.height
	//				, std::min(_context->device_info.capabilities.maxImageExtent.height, swapchain_info.extent.height));
	//		}
	//	}

//	swapchain_info.img_count = _context->device_info.capabilities.minImageCount + 1;
	//	if (_context->device_info.capabilities.maxImageCount > 0 &&
	//		swapchain_info.img_count > _context->device_info.capabilities.maxImageCount)
	//		swapchain_info.img_count = _context->device_info.capabilities.maxImageCount;
	//}

	//初始化Surface，查找队列，图片格式和颜色空间
	VulkanSwapchain::VulkanSwapchain(GLFWwindow* window)
	{
		VulkanContext* context = (VulkanContext*)RendererContext::GetContext().get();

		/////////////////////////////////////////////// 创建Surface
		glfwCreateWindowSurface(context->GetVkInstance(), window, nullptr, &_Surface);

		FindQueueIndex(context->GetPhysicalDevice()->GetVkPhysicalDevice());
		FindColorForamatAndColorSpace(context->GetPhysicalDevice()->GetVkPhysicalDevice());
	}

	void VulkanSwapchain::Create(uint32_t width, uint32_t height)
	{
		VulkanContext* context = (VulkanContext*)RendererContext::GetContext().get();

		VkSwapchainCreateInfoKHR create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		create_info.surface = _Surface;
		create_info.minImageCount = ;
		create_info.imageFormat = _ColorFormat;
		create_info.imageColorSpace = _ColorSpace;
		create_info.imageExtent = ;
		create_info.imageArrayLayers = 1;
		create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		std::vector<uint32_t> queue_indices { _GraphicQueueIndex };
		if(_GraphicQueueIndex != _PresentQueueIndex) queue_indices.emplace_back(_PresentQueueIndex);

		//此处要决定哪些队列簇会使用到swapchain的图片
		create_info.imageSharingMode = queue_indices.size() > 1 ? VK_SHARING_MODE_CONCURRENT : VK_SHARING_MODE_EXCLUSIVE;
		create_info.queueFamilyIndexCount = queue_indices.size();
		create_info.pQueueFamilyIndices = queue_indices.data();

		create_info.preTransform = ;
		create_info.compositeAlpha = ;
		create_info.presentMode = ;
		create_info.clipped = VK_TRUE;
		create_info.oldSwapchain = VK_NULL_HANDLE;

		VkResult result = vkCreateSwapchainKHR(context->GetDevice()->GetVkDevice(), &create_info, nullptr, &_Swapchain);
		SRX_ASSERT(result == VK_SUCCESS, "创建Swapchain失败!");
	}

	/// <summary>
	/// 查找Graphic和Present两种功能的队列簇
	/// </summary>
	void VulkanSwapchain::FindQueueIndex(VkPhysicalDevice physical_device)
	{
		uint32_t queue_count;
		vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_count, nullptr);
		std::vector<VkQueueFamilyProperties> queue_properties(queue_count);
		vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_count, queue_properties.data());

		//////////////////////////////////////////////// 查询队列簇是否兼容Surface
		std::vector<VkBool32> supported_present(queue_count);
		for (uint32_t i = 0; i < queue_count; i++)
			vkGetPhysicalDeviceSurfaceSupportKHR(physical_device, i, _Surface, supported_present.data());

		_GraphicQueueIndex = UINT32_MAX;
		_PresentQueueIndex = UINT32_MAX;

		for (uint32_t i = 0; i < queue_count; i++)
		{
			if (_GraphicQueueIndex != UINT32_MAX && (queue_properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0)
				_GraphicQueueIndex = i;
			
			if (_PresentQueueIndex != UINT32_MAX && supported_present[i] == VK_TRUE)
				_PresentQueueIndex = i;

			if (_GraphicQueueIndex != UINT32_MAX && _PresentQueueIndex != UINT32_MAX)
				break;
		}

		SRX_ASSERT(_GraphicQueueIndex == UINT32_MAX || _PresentQueueIndex == UINT32_MAX, "创建Swapchain时，查找不到需要的队列簇!");
	}

	//目的是查找R8G8B8A8的格式，如果没有就随便用一个
	void VulkanSwapchain::FindColorForamatAndColorSpace(VkPhysicalDevice physical_device)
	{
		uint32_t format_count;
		vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, _Surface, &format_count, nullptr);
		std::vector<VkSurfaceFormatKHR> surface_formats(format_count);
		vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, _Surface, &format_count, surface_formats.data());

		if (format_count == 1 && surface_formats[0].format == VK_FORMAT_UNDEFINED)		//表示Surface没有指定的格式
		{
			_ColorFormat = VK_FORMAT_R8G8B8A8_UNORM;
			_ColorSpace = surface_formats[0].colorSpace;
		}

		for (uint32_t i = 0; i < format_count; i++)
		{
			if (surface_formats[i].format == VK_FORMAT_R8G8B8A8_UNORM)
			{
				_ColorFormat = surface_formats[i].format;
				_ColorSpace = surface_formats[i].colorSpace;
				return;
			}
		}

		SRX_ASSERT(format_count != 0, "查找Swapchian图片格式和颜色空间失败!");

		_ColorFormat = surface_formats[0].format;
		_ColorSpace = surface_formats[0].colorSpace;
	}


}