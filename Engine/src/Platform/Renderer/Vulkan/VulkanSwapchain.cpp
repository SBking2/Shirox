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

		FindQueueIndex(context->GetVkPhysicalDevice());
		FindColorForamatAndColorSpace(context->GetVkPhysicalDevice());
	}

	void VulkanSwapchain::Create(uint32_t width, uint32_t height)
	{
		_OldSwapchain = _Swapchain;

		VulkanContext* context = (VulkanContext*)RendererContext::GetContext().get();
		VkPhysicalDevice physical_device = context->GetVkPhysicalDevice();
		PickPresentMode(physical_device);

		//////////////////////////////////////// 获取创建交换链需要的信息
		VkSurfaceCapabilitiesKHR capabilities;
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device
			, _Surface, &capabilities);

		//////////////////////////////////////// 获取宽高和Extent
		VkExtent2D extent = {};
		if (capabilities.currentExtent.width == (uint32_t)-1)	//这表示当前的surface没有指定的宽高
		{
			extent.width = width;
			extent.height = height;
		}
		else
		{
			extent = capabilities.currentExtent;
			_Width = extent.width;
			_Height = extent.height;
		}

		if (_Width == 0 || _Height == 0) return;

		//////////////////////////////////////// 获取ImageCount
		uint32_t img_count = capabilities.minImageCount + 1;
		if (capabilities.maxImageCount > 0 && img_count > capabilities.maxImageCount)
			img_count = capabilities.maxImageCount;

		//////////////////////////////////////// 获取preTransform
		VkSurfaceTransformFlagsKHR pre_transform;
		if (pre_transform & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR)	//不需要选择
			pre_transform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
		else
			pre_transform = capabilities.currentTransform;
		
		//////////////////////////////////////// 获取compositeAlpha
		VkCompositeAlphaFlagBitsKHR composite_alpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;		//不透明
		std::vector<VkCompositeAlphaFlagBitsKHR> compositeAlphaFlags = {
			VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
			VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
			VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR,
			VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR,
		};

		for (auto& compositeAlphaFlag : compositeAlphaFlags) {
			if (capabilities.supportedCompositeAlpha & compositeAlphaFlag) {
				composite_alpha = compositeAlphaFlag;
				break;
			};
		}

		//////////////////////////////////////// 创建交换链
		VkSwapchainCreateInfoKHR create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		create_info.surface = _Surface;
		create_info.minImageCount = img_count;
		create_info.imageFormat = _ColorFormat;
		create_info.imageColorSpace = _ColorSpace;
		create_info.imageExtent = extent;
		create_info.imageArrayLayers = 1;
		create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		//添加Image的Usage
		if (capabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_SRC_BIT)
			create_info.imageUsage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;

		if (capabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT)
			create_info.imageUsage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;

		std::vector<uint32_t> queue_indices { _GraphicQueueIndex };
		if(_GraphicQueueIndex != _PresentQueueIndex) queue_indices.emplace_back(_PresentQueueIndex);

		//此处要决定哪些队列簇会使用到swapchain的图片
		create_info.imageSharingMode = queue_indices.size() > 1 ? VK_SHARING_MODE_CONCURRENT : VK_SHARING_MODE_EXCLUSIVE;
		create_info.queueFamilyIndexCount = queue_indices.size();
		create_info.pQueueFamilyIndices = queue_indices.data();

		create_info.preTransform = (VkSurfaceTransformFlagBitsKHR)pre_transform;
		create_info.compositeAlpha = composite_alpha;
		create_info.presentMode = _PresentMode;
		create_info.clipped = VK_TRUE;
		create_info.oldSwapchain = _OldSwapchain;

		VkResult result = vkCreateSwapchainKHR(context->GetVkDevice(), &create_info, nullptr, &_Swapchain);
		SRX_ASSERT(result == VK_SUCCESS, "创建Swapchain失败!");

		if (_OldSwapchain != nullptr)
			vkDestroySwapchainKHR(context->GetVkDevice(), _OldSwapchain, nullptr);

		CreateImages();
		CreateRenderPass();
		CreateFrameBuffers();
		CreateCommandBuffer();
	}

	void VulkanSwapchain::Destory()
	{
		VulkanContext* context = (VulkanContext*)RendererContext::GetContext().get();
		VkDevice device = context->GetVkDevice();
		vkDestroyCommandPool(device, _CommandPool, nullptr);

		for (auto& frame_buffer : _FrameBuffers)
			vkDestroyFramebuffer(device, frame_buffer, nullptr);

		vkDestroyRenderPass(device, _RenderPass, nullptr);

		for (auto& view : _ImageViews)
			vkDestroyImageView(device, view, nullptr);

		vkDestroySwapchainKHR(device, _Swapchain, nullptr);
		vkDestroySurfaceKHR(context->GetVkInstance(), _Surface, nullptr);
	}

	uint32_t VulkanSwapchain::AcquireImageIndex()
	{
		VulkanContext* context = (VulkanContext*)RendererContext::GetContext().get();

		uint32_t img_index;
		VkResult result = vkAcquireNextImageKHR(context->GetVkDevice(), _Swapchain, std::numeric_limits<uint64_t>::max(),
			nullptr, VK_NULL_HANDLE, &img_index);

		SRX_ASSERT(result == VK_SUCCESS, "获取交换链图像索引失败!");
		return img_index;
	}

	void VulkanSwapchain::Present()
	{
		VulkanContext* context = (VulkanContext*)RendererContext::GetContext().get();

		//返回渲染后的图像到交换链进行呈现
		VkPresentInfoKHR present_info = {};
		present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		present_info.waitSemaphoreCount = 0;
		present_info.pWaitSemaphores = nullptr;

		uint32_t img_index = AcquireImageIndex();
		present_info.swapchainCount = 1;
		present_info.pSwapchains = &_Swapchain;
		present_info.pImageIndices = &img_index;

		vkQueuePresentKHR(context->GetDevice()->GetGraphicQueu(), &present_info);
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

	/// <summary>
	/// 目的是查找R8G8B8A8的格式，如果没有就随便用一个
	/// </summary>
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

	/// <summary>
	/// 优先级 MailBox > Immediate > FIFO
	/// </summary>
	void VulkanSwapchain::PickPresentMode(VkPhysicalDevice physical_device)
	{
		uint32_t present_mode_count;
		vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, _Surface, &present_mode_count, nullptr);
		std::vector<VkPresentModeKHR> present_modes(present_mode_count);
		vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, _Surface, &present_mode_count, present_modes.data());
		
		for (uint32_t i = 0; i < present_mode_count; i++)
		{
			if (present_modes[i] == VK_PRESENT_MODE_MAILBOX_KHR)
			{
				_PresentMode = present_modes[i];
				return;
			}

			if (present_modes[i] == VK_PRESENT_MODE_IMMEDIATE_KHR)
				_PresentMode = present_modes[i];
		}

		if (_PresentMode != VK_PRESENT_MODE_IMMEDIATE_KHR)
			_PresentMode = VK_PRESENT_MODE_FIFO_KHR;
	}

	void VulkanSwapchain::CreateImages()
	{
		VulkanContext* context = (VulkanContext*)RendererContext::GetContext().get();

		uint32_t image_count;
		vkGetSwapchainImagesKHR(context->GetVkDevice(), _Swapchain, &image_count, nullptr);
		_Images.resize(image_count);
		vkGetSwapchainImagesKHR(context->GetVkDevice(), _Swapchain, &image_count, _Images.data());

		//创建ImageView之前要销毁旧的
		for (auto& view : _ImageViews)
			vkDestroyImageView(context->GetVkDevice(), view, nullptr);

		_ImageViews.resize(image_count);
		for(uint32_t i = 0; i < image_count; i++)
		{
			VkImageViewCreateInfo create_info = {};
			create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			create_info.pNext = NULL;
			create_info.format = _ColorFormat;
			create_info.image = _Images[i];
			create_info.components = {
				VK_COMPONENT_SWIZZLE_R,
				VK_COMPONENT_SWIZZLE_G,
				VK_COMPONENT_SWIZZLE_B,
				VK_COMPONENT_SWIZZLE_A
			};
			create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			create_info.subresourceRange.baseMipLevel = 0;
			create_info.subresourceRange.levelCount = 1;
			create_info.subresourceRange.baseArrayLayer = 0;
			create_info.subresourceRange.layerCount = 1;
			create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
			create_info.flags = 0;

			VkResult result = vkCreateImageView(context->GetVkDevice(), &create_info, nullptr, &_ImageViews[i]);
			SRX_ASSERT(result == VK_SUCCESS, "创建ImageView失败!");
		}
	}

	void VulkanSwapchain::CreateRenderPass()
	{
		VulkanContext* context = (VulkanContext*)RendererContext::GetContext().get();

		if (_RenderPass != nullptr)
			vkDestroyRenderPass(context->GetVkDevice(), _RenderPass, nullptr);

		VkAttachmentDescription color_attachment_descript = {};
		// Color attachment
		color_attachment_descript.format = _ColorFormat;
		color_attachment_descript.samples = VK_SAMPLE_COUNT_1_BIT;
		color_attachment_descript.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		color_attachment_descript.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		color_attachment_descript.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		color_attachment_descript.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		color_attachment_descript.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		color_attachment_descript.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentReference color_attachment_ref = {};
		color_attachment_ref.attachment = 0;
		color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpassDescription = {};
		subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpassDescription.colorAttachmentCount = 1;
		subpassDescription.pColorAttachments = &color_attachment_ref;
		subpassDescription.inputAttachmentCount = 0;
		subpassDescription.pInputAttachments = nullptr;
		subpassDescription.preserveAttachmentCount = 0;
		subpassDescription.pPreserveAttachments = nullptr;
		subpassDescription.pResolveAttachments = nullptr;

		VkSubpassDependency dependency = {};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.srcAccessMask = 0;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		VkRenderPassCreateInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = 1;
		renderPassInfo.pAttachments = &color_attachment_descript;
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpassDescription;
		renderPassInfo.dependencyCount = 1;
		renderPassInfo.pDependencies = &dependency;

		VkResult result = vkCreateRenderPass(context->GetVkDevice(), &renderPassInfo, nullptr, &_RenderPass);
		SRX_ASSERT(result == VK_SUCCESS, "创建RenderPass失败!");
	}

	void VulkanSwapchain::CreateFrameBuffers()
	{
		VulkanContext* context = (VulkanContext*)RendererContext::GetContext().get();
		for (auto& frame_buffer : _FrameBuffers)
			vkDestroyFramebuffer(context->GetVkDevice(), frame_buffer, nullptr);

		_FrameBuffers.resize(_ImageViews.size());

		VkFramebufferCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		create_info.renderPass = _RenderPass;
		create_info.attachmentCount = 1;
		create_info.width = _Width;
		create_info.height = _Height;
		create_info.layers = 1;

		for (uint32_t i = 0; i < _FrameBuffers.size(); i++)
		{
			create_info.pAttachments = &_ImageViews[i];
			VkResult result = vkCreateFramebuffer(context->GetVkDevice(), &create_info, nullptr, &_FrameBuffers[i]);
			SRX_ASSERT(result == VK_SUCCESS, "创建FrameBuffer失败!");
		}
	}

	void VulkanSwapchain::CreateCommandBuffer()
	{
		VulkanContext* context = (VulkanContext*)RendererContext::GetContext().get();
		if (_CommandPool != nullptr) 
			vkDestroyCommandPool(context->GetVkDevice(), _CommandPool, nullptr);

		_CommandBuffers.resize(_ImageViews.size());

		VkCommandPoolCreateInfo cmd_pool_info = {};
		cmd_pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		cmd_pool_info.queueFamilyIndex = _GraphicQueueIndex;
		cmd_pool_info.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
		VkResult pool_result = vkCreateCommandPool(
			context->GetVkDevice()
			, &cmd_pool_info
			, nullptr
			, &_CommandPool
		);
		SRX_ASSERT(pool_result == VK_SUCCESS, "创建Command Pool 失败!");

		VkCommandBufferAllocateInfo cmd_buffer_allocate_info = {};
		cmd_buffer_allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		cmd_buffer_allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		cmd_buffer_allocate_info.commandPool = _CommandPool;
		cmd_buffer_allocate_info.commandBufferCount = _ImageViews.size();
		VkResult buffer_result = vkAllocateCommandBuffers(context->GetVkDevice(), &cmd_buffer_allocate_info, _CommandBuffers.data());
		SRX_ASSERT(buffer_result == VK_SUCCESS, "CmdBuffer创建失败!");
		
	}


}