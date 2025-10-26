#include "VulkanContext.h"
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>
#include <chrono>
#include <map>
#include <iostream>
#include <fstream>
#include <set>
#include <array>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"


#include <glm/gtx/quaternion.hpp>

namespace ev
{
	/// <summary>
	/// 验证层的消息回调
	/// </summary>
	static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(
		VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
		VkDebugUtilsMessageTypeFlagsEXT message_type,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData
	)
	{
		std::cerr << "validation layer : " << pCallbackData->pMessage << std::endl;
		std::cerr << std::endl;
		return VK_FALSE;
	}

	VulkanContext::VulkanContext():_camera(), _camera_controller(&_camera)
	{
		_camera.position = glm::vec3(0.0f, 0.0f, 50.0f);
		_camera.rotation = glm::vec3(0.0f, 0.0f, 0.0f);
	}

	void VulkanContext::init(GLFWwindow* window)
	{
		m_window = window;

		_instance.Init(m_window);

		create_swapchain();
		create_img_views();
		create_renderpass();

		create_command_pool();
		create_depth_resource();
		create_framebuffer();
		//create_texture();
		assimp_load_model();
		create_descriptor_layout();
		create_graphic_piple();
		create_plane();
		create_texture_view();
		create_texture_sampler();
		create_vertex_buffer();
		create_descriptor_pool();
		create_descriptor_sets();
		create_command_buffer();
		create_semaphore();
	}

	void VulkanContext::clear()
	{
		vkDestroySemaphore(_instance.GetLogicalDevice(), m_img_avaliable_semaphore, nullptr);
		vkDestroySemaphore(_instance.GetLogicalDevice(), m_render_finish_semaphore, nullptr);
		vkDestroyFence(_instance.GetLogicalDevice(), m_inflight_fence, nullptr);

		vkDestroyBuffer(_instance.GetLogicalDevice(), m_vertex_buffer, nullptr);
		vkFreeMemory(_instance.GetLogicalDevice(), m_vertex_buffer_memory, nullptr);
		vkDestroyBuffer(_instance.GetLogicalDevice(), m_index_buffer, nullptr);
		vkFreeMemory(_instance.GetLogicalDevice(), m_index_buffer_memory, nullptr);
		vkDestroyBuffer(_instance.GetLogicalDevice(), m_uniform_buffer, nullptr);
		vkFreeMemory(_instance.GetLogicalDevice(), m_uniform_memory, nullptr);

		vkDestroySampler(_instance.GetLogicalDevice(), m_texture_sampler, nullptr);
		vkDestroyImageView(_instance.GetLogicalDevice(), m_texture_img_view, nullptr);
		vkDestroyImage(_instance.GetLogicalDevice(), m_texture_img, nullptr);
		vkFreeMemory(_instance.GetLogicalDevice(), m_texture_img_memory, nullptr);

		vkDestroyImage(_instance.GetLogicalDevice(), m_depth_img, nullptr);
		vkFreeMemory(_instance.GetLogicalDevice(), m_depth_img_memory, nullptr);
		vkDestroyImageView(_instance.GetLogicalDevice(), m_depth_img_view, nullptr);

		vkDestroyDescriptorPool(_instance.GetLogicalDevice(), m_descriptor_pool, nullptr);
		vkDestroyDescriptorSetLayout(_instance.GetLogicalDevice(), m_descriptor_layout, nullptr);

		for (auto& view : m_swapchain_imgviews)
		{
			vkDestroyImageView(_instance.GetLogicalDevice(), view, nullptr);
		}

		for (auto& framebuffer : m_swapchain_framebuffers)
		{
			vkDestroyFramebuffer(_instance.GetLogicalDevice(), framebuffer, nullptr);
		}

		vkDestroyCommandPool(_instance.GetLogicalDevice(), m_command_pool, nullptr);

		//清理Vulkan
		vkDestroyRenderPass(_instance.GetLogicalDevice(), m_renderpass, nullptr);
		vkDestroyPipelineLayout(_instance.GetLogicalDevice(), m_pipeline_layout, nullptr);
		vkDestroyPipeline(_instance.GetLogicalDevice(), m_graphic_pipeline, nullptr);
		vkDestroySwapchainKHR(_instance.GetLogicalDevice(), m_swapchain, nullptr);

		_instance.Destroy();
	}

	void VulkanContext::OnEvent(const Event& e)
	{
		_camera_controller.OnEvent(e);

		EventDispathcer dispatch(e);
		dispatch.Dispatch<WindowResizeEvent>(std::bind(&VulkanContext::OnWindowResizeEvent, this, std::placeholders::_1));
	}

	/// <summary>
	/// 1.取图片（异步）
	/// 2.提交命令（异步）
	/// 3.呈现（异步）
	/// 三者之间要同步顺序
	/// </summary>
	void VulkanContext::draw_frame()
	{
		//等待fence变为signaled
		//vkWaitForFences(_instance.GetLogicalDevice(), 1, &m_inflight_fence, VK_TRUE
		//	, std::numeric_limits<uint64_t>::max());
		//vkResetFences(_instance.GetLogicalDevice(), 1, &m_inflight_fence);	//将fence变为unsignaled

		//从交换链获取一张图像
		uint32_t img_index;
		auto result = vkAcquireNextImageKHR(_instance.GetLogicalDevice(), m_swapchain, std::numeric_limits<uint64_t>::max(),
			m_img_avaliable_semaphore, VK_NULL_HANDLE, &img_index);

		//当KHR无法使用或者不匹配的时候直接重建交换链，并退出当前的绘制
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_is_wanna_recreate_swapchain)
		{
			m_is_wanna_recreate_swapchain = false;
			printf("**********************recreate_swapchain!\n");
			recreate_swapchain();
			return;
		}

		update_uniform_data();

		//执行指令
		VkSubmitInfo submit_info = {};
		submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		VkSemaphore wait_semaphores[] = { m_img_avaliable_semaphore };
		VkPipelineStageFlags wait_stages[] = {
			VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
		};
		submit_info.waitSemaphoreCount = 1;
		submit_info.pWaitSemaphores = wait_semaphores;
		submit_info.pWaitDstStageMask = wait_stages;

		submit_info.commandBufferCount = 1;
		submit_info.pCommandBuffers = &m_command_buffers[img_index];

		//指令结束发出信号（相当于信号量+1）
		VkSemaphore signalSemaphores[] = { m_render_finish_semaphore };
		submit_info.signalSemaphoreCount = 1;
		submit_info.pSignalSemaphores = signalSemaphores;

		//调用指令需要一个fence,要求fence是unsignaled状态
		if (vkQueueSubmit(_instance.GetGraphicQueue(), 1, &submit_info, VK_NULL_HANDLE) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to queue submit!");
		}

		//返回渲染后的图像到交换链进行呈现
		VkPresentInfoKHR present_info = {};
		present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		present_info.waitSemaphoreCount = 1;
		present_info.pWaitSemaphores = signalSemaphores;

		VkSwapchainKHR swap_chains[] = { m_swapchain };
		present_info.swapchainCount = 1;
		present_info.pSwapchains = swap_chains;
		present_info.pImageIndices = &img_index;

		vkQueuePresentKHR(_instance.GetPresentQueue(), &present_info);
		vkQueueWaitIdle(_instance.GetPresentQueue());
	}

	void VulkanContext::recreate_swapchain()
	{
		int width = 0, height = 0;

		//最小化窗口时，不冲击swapchain
		while (width == 0 || height == 0)
		{
			glfwGetFramebufferSize(m_window, &width, &height);
			glfwWaitEvents();
		}

		vkDeviceWaitIdle(_instance.GetLogicalDevice());		//等待执行结束

		vkDestroySemaphore(_instance.GetLogicalDevice(), m_img_avaliable_semaphore, nullptr);
		vkDestroySemaphore(_instance.GetLogicalDevice(), m_render_finish_semaphore, nullptr);
		vkDestroyFence(_instance.GetLogicalDevice(), m_inflight_fence, nullptr);

		vkDestroyImage(_instance.GetLogicalDevice(), m_depth_img, nullptr);
		vkFreeMemory(_instance.GetLogicalDevice(), m_depth_img_memory, nullptr);
		vkDestroyImageView(_instance.GetLogicalDevice(), m_depth_img_view, nullptr);

		for (size_t i = 0; i < m_swapchain_framebuffers.size(); i++)
		{
			vkDestroyFramebuffer(_instance.GetLogicalDevice(), m_swapchain_framebuffers[i], nullptr);
		}
		vkFreeCommandBuffers(_instance.GetLogicalDevice(), m_command_pool, static_cast<uint32_t>(m_command_buffers.size())
			, m_command_buffers.data());

		vkDestroyPipeline(_instance.GetLogicalDevice(), m_graphic_pipeline, nullptr);
		vkDestroyPipelineLayout(_instance.GetLogicalDevice(), m_pipeline_layout, nullptr);
		vkDestroyRenderPass(_instance.GetLogicalDevice(), m_renderpass, nullptr);

		for (size_t i = 0; i < m_swapchain_imgviews.size(); i++)
		{
			vkDestroyImageView(_instance.GetLogicalDevice(), m_swapchain_imgviews[i], nullptr);
		}

		vkDestroySwapchainKHR(_instance.GetLogicalDevice(), m_swapchain, nullptr);

		create_swapchain();
		create_img_views();
		create_renderpass();
		create_graphic_piple();
		create_depth_resource();
		create_framebuffer();
		create_command_buffer();
		create_semaphore();
	}

	#pragma region step

	//创建交换链和其img
	void VulkanContext::create_swapchain()
	{
		//首先第一件事，就是从物理显卡支持的swapchain设置中，挑出合适的
		VkExtent2D choose_extent;
		VkSurfaceFormatKHR choose_format;
		VkPresentModeKHR choose_present_mode;

		//选择format
		{
			if (_instance.GetDevice().device_info.formats.size() == 1 && _instance.GetDevice().device_info.formats[0].format == VK_FORMAT_UNDEFINED)
			{
				choose_format = { VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
			}

			bool is_have_format = false;
			for (const auto& format : _instance.GetDevice().device_info.formats)
			{
				if (format.format == VK_FORMAT_B8G8R8A8_UNORM && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
				{
					choose_format = format;
					is_have_format = true;
					break;
				}
			}

			if (!is_have_format) choose_format = _instance.GetDevice().device_info.formats[0];
		}

		//选择mode
		{
			bool is_have_mode = false;
			for (const auto& mode : _instance.GetDevice().device_info.present_modes)
			{
				if (mode == VK_PRESENT_MODE_MAILBOX_KHR)
				{
					choose_present_mode = mode;
					is_have_mode = true;
					break;
				}
			}
			if (!is_have_mode) choose_present_mode = VK_PRESENT_MODE_FIFO_KHR;	//所有设备都支持这个
		}

		//选择extent
		{
			//窗口大小改变后，要重新获取其extent
			vkGetPhysicalDeviceSurfaceCapabilitiesKHR(_instance.GetPhysicalDevice(), _instance.GetSurfaceKHR(), &_instance.GetDeviceP()->device_info.capabilities);

			if (_instance.GetDevice().device_info.capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
			{
				choose_extent = _instance.GetDevice().device_info.capabilities.currentExtent;
			}
			else
			{
				int width, height;
				glfwGetFramebufferSize(m_window, &width, &height);

				choose_extent = { static_cast<uint32_t>(width), static_cast<uint32_t>(height) };
				choose_extent.width = std::max(_instance.GetDevice().device_info.capabilities.minImageExtent.width
					, std::min(_instance.GetDevice().device_info.capabilities.maxImageExtent.width, choose_extent.width));

				choose_extent.height = std::max(_instance.GetDevice().device_info.capabilities.minImageExtent.height
					, std::min(_instance.GetDevice().device_info.capabilities.maxImageExtent.height, choose_extent.height));
			}
		}

		uint32_t img_count = _instance.GetDevice().device_info.capabilities.minImageCount + 1;
		if (_instance.GetDevice().device_info.capabilities.maxImageCount > 0 &&
			img_count > _instance.GetDevice().device_info.capabilities.maxImageCount)
			img_count = _instance.GetDevice().device_info.capabilities.maxImageCount;

		VkSwapchainCreateInfoKHR create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		create_info.surface = _instance.GetSurfaceKHR();
		create_info.minImageCount = img_count;
		create_info.imageFormat = choose_format.format;
		create_info.imageColorSpace = choose_format.colorSpace;
		create_info.imageExtent = choose_extent;
		create_info.imageArrayLayers = 1;
		create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		uint32_t queueFamilyIndices[] = { _instance.GetDevice().device_info.graphic_queue_index, _instance.GetDevice().device_info.present_queue_index };

		if (_instance.GetDevice().device_info.graphic_queue_index != _instance.GetDevice().device_info.present_queue_index)
		{
			create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			create_info.queueFamilyIndexCount = 2;
			create_info.pQueueFamilyIndices = queueFamilyIndices;
		}
		else
		{
			create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			create_info.queueFamilyIndexCount = 0;
			create_info.pQueueFamilyIndices = nullptr;
		}
		create_info.preTransform = _instance.GetDevice().device_info.capabilities.currentTransform;
		create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		create_info.presentMode = choose_present_mode;
		create_info.clipped = VK_TRUE;
		create_info.oldSwapchain = VK_NULL_HANDLE;

		if (vkCreateSwapchainKHR(_instance.GetLogicalDevice(), &create_info, nullptr, &m_swapchain) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create swapchain KHR");
		}

		//获取交换链中图片句柄
		uint32_t swap_chain_img_count;
		vkGetSwapchainImagesKHR(_instance.GetLogicalDevice(), m_swapchain, &swap_chain_img_count, nullptr);
		m_swapchain_imgs.resize(swap_chain_img_count);
		vkGetSwapchainImagesKHR(_instance.GetLogicalDevice(), m_swapchain, &swap_chain_img_count, m_swapchain_imgs.data());

		m_swapchain_format = choose_format.format;
		m_swapchain_extent = choose_extent;
	}

	//为交换链的图片创建句柄
	void VulkanContext::create_img_views()
	{
		m_swapchain_imgviews.resize(m_swapchain_imgs.size());
		for (int i = 0; i < m_swapchain_imgs.size(); i++)
		{
			VkImageViewCreateInfo create_info = {};
			create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			create_info.image = m_swapchain_imgs[i];
			create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
			create_info.format = m_swapchain_format;

			create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

			create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			create_info.subresourceRange.baseMipLevel = 0;
			create_info.subresourceRange.levelCount = 1;
			create_info.subresourceRange.baseArrayLayer = 0;
			create_info.subresourceRange.layerCount = 1;

			if (vkCreateImageView(_instance.GetLogicalDevice(), &create_info, nullptr, &m_swapchain_imgviews[i])
				!= VK_SUCCESS)
			{
				throw std::runtime_error("failed to create image view!");
			}
		}
	}

	VkFormat VulkanContext::findSupportedFormat(
		const std::vector<VkFormat>& candidates,
		VkImageTiling tiling,
		VkFormatFeatureFlags features)
	{
		for (VkFormat format : candidates) {
			VkFormatProperties props;
			vkGetPhysicalDeviceFormatProperties(_instance.GetPhysicalDevice(), format, &props);

			if (tiling == VK_IMAGE_TILING_LINEAR &&
				(props.linearTilingFeatures & features) == features)
			{
				return format;
			}
			else if (tiling == VK_IMAGE_TILING_OPTIMAL &&
				(props.optimalTilingFeatures & features) == features)
			{
				return format;
			}
		}

		throw std::runtime_error("failed to find supported format!");
	}


	void VulkanContext::create_renderpass()
	{
		VkAttachmentDescription color_attachment = {};
		color_attachment.format = m_swapchain_format;
		color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
		color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;	//每次渲染前一帧清楚帧缓冲
		color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;	//渲染的内容会被存储起来

		//对模板缓冲不关心（暂时）
		color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

		color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;		//表示不关心渲染前的图像布局
		color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;		//图像被用作在交换链中呈现

		VkAttachmentReference color_attachment_ref = {};
		color_attachment_ref.attachment = 0;	//attachment description的索引
		color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkAttachmentDescription depth_attachment = {};
		depth_attachment.format = findSupportedFormat({ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT }, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
		depth_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
		depth_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		depth_attachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depth_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		depth_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depth_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		depth_attachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkAttachmentReference depth_attachment_ref = {};
		depth_attachment_ref.attachment = 1;
		depth_attachment_ref.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpass = {};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &color_attachment_ref;
		subpass.pDepthStencilAttachment = &depth_attachment_ref;

		std::array<VkAttachmentDescription, 2> attachments = {
			color_attachment, depth_attachment
		};

		//配置子流程依赖
		VkSubpassDependency dependency = {};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;	//等待颜色附着输出的阶段
		dependency.srcAccessMask = 0;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		VkRenderPassCreateInfo renderpass_create_info = {};
		renderpass_create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderpass_create_info.attachmentCount = static_cast<uint32_t>(attachments.size());
		renderpass_create_info.pAttachments = attachments.data();
		renderpass_create_info.subpassCount = 1;
		renderpass_create_info.pSubpasses = &subpass;
		renderpass_create_info.dependencyCount = 1;
		renderpass_create_info.pDependencies = &dependency;

		if (vkCreateRenderPass(_instance.GetLogicalDevice(), &renderpass_create_info, nullptr, &m_renderpass) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create render pass!");
		}
	}

	void VulkanContext::create_descriptor_layout()
	{
		//uniform
		VkDescriptorSetLayoutBinding ubo_layout_binding = {};
		ubo_layout_binding.binding = 0;
		ubo_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		ubo_layout_binding.descriptorCount = 1;
		ubo_layout_binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;		//表明在顶点着色器阶段使用

		//sampler
		VkDescriptorSetLayoutBinding sampler_layout_binding = {};
		sampler_layout_binding.binding = 1;
		sampler_layout_binding.descriptorCount = 1;
		sampler_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		sampler_layout_binding.pImmutableSamplers = nullptr;
		sampler_layout_binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;	//在片段着色器阶段使用

		std::array<VkDescriptorSetLayoutBinding, 2> bindings = {
			ubo_layout_binding, sampler_layout_binding
		};

		VkDescriptorSetLayoutCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		create_info.bindingCount = static_cast<uint32_t>(bindings.size());
		create_info.pBindings = bindings.data();

		if (vkCreateDescriptorSetLayout(_instance.GetLogicalDevice(), &create_info, nullptr, &m_descriptor_layout) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create descriptor set layout!");
		}
	}

	void VulkanContext::create_graphic_piple()
	{
		//准备Shader Module
		std::vector<char> vert_source;
		std::vector<char> frag_source;

		read_shader("assets/shaders/vert.spv", vert_source);
		read_shader("assets/shaders/frag.spv", frag_source);

		VkShaderModule vertex_shader_module = create_shader_module(vert_source);
		VkShaderModule frag_shader_module = create_shader_module(frag_source);

		//指定shader在哪个着色器阶段使用
		VkPipelineShaderStageCreateInfo vert_shader_stage_create_info = {};
		vert_shader_stage_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vert_shader_stage_create_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
		vert_shader_stage_create_info.module = vertex_shader_module;
		vert_shader_stage_create_info.pName = "main";

		VkPipelineShaderStageCreateInfo frag_shader_stage_create_info = {};
		frag_shader_stage_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		frag_shader_stage_create_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		frag_shader_stage_create_info.module = frag_shader_module;
		frag_shader_stage_create_info.pName = "main";

		VkPipelineShaderStageCreateInfo shaderStages[] =
		{
			vert_shader_stage_create_info,
			frag_shader_stage_create_info
		};

		//顶点绑定描述
		VkVertexInputBindingDescription bind_descriptions = {};
		bind_descriptions.binding = 0;
		bind_descriptions.stride = sizeof(SkinnedMesh::Vertex);
		bind_descriptions.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		//顶点属性描述
		std::array<VkVertexInputAttributeDescription, 5> attribute_descriptions{};
		attribute_descriptions[0].binding = 0;
		attribute_descriptions[0].location = 0;
		attribute_descriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		attribute_descriptions[0].offset = 0;

		attribute_descriptions[1].binding = 0;
		attribute_descriptions[1].location = 1;
		attribute_descriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attribute_descriptions[1].offset = offsetof(SkinnedMesh::Vertex, color);

		attribute_descriptions[2].binding = 0;
		attribute_descriptions[2].location = 2;
		attribute_descriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
		attribute_descriptions[2].offset = offsetof(SkinnedMesh::Vertex, tex_coord);

		attribute_descriptions[3].binding = 0;
		attribute_descriptions[3].location = 3;
		attribute_descriptions[3].format = VK_FORMAT_R32G32B32A32_SINT;
		attribute_descriptions[3].offset = offsetof(SkinnedMesh::Vertex, bone_ids);

		attribute_descriptions[4].binding = 0;
		attribute_descriptions[4].location = 4;
		attribute_descriptions[4].format = VK_FORMAT_R32G32B32A32_SFLOAT;
		attribute_descriptions[4].offset = offsetof(SkinnedMesh::Vertex, weights);

		//1.顶点输入
		//指定传给顶点着色器地顶点数据的格式
		VkPipelineVertexInputStateCreateInfo vertex_input_state_create_info = {};
		vertex_input_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertex_input_state_create_info.vertexBindingDescriptionCount = 1;
		vertex_input_state_create_info.pVertexBindingDescriptions = &bind_descriptions;
		vertex_input_state_create_info.vertexAttributeDescriptionCount = static_cast<uint32_t>(attribute_descriptions.size());
		vertex_input_state_create_info.pVertexAttributeDescriptions = attribute_descriptions.data();

		//2.输入装配
		//定义了哪几种类型的图元
		//是否启用几何图元重启
		VkPipelineInputAssemblyStateCreateInfo assembly_create_info = {};
		assembly_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		assembly_create_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		assembly_create_info.primitiveRestartEnable = VK_FALSE;

		//3.视口和裁剪

		//设置视口
		VkViewport viewport = {};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = (float)m_swapchain_extent.width;
		viewport.height = (float)m_swapchain_extent.height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		//设置裁剪
		VkRect2D scissor = {};
		scissor.offset = { 0, 0 };
		scissor.extent = m_swapchain_extent;

		VkPipelineViewportStateCreateInfo viewport_state_create_info = {};
		viewport_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewport_state_create_info.viewportCount = 1;
		viewport_state_create_info.pViewports = &viewport;
		viewport_state_create_info.scissorCount = 1;
		viewport_state_create_info.pScissors = &scissor;

		//4.光栅化
		VkPipelineRasterizationStateCreateInfo rasterization_create_info = {};
		rasterization_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterization_create_info.depthClampEnable = VK_FALSE;		//表明丢弃远近平面外的东西，并不截断为远近平面
		rasterization_create_info.rasterizerDiscardEnable = VK_FALSE;		//如果是True，则禁止一切片段输出到帧缓冲
		rasterization_create_info.lineWidth = 1.0f;		//如果是True，则禁止一切片段输出到帧缓冲
		rasterization_create_info.cullMode = VK_CULL_MODE_BACK_BIT;		//背面剔除
		rasterization_create_info.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;		//指定顺时针的顶点顺序为正面

		rasterization_create_info.depthBiasEnable = VK_FALSE;	//是否将片段所处线段的斜率？放到深度值上？
		rasterization_create_info.depthBiasConstantFactor = 0.0f;
		rasterization_create_info.depthBiasClamp = 0.0f;
		rasterization_create_info.depthBiasSlopeFactor = 0.0f;

		//5.多重采样
		VkPipelineMultisampleStateCreateInfo multi_sample_create_info = {};
		multi_sample_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multi_sample_create_info.sampleShadingEnable = VK_TRUE;	//禁用多重采样
		multi_sample_create_info.minSampleShading = 0.2f;
		multi_sample_create_info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;	//采样一次？

		//6.深度和模板测试
		VkPipelineDepthStencilStateCreateInfo depthStencil{};
		depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		depthStencil.depthTestEnable = VK_TRUE;
		depthStencil.depthWriteEnable = VK_TRUE;
		depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
		depthStencil.depthBoundsTestEnable = VK_FALSE;
		depthStencil.stencilTestEnable = VK_FALSE;

		//7.颜色混合
		VkPipelineColorBlendAttachmentState color_blend_attachment = {};
		color_blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT
			| VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT |
			VK_COLOR_COMPONENT_A_BIT;
		color_blend_attachment.blendEnable = VK_FALSE;		//暂时禁用颜色混合

		VkPipelineColorBlendStateCreateInfo color_blend_create_info = {};
		color_blend_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		color_blend_create_info.logicOpEnable = VK_FALSE;
		color_blend_create_info.logicOp = VK_LOGIC_OP_COPY;
		color_blend_create_info.attachmentCount = 1;
		color_blend_create_info.pAttachments = &color_blend_attachment;

		//8.动态状态
		VkDynamicState dynamicStates[] = {
			VK_DYNAMIC_STATE_VIEWPORT,		//视口变换
			VK_DYNAMIC_STATE_LINE_WIDTH		//线宽
		};

		VkPipelineDynamicStateCreateInfo dynamic_create_info = {};
		dynamic_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamic_create_info.dynamicStateCount = 0;	//sizeof(dynamicStates) / sizeof(VkDynamicState);	//暂时不开启
		dynamic_create_info.pDynamicStates = dynamicStates;

		//9.管线布局(uniform)
		VkPipelineLayoutCreateInfo pipeline_layout_create_info = {};
		pipeline_layout_create_info.sType =
			VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipeline_layout_create_info.setLayoutCount = 1; 
		pipeline_layout_create_info.pSetLayouts = &m_descriptor_layout;
		pipeline_layout_create_info.pushConstantRangeCount = 0; // Optional
		pipeline_layout_create_info.pPushConstantRanges = nullptr; // Optional

		if (vkCreatePipelineLayout(_instance.GetLogicalDevice(), &pipeline_layout_create_info, nullptr,
			&m_pipeline_layout) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create pipeline layout!");
		}

		//创建渲染管线
		VkGraphicsPipelineCreateInfo pipeline_create_info = {};
		pipeline_create_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipeline_create_info.stageCount = 2;	//两个着色器阶段
		pipeline_create_info.pStages = shaderStages;

		pipeline_create_info.pVertexInputState = &vertex_input_state_create_info;
		pipeline_create_info.pInputAssemblyState = &assembly_create_info;
		pipeline_create_info.pViewportState = &viewport_state_create_info;
		pipeline_create_info.pRasterizationState = &rasterization_create_info;
		pipeline_create_info.pMultisampleState = &multi_sample_create_info;
		pipeline_create_info.pDepthStencilState = &depthStencil;
		pipeline_create_info.pColorBlendState = &color_blend_create_info;
		pipeline_create_info.pDynamicState = &dynamic_create_info;

		pipeline_create_info.layout = m_pipeline_layout;
		pipeline_create_info.renderPass = m_renderpass;
		pipeline_create_info.subpass = 0;	//子流程的索引

		pipeline_create_info.basePipelineHandle = VK_NULL_HANDLE; // Optional
		pipeline_create_info.basePipelineIndex = -1; // Optional

		if (vkCreateGraphicsPipelines(_instance.GetLogicalDevice(), VK_NULL_HANDLE, 1, &pipeline_create_info
			, nullptr, &m_graphic_pipeline) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create graphic pipeline!");
		}

		//创建了管线之后可以销毁shader module了
		vkDestroyShaderModule(_instance.GetLogicalDevice(), vertex_shader_module, nullptr);
		vkDestroyShaderModule(_instance.GetLogicalDevice(), frag_shader_module, nullptr);
	}

	//为交换链的图片创建帧缓冲
	void VulkanContext::create_framebuffer()
	{
		m_swapchain_framebuffers.resize(m_swapchain_imgviews.size());
		for (int i = 0; i < m_swapchain_imgviews.size(); i++)
		{
			std::array<VkImageView, 2> attachments = {
				m_swapchain_imgviews[i],
				m_depth_img_view
			};

			VkFramebufferCreateInfo framebuffer_create_info = {};
			framebuffer_create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebuffer_create_info.renderPass = m_renderpass;
			framebuffer_create_info.attachmentCount = static_cast<uint32_t>(attachments.size());
			framebuffer_create_info.pAttachments = attachments.data();
			framebuffer_create_info.width = m_swapchain_extent.width;
			framebuffer_create_info.height = m_swapchain_extent.height;
			framebuffer_create_info.layers = 1;

			if (vkCreateFramebuffer(_instance.GetLogicalDevice(), &framebuffer_create_info, nullptr, &m_swapchain_framebuffers[i])
				!= VK_SUCCESS)
			{
				throw std::runtime_error("failed to create framebuffer!");
			}
		}
	}

	//开始创建渲染命令
	void VulkanContext::create_command_pool()
	{
		VkCommandPoolCreateInfo command_pool_create_info = {};
		command_pool_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		command_pool_create_info.queueFamilyIndex = _instance.GetDevice().device_info.graphic_queue_index;
		command_pool_create_info.flags = 0;

		if (vkCreateCommandPool(_instance.GetLogicalDevice(), &command_pool_create_info, nullptr, &m_command_pool)
			!= VK_SUCCESS)
		{
			throw std::runtime_error("failed to create command pool!");
		}
	}

	void VulkanContext::create_depth_resource()
	{
		VkFormat depth_format = findSupportedFormat({ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT }, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);

		create_image(m_swapchain_extent.width, m_swapchain_extent.height, depth_format
			, VK_IMAGE_TILING_OPTIMAL
			, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT
			, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			, m_depth_img
			, m_depth_img_memory
			);
		
		VkImageViewCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		create_info.image = m_depth_img;
		create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
		create_info.format = depth_format;
		create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
		create_info.subresourceRange.baseMipLevel = 0;
		create_info.subresourceRange.levelCount = 1;
		create_info.subresourceRange.baseArrayLayer = 0;
		create_info.subresourceRange.layerCount = 1;

		if (vkCreateImageView(_instance.GetLogicalDevice(), &create_info, nullptr, &m_depth_img_view) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create texture img_view!");
		}

		tansition_img_layout(m_depth_img, depth_format,
			VK_IMAGE_LAYOUT_UNDEFINED,
			VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
	}

	void VulkanContext::create_texture()
	{
		int tex_width, tex_height, tex_channels;
		stbi_uc* pixels = stbi_load("assets/models/amiya/body_tex.png", &tex_width, &tex_height, &tex_channels
			, STBI_rgb_alpha);		//使用了STBI_rgb_alpha，即使图片不包含alpha通道，也会强制加上
		VkDeviceSize img_size = tex_width * tex_height * 4;		//一个像素需要一个字节0~255

		//暂存缓冲
		VkBuffer staging_buffer;
		VkDeviceMemory staging_memory;

		create_buffer(img_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT
			, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
			, staging_buffer
			, staging_memory
			);

		void* data;
		vkMapMemory(_instance.GetLogicalDevice(), staging_memory, 0, img_size, 0, &data);
		memcpy(data, pixels, img_size);
		vkUnmapMemory(_instance.GetLogicalDevice(), staging_memory);

		stbi_image_free(pixels);	//释放图像数据

		create_image(tex_width, tex_height
			, VK_FORMAT_R8G8B8A8_UNORM
			, VK_IMAGE_TILING_OPTIMAL
			, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT
			, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			, m_texture_img
			, m_texture_img_memory
		);

		//把数据从暂存缓冲传入到image中
		tansition_img_layout(m_texture_img, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		copy_buffer_image(staging_buffer, m_texture_img, tex_width, tex_height);
		tansition_img_layout(m_texture_img, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

		vkDestroyBuffer(_instance.GetLogicalDevice(), staging_buffer, nullptr);
		vkFreeMemory(_instance.GetLogicalDevice(), staging_memory, nullptr);
	}

	void VulkanContext::create_texture_view()
	{
		VkImageViewCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		create_info.image = m_texture_img;
		create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
		create_info.format = VK_FORMAT_R8G8B8A8_UNORM;
		create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		create_info.subresourceRange.baseMipLevel = 0;
		create_info.subresourceRange.levelCount = 1;
		create_info.subresourceRange.baseArrayLayer = 0;
		create_info.subresourceRange.layerCount = 1;

		if (vkCreateImageView(_instance.GetLogicalDevice(), &create_info, nullptr, &m_texture_img_view) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create texture img_view!");
		}
	}

	void VulkanContext::create_texture_sampler()
	{
		VkSamplerCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		//过滤方式
		create_info.magFilter = VK_FILTER_LINEAR;
		create_info.minFilter = VK_FILTER_LINEAR;

		//寻址方式
		create_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		create_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		create_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

		//各向异性过滤
		create_info.anisotropyEnable = VK_TRUE;
		create_info.maxAnisotropy = 16;

		create_info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;

		create_info.unnormalizedCoordinates = VK_FALSE;		//UV的坐标系统(是否非标准化)

		create_info.compareEnable = VK_FALSE;
		create_info.compareOp = VK_COMPARE_OP_ALWAYS;

		create_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		create_info.mipLodBias = 0.0f;
		create_info.minLod = 0.0f;
		create_info.maxLod = 0.0f;

		if (vkCreateSampler(_instance.GetLogicalDevice(), &create_info, nullptr, &m_texture_sampler) != VK_SUCCESS)
		{
			throw std::runtime_error("falied to create sampler!");
		}
	}

	void VulkanContext::create_vertex_buffer()
	{
		VkBuffer staging_buffer;
		VkDeviceMemory staging_memory;

		create_buffer(sizeof(SkinnedMesh::Vertex) * _skinned_mesh.GetVerticesSize(), VK_BUFFER_USAGE_TRANSFER_SRC_BIT
			, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT	//CPU可见/CPU和GPU缓存保持一致
			, staging_buffer, staging_memory
		);

		//往暂存缓冲中填充数据
		void* data;
		vkMapMemory(_instance.GetLogicalDevice(), staging_memory, 0, sizeof(SkinnedMesh::Vertex) * _skinned_mesh.GetVerticesSize(), 0, &data);	//memory映射到cpu可以访问的内存中
		memcpy(data, _skinned_mesh.GetVerticesData(), sizeof(SkinnedMesh::Vertex) * _skinned_mesh.GetVerticesSize());
		vkUnmapMemory(_instance.GetLogicalDevice(), staging_memory);

		create_buffer(sizeof(SkinnedMesh::Vertex) * _skinned_mesh.GetVerticesSize()
			, (VkBufferUsageFlagBits)(VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT)
			, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT		//表明这块内存是GPU内部
			, m_vertex_buffer, m_vertex_buffer_memory
			);

		//将暂存buffer的数据传入到vertex buffer中
		copy_buffer(staging_buffer, m_vertex_buffer, sizeof(SkinnedMesh::Vertex) * _skinned_mesh.GetVerticesSize());

		vkDestroyBuffer(_instance.GetLogicalDevice(), staging_buffer, nullptr);
		vkFreeMemory(_instance.GetLogicalDevice(), staging_memory, nullptr);

		//*********************************************** Index buffer *****************************************************

		create_buffer(sizeof(uint32_t) * _skinned_mesh.GetIndicesSize(), VK_BUFFER_USAGE_TRANSFER_SRC_BIT
			, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT	//CPU可见/CPU和GPU缓存保持一致
			, staging_buffer, staging_memory
		);

		//往暂存缓冲中填充数据
		vkMapMemory(_instance.GetLogicalDevice(), staging_memory, 0, sizeof(uint32_t) * _skinned_mesh.GetIndicesSize(), 0, &data);	//memory映射到cpu可以访问的内存中
		memcpy(data, _skinned_mesh.GetIndicesData(), sizeof(uint32_t) * _skinned_mesh.GetIndicesSize());
		vkUnmapMemory(_instance.GetLogicalDevice(), staging_memory);

		create_buffer(sizeof(uint32_t) * _skinned_mesh.GetIndicesSize()
			, (VkBufferUsageFlagBits)(VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT)
			, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT		//表明这块内存是GPU内部
			, m_index_buffer, m_index_buffer_memory
		);

		//将暂存buffer的数据传入到index buffer中
		copy_buffer(staging_buffer, m_index_buffer, sizeof(uint32_t) * _skinned_mesh.GetIndicesSize());

		vkDestroyBuffer(_instance.GetLogicalDevice(), staging_buffer, nullptr);
		vkFreeMemory(_instance.GetLogicalDevice(), staging_memory, nullptr);

		//********************************************* Uniform buffer ***************************************************

		create_buffer(sizeof(UniformBufferObject), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT
			, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
			, m_uniform_buffer
			, m_uniform_memory
			);
	}

	void VulkanContext::load_model()
	{
		/*tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string err;
		if (!tinyobj::LoadObj(&attrib, &shapes, &materials, nullptr, &err, "assets/models/amiya/amiya.obj"))
		{
			throw std::runtime_error(err);
		}

		for (const auto& shape : shapes)
		{
			for (const auto& index : shape.mesh.indices)
			{
				Vertex vertex = {};

				vertex.pos =
				{
					attrib.vertices[3 * index.vertex_index + 0],
					attrib.vertices[3 * index.vertex_index + 1],
					attrib.vertices[3 * index.vertex_index + 2]
				};

				vertex.tex_coord =
				{
					attrib.texcoords[2 * index.texcoord_index + 0],
					1.0f - attrib.texcoords[2 * index.texcoord_index + 1],
				};

				vertex.color = { 1.0f, 1.0f, 1.0f };

				vertices.push_back(vertex);
				indices.push_back(indices.size());
			}
		}*/
	}

	void VulkanContext::assimp_load_model()
	{
		const aiScene* scene = _importer.ReadFile(
			"assets/models/racer/Silly Dancing.fbx"
			, aiProcess_Triangulate | aiProcess_LimitBoneWeights
			| aiProcess_JoinIdenticalVertices | aiProcess_GenSmoothNormals
			| aiProcess_CalcTangentSpace
			);

		if (scene == nullptr)
		{
			throw std::runtime_error("failed to load assimp model!");
		}

		//加载纹理
		aiTexture* diffuse_tex = scene->mTextures[1];

		int img_width, img_height, img_channels;
		//要使用stb解码
		unsigned char* pixels = stbi_load_from_memory(reinterpret_cast<unsigned char*>(diffuse_tex->pcData)
			, diffuse_tex->mWidth, &img_width, &img_height, &img_channels, STBI_rgb_alpha);

		VkDeviceSize img_size = img_width * img_height * 4;

		//暂存缓冲
		VkBuffer staging_buffer;
		VkDeviceMemory staging_memory;

		create_buffer(img_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT
			, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
			, staging_buffer
			, staging_memory
		);

		void* data;
		vkMapMemory(_instance.GetLogicalDevice(), staging_memory, 0, img_size, 0, &data);
		memcpy(data, pixels, img_size);
		vkUnmapMemory(_instance.GetLogicalDevice(), staging_memory);

		stbi_image_free(pixels);

		create_image(img_width, img_height
			, VK_FORMAT_R8G8B8A8_UNORM
			, VK_IMAGE_TILING_OPTIMAL
			, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT
			, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			, m_texture_img
			, m_texture_img_memory
		);

		//把数据从暂存缓冲传入到image中
		tansition_img_layout(m_texture_img, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		copy_buffer_image(staging_buffer, m_texture_img, img_width, img_height);
		tansition_img_layout(m_texture_img, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

		vkDestroyBuffer(_instance.GetLogicalDevice(), staging_buffer, nullptr);
		vkFreeMemory(_instance.GetLogicalDevice(), staging_memory, nullptr);

		_skinned_mesh.LoadMesh(scene, scene->mMeshes[0]);
		_skinned_mesh.LoadAnimation(scene->mAnimations[0]);
	}

	void VulkanContext::create_plane()
	{
	}

	void VulkanContext::create_descriptor_pool()
	{
		std::array<VkDescriptorPoolSize, 2> pool_sizes = {};
		pool_sizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		pool_sizes[0].descriptorCount = 1;

		pool_sizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		pool_sizes[1].descriptorCount = 1;

		VkDescriptorPoolCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		create_info.poolSizeCount = static_cast<uint32_t>(pool_sizes.size());
		create_info.pPoolSizes = pool_sizes.data();

		create_info.maxSets = 1;

		if (vkCreateDescriptorPool(_instance.GetLogicalDevice(), &create_info, nullptr, &m_descriptor_pool) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create descriptor pool!");
		}
	}

	void VulkanContext::create_descriptor_sets()
	{
		VkDescriptorSetAllocateInfo alloc_info = {};
		alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		alloc_info.descriptorPool = m_descriptor_pool;
		alloc_info.descriptorSetCount = 1;
		alloc_info.pSetLayouts = &m_descriptor_layout;

		if (vkAllocateDescriptorSets(_instance.GetLogicalDevice(), &alloc_info, &m_descriptor_set) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to allocate descriptor set!");
		}

		VkDescriptorBufferInfo buffer_info = {};
		buffer_info.buffer = m_uniform_buffer;
		buffer_info.offset = 0;
		buffer_info.range = sizeof(UniformBufferObject);

		VkDescriptorImageInfo img_info = {};
		img_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		img_info.imageView = m_texture_img_view;
		img_info.sampler = m_texture_sampler;

		std::array<VkWriteDescriptorSet, 2> descriptor_writes = {};

		//更新描述符的配置
		descriptor_writes[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptor_writes[0].dstSet = m_descriptor_set;
		descriptor_writes[0].dstBinding = 0;
		descriptor_writes[0].dstArrayElement = 0;
		descriptor_writes[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptor_writes[0].descriptorCount = 1;
		descriptor_writes[0].pBufferInfo = &buffer_info;

		descriptor_writes[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptor_writes[1].dstSet = m_descriptor_set;
		descriptor_writes[1].dstBinding = 1;
		descriptor_writes[1].dstArrayElement = 0;
		descriptor_writes[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptor_writes[1].descriptorCount = 1;
		descriptor_writes[1].pImageInfo = &img_info;

		vkUpdateDescriptorSets(_instance.GetLogicalDevice(), static_cast<uint32_t>(descriptor_writes.size()), descriptor_writes.data(), 0, nullptr);
	}

	void VulkanContext::create_command_buffer()
	{
		m_command_buffers.resize(m_swapchain_framebuffers.size());

		VkCommandBufferAllocateInfo alloc_info = {};
		alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		alloc_info.commandPool = m_command_pool;
		alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		alloc_info.commandBufferCount = (uint32_t)m_command_buffers.size();

		if (vkAllocateCommandBuffers(_instance.GetLogicalDevice(), &alloc_info, m_command_buffers.data())
			!= VK_SUCCESS)
		{
			throw std::runtime_error("failed to create command buffers!");
		}

		for (size_t i = 0; i < m_command_buffers.size(); i++)
		{
			VkCommandBufferBeginInfo begin_info = {};
			begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			begin_info.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
			begin_info.pInheritanceInfo = nullptr;

			//开始录制操作
			if (vkBeginCommandBuffer(m_command_buffers[i], &begin_info) != VK_SUCCESS)
			{
				throw std::runtime_error("failed to begin command buffers!");
			}

			VkRenderPassBeginInfo renderpass_info = {};
			renderpass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			renderpass_info.renderPass = m_renderpass;
			renderpass_info.framebuffer = m_swapchain_framebuffers[i];
			renderpass_info.renderArea.offset = { 0, 0 };
			renderpass_info.renderArea.extent = m_swapchain_extent;

			std::array<VkClearValue, 2> clear_values = {};
			clear_values[0].color = {0.1f, 0.1f, 0.1f, 1.0f};
			clear_values[1].depthStencil = { 1.0f , 0 };
			renderpass_info.clearValueCount = static_cast<uint32_t>(clear_values.size());
			renderpass_info.pClearValues = clear_values.data();

			//开始renderpass
			vkCmdBeginRenderPass(m_command_buffers[i], &renderpass_info, VK_SUBPASS_CONTENTS_INLINE);

			//绑定渲染管线
			vkCmdBindPipeline(m_command_buffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, m_graphic_pipeline);

			VkBuffer vertex_buffers[] = { m_vertex_buffer };
			VkDeviceSize offsets[] = { 0 };
			vkCmdBindVertexBuffers(m_command_buffers[i], 0, 1, vertex_buffers, offsets);
			vkCmdBindDescriptorSets(m_command_buffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline_layout
				, 0, 1, &m_descriptor_set, 0, nullptr);
			vkCmdBindIndexBuffer(m_command_buffers[i], m_index_buffer, 0, VK_INDEX_TYPE_UINT32);

			//绘制
			vkCmdDrawIndexed(m_command_buffers[i], static_cast<uint32_t>(_skinned_mesh.GetIndicesSize()), 1, 0, 0, 0);

			//结束renderpass
			vkCmdEndRenderPass(m_command_buffers[i]);

			//结束记录指令
			if (vkEndCommandBuffer(m_command_buffers[i]) != VK_SUCCESS)
			{
				throw std::runtime_error("failed to end command buffers!");
			}
		}

	}

	//用于GPU操作的同步
	void VulkanContext::create_semaphore()
	{
		VkSemaphoreCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo fence_create_info = {};
		fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fence_create_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		if (vkCreateSemaphore(_instance.GetLogicalDevice(), &create_info, nullptr, &m_img_avaliable_semaphore) != VK_SUCCESS
			|| vkCreateSemaphore(_instance.GetLogicalDevice(), &create_info, nullptr, &m_render_finish_semaphore) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create semaphore!");
		}

		if (vkCreateFence(_instance.GetLogicalDevice(), &fence_create_info, nullptr, &m_inflight_fence) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create fence!");
		}
	}

	#pragma endregion

	uint32_t VulkanContext::find_memory_type(uint32_t filter, VkMemoryPropertyFlags properties)
	{
		VkPhysicalDeviceMemoryProperties mem_properties;
		vkGetPhysicalDeviceMemoryProperties(_instance.GetPhysicalDevice(), &mem_properties);
		for (uint32_t i = 0; i < mem_properties.memoryTypeCount; i++)
		{
			if ((filter & (1 << i)) && (mem_properties.memoryTypes[i].propertyFlags & properties) == properties)
				return i;
		}
	}

	/// <summary>
	/// 检查物理显卡是否合格
	/// </summary>
	bool VulkanContext::check_device(VkPhysicalDevice device, int* score
		, int* graphic_queue_index, int* present_queue_index
		, VkSurfaceCapabilitiesKHR& capabilities
		, std::vector<VkSurfaceFormatKHR>& formats, std::vector<VkPresentModeKHR>& modes
	)
	{
#pragma region 计算显卡的分数

		{
			//获取Property
			VkPhysicalDeviceProperties property;
			vkGetPhysicalDeviceProperties(device, &property);

			VkPhysicalDeviceFeatures feature;
			vkGetPhysicalDeviceFeatures(device, &feature);

			//计算当前显卡的分数
			*score = 0;
			if (property.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
				*score += 1000;
			*score += property.limits.maxImageDimension2D;
			if (!feature.geometryShader || !feature.samplerAnisotropy)
				*score = 0;

			if (*score == 0)
				return false;
		}

#pragma endregion

#pragma region 检查队列簇是否有图像和显示功能

		{
			//还要求物理显卡有Graphic的功能
			uint32_t queue_family_count;
			vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, nullptr);
			std::vector<VkQueueFamilyProperties> properties(queue_family_count);
			vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, properties.data());

			for (int i = 0; i < properties.size(); i++)
			{
				if (*graphic_queue_index == -1 && properties[i].queueCount > 0
					&& properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
				{
					*graphic_queue_index = i;		//记录下具有Graphic功能的队列簇
				}

				if (*present_queue_index == -1)
				{
					VkBool32 is_present_supported = false;
					vkGetPhysicalDeviceSurfaceSupportKHR(device, i, _instance.GetSurfaceKHR(), &is_present_supported);
					if (is_present_supported)
						*present_queue_index = i;
				}

				if (*graphic_queue_index != -1 && *present_queue_index != -1)
					break;
			}

			if (*graphic_queue_index == -1 || *present_queue_index == -1)
				return false;
		}

#pragma endregion

#pragma region 检查是否支持SwapChain拓展

		{
			uint32_t extension_count;
			vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, nullptr);
			std::vector<VkExtensionProperties> properties(extension_count);
			vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, properties.data());

			bool is_supported_swapchain = false;

			for (const auto& extension : properties)
			{
				if (strcmp(extension.extensionName, VK_KHR_SWAPCHAIN_EXTENSION_NAME) == 0)
				{
					is_supported_swapchain = true;
					break;
				}
			}

			if (!is_supported_swapchain) return false;
		}

#pragma endregion

#pragma region 检查显卡的Swapchain是否与Surface兼容

		{
			vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, _instance.GetSurfaceKHR(), &capabilities);

			uint32_t format_count;
			vkGetPhysicalDeviceSurfaceFormatsKHR(device, _instance.GetSurfaceKHR(), &format_count, nullptr);
			formats.resize(format_count);
			vkGetPhysicalDeviceSurfaceFormatsKHR(device, _instance.GetSurfaceKHR(), &format_count, formats.data());

			uint32_t present_mode_count;
			vkGetPhysicalDeviceSurfacePresentModesKHR(device, _instance.GetSurfaceKHR(), &present_mode_count, nullptr);
			modes.resize(present_mode_count);
			vkGetPhysicalDeviceSurfacePresentModesKHR(device, _instance.GetSurfaceKHR(), &present_mode_count, modes.data());

			if (formats.empty() || modes.empty())
				return false;
		}

#pragma endregion

		return true;
	}

	void VulkanContext::OnUpdate(float delta)
	{
		_camera_controller.OnUpdate(delta);
	}

	void VulkanContext::create_buffer(VkDeviceSize size, VkBufferUsageFlagBits usage, VkMemoryPropertyFlags properties
		, VkBuffer& buffer, VkDeviceMemory& memory)
	{
		VkBufferCreateInfo buffer_create_info = {};
		buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		buffer_create_info.size = size;
		buffer_create_info.usage = usage;
		buffer_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		if (vkCreateBuffer(_instance.GetLogicalDevice(), &buffer_create_info, nullptr, &buffer) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create buffer!");
		}

		//内存
		VkMemoryRequirements mem_requirment;
		vkGetBufferMemoryRequirements(_instance.GetLogicalDevice(), buffer, &mem_requirment);

		VkMemoryAllocateInfo alloc_info = {};
		alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		alloc_info.allocationSize = mem_requirment.size;
		alloc_info.memoryTypeIndex = find_memory_type(mem_requirment.memoryTypeBits
			, properties
			);
		
		if (vkAllocateMemory(_instance.GetLogicalDevice(), &alloc_info, nullptr, &memory) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to allocate memory!");
		}

		vkBindBufferMemory(_instance.GetLogicalDevice(), buffer, memory, 0);
	}

	void VulkanContext::create_image(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling
		, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& memory)
	{
		//开始创建VkImage
		VkImageCreateInfo img_create_info = {};
		img_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		img_create_info.imageType = VK_IMAGE_TYPE_2D;
		img_create_info.extent.width = width;
		img_create_info.extent.height = height;
		img_create_info.extent.depth = 1;
		img_create_info.mipLevels = 1;
		img_create_info.arrayLayers = 1;
		img_create_info.format = format;
		img_create_info.tiling = tiling;	//纹素的排列方式
		img_create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;	//第一次变换后就丢弃？
		img_create_info.usage = usage;
		img_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		img_create_info.samples = VK_SAMPLE_COUNT_1_BIT;

		if (vkCreateImage(_instance.GetLogicalDevice(), &img_create_info, nullptr, &image) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create image!");
		}

		VkMemoryRequirements mem_requirement;
		vkGetImageMemoryRequirements(_instance.GetLogicalDevice(), image, &mem_requirement);

		VkMemoryAllocateInfo alloc_info = {};
		alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		alloc_info.allocationSize = mem_requirement.size;
		alloc_info.memoryTypeIndex = find_memory_type(mem_requirement.memoryTypeBits, properties);

		if (vkAllocateMemory(_instance.GetLogicalDevice(), &alloc_info, nullptr, &memory) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to allocate img memory!");
		}

		vkBindImageMemory(_instance.GetLogicalDevice(), image, memory, 0);
	}

	void VulkanContext::read_shader(const std::string& path, std::vector<char>& source)
	{
		std::ifstream file(path, std::ios::ate | std::ios::binary);		//ate模式是开始时处于文件尾部
		if (!file.is_open())
		{
			throw std::runtime_error("failed to read file : " + path);
		}

		size_t file_size = (size_t)file.tellg();
		source.resize(file_size);

		//将文件指针指到开头
		file.seekg(0);
		file.read(source.data(), file_size);

		file.close();
	}

	VkShaderModule VulkanContext::create_shader_module(const std::vector<char>& source)
	{
		VkShaderModule shader_module;
		VkShaderModuleCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		create_info.codeSize = source.size();
		create_info.pCode = reinterpret_cast<const uint32_t*>(source.data());

		if (vkCreateShaderModule(_instance.GetLogicalDevice(), &create_info, nullptr, &shader_module) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create shader module!");
		}

		return shader_module;
	}

	void VulkanContext::OnWindowResizeEvent(const WindowResizeEvent& e)
	{
		wanna_recreate_swapchain();
	}

	//使用命令池传输数据
	void VulkanContext::copy_buffer(VkBuffer src_buffer, VkBuffer dst_buffer, VkDeviceSize size)
	{
		VkCommandBuffer command_buffer = begin_command_buffer();

		VkBufferCopy copy_region = {};
		copy_region.srcOffset = 0;
		copy_region.dstOffset = 0;
		copy_region.size = size;
		vkCmdCopyBuffer(command_buffer, src_buffer, dst_buffer, 1, &copy_region);

		end_command_buffer(command_buffer);
	}

	void VulkanContext::copy_buffer_image(VkBuffer buffer, VkImage img, uint32_t width, uint32_t height)
	{
		VkCommandBuffer command_buffer = begin_command_buffer();

		VkBufferImageCopy region = {};
		region.bufferOffset = 0;
		region.bufferRowLength = 0;
		region.bufferImageHeight = 0;

		region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		region.imageSubresource.mipLevel = 0;
		region.imageSubresource.baseArrayLayer = 0;
		region.imageSubresource.layerCount = 1;

		region.imageOffset = { 0, 0, 0 };
		region.imageExtent = { width, height, 1 };

		vkCmdCopyBufferToImage(command_buffer, buffer, img, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

		end_command_buffer(command_buffer);
	}

	void VulkanContext::update_uniform_data()
	{
		static auto start_time = std::chrono::high_resolution_clock::now();
		auto current_time = std::chrono::high_resolution_clock::now();
		float time = std::chrono::duration<float, std::chrono::seconds::period>(current_time - start_time).count();

		UniformBufferObject ubo = {};
		ubo.model = glm::mat4(1.0f) * glm::scale(glm::mat4(1.0f), glm::vec3(0.1f, 0.1f, 0.1f));
		ubo.view = _camera.GetViewMatrix();
		ubo.projection = glm::perspective(
			glm::radians(45.0f)
			, m_swapchain_extent.width / (float) m_swapchain_extent.height
		, 0.1f, 1000.0f);

		//ubo.time.x = time;

		ubo.projection[1][1] *= -1;

		std::vector<glm::mat4> bone_vector;
		_skinned_mesh.UpdateAnimation(time, bone_vector);

		for (int i = 0; i < bone_vector.size(); i++)
			ubo.bones[i] = bone_vector[i];

		void* data;
		vkMapMemory(_instance.GetLogicalDevice(), m_uniform_memory, 0, sizeof(ubo), 0, &data);
		memcpy(data, &ubo, sizeof(ubo));
		vkUnmapMemory(_instance.GetLogicalDevice(), m_uniform_memory);
	}

	VkCommandBuffer VulkanContext::begin_command_buffer()
	{
		VkCommandBufferAllocateInfo alloc_info = {};
		alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		alloc_info.commandPool = m_command_pool;
		alloc_info.commandBufferCount = 1;

		VkCommandBuffer command_buffer;
		vkAllocateCommandBuffers(_instance.GetLogicalDevice(), &alloc_info, &command_buffer);

		//开始记录指令
		VkCommandBufferBeginInfo begin_info = {};
		begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		vkBeginCommandBuffer(command_buffer, &begin_info);
		
		return command_buffer;
	}

	void VulkanContext::end_command_buffer(VkCommandBuffer command_buffer)
	{
		vkEndCommandBuffer(command_buffer);

		//提交指令
		VkSubmitInfo submit_info = {};
		submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submit_info.commandBufferCount = 1;
		submit_info.pCommandBuffers = &command_buffer;

		vkQueueSubmit(_instance.GetGraphicQueue(), 1, &submit_info, VK_NULL_HANDLE);
		vkQueueWaitIdle(_instance.GetGraphicQueue());

		vkFreeCommandBuffers(_instance.GetLogicalDevice(), m_command_pool, 1, &command_buffer);
	}

	static bool hasStencilComponent(VkFormat format) {
		return format == VK_FORMAT_D32_SFLOAT_S8_UINT ||
			format == VK_FORMAT_D24_UNORM_S8_UINT;
	}

	void VulkanContext::tansition_img_layout(VkImage img, VkFormat format, VkImageLayout old_layout, VkImageLayout new_layout)
	{
		VkCommandBuffer command_buffer = begin_command_buffer();

		VkImageMemoryBarrier barrier = {};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.oldLayout = old_layout;
		barrier.newLayout = new_layout;

		//传输队列所有权
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

		barrier.image = img;
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = 1;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;

		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = 0;

		VkPipelineStageFlags sourceStage;
		VkPipelineStageFlags destinationStage;

		if (new_layout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
			barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

			if (hasStencilComponent(format)) {
				barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
			}
		}
		else {
			barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		}

		if (old_layout == VK_IMAGE_LAYOUT_UNDEFINED && new_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		}
		else if (old_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
			new_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}
		else if (old_layout == VK_IMAGE_LAYOUT_UNDEFINED &&
			new_layout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask =
				VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
				VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		}
		else {
			throw std::invalid_argument("unsupported layout transition!");
		}

		vkCmdPipelineBarrier(command_buffer, sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);

		end_command_buffer(command_buffer);
	}
}