#include "PCH.h"
#include "VulkanContext.h"
#include "RenderObject/Shader/ShaderModule.h"
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"


#include <glm/gtx/quaternion.hpp>
#include "VulkanContext.h"

namespace srx
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

		_instance.Init();
		_surface.Init(m_window, _instance);
		_device.Init(_instance, _surface);
		
		//首先第一件事，就是从物理显卡支持的swapchain设置中，挑出合适的
		VkExtent2D choose_extent;
		VkSurfaceFormatKHR choose_format;
		VkPresentModeKHR choose_present_mode;

		//选择format
		{
			if (_device.device_info.formats.size() == 1 && _device.device_info.formats[0].format == VK_FORMAT_UNDEFINED)
			{
				choose_format = { VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
			}

			bool is_have_format = false;
			for (const auto& format : _device.device_info.formats)
			{
				if (format.format == VK_FORMAT_B8G8R8A8_UNORM && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
				{
					choose_format = format;
					is_have_format = true;
					break;
				}
			}

			if (!is_have_format) choose_format = _device.device_info.formats[0];
		}

		//选择mode
		{
			bool is_have_mode = false;
			for (const auto& mode : _device.device_info.present_modes)
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
			vkGetPhysicalDeviceSurfaceCapabilitiesKHR(_device.GetPhysicalDevice(), _surface.GetSurface(), &_device.device_info.capabilities);

			if (_device.device_info.capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
			{
				choose_extent = _device.device_info.capabilities.currentExtent;
			}
			else
			{
				int width, height;
				glfwGetFramebufferSize(m_window, &width, &height);

				choose_extent = { static_cast<uint32_t>(width), static_cast<uint32_t>(height) };
				choose_extent.width = std::max(_device.device_info.capabilities.minImageExtent.width
					, std::min(_device.device_info.capabilities.maxImageExtent.width, choose_extent.width));

				choose_extent.height = std::max(_device.device_info.capabilities.minImageExtent.height
					, std::min(_device.device_info.capabilities.maxImageExtent.height, choose_extent.height));
			}
		}

		uint32_t img_count = _device.device_info.capabilities.minImageCount + 1;
		if (_device.device_info.capabilities.maxImageCount > 0 &&
			img_count > _device.device_info.capabilities.maxImageCount)
			img_count = _device.device_info.capabilities.maxImageCount;

		std::unordered_set<uint32_t> queue_indices_set;
		queue_indices_set.insert(_device.device_info.graphic_queue_index);
		queue_indices_set.insert(_device.device_info.present_queue_index);

		std::vector<uint32_t> queue_indedices_vec;
		for (const auto& it : queue_indices_set)
			queue_indedices_vec.push_back(it);

		_swapchain.Init(_device, _surface, choose_format, choose_extent
			, choose_present_mode, queue_indedices_vec, img_count);

		_render_pass.Init(_device, _swapchain);

		_command_pool.Init(_device);
		create_depth_resource();
		create_framebuffer();
		//create_texture();
		assimp_load_model();
		_descriptor_layout = DescriptorSetLayout::Create(_device);
		_pipeline.Init(_device, _render_pass, _swapchain, *_descriptor_layout->GetLayoutP());
		create_texture_sampler();
		create_vertex_buffer();
		_descriptor_pool.Init(_device);
		create_descriptor_sets();
		create_command_buffer();
		create_semaphore();
	}

	void VulkanContext::clear()
	{
		vkDestroySemaphore(_device.GetLogicalDevice(), m_img_avaliable_semaphore, nullptr);
		vkDestroySemaphore(_device.GetLogicalDevice(), m_render_finish_semaphore, nullptr);
		vkDestroyFence(_device.GetLogicalDevice(), m_inflight_fence, nullptr);

		_vertex_buffer->Destroy(_device);
		_index_buffer->Destroy(_device);
		_uniform_buffer->Destroy(_device);

		vkDestroySampler(_device.GetLogicalDevice(), m_texture_sampler, nullptr);
		_texture_img_view->Destroy(_device);
		_texture_img->Destroy(_device);

		_depth_img->Destroy(_device);
		_depth_img_view->Destroy(_device);

		_descriptor_pool.Destroy(_device);
		_descriptor_layout->Destroy(_device);

		for (auto& framebuffer : _frame_buffers)
		{
			framebuffer->Destroy(_device);
		}

		_command_pool.Destroy(_device);

		//清理Vulkan
		_render_pass.Destroy(_device);
		_pipeline.Destroy(_device);

		_swapchain.Destroy(_device);
		_device.Destroy();
		_surface.Destroy(_instance);
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
		//vkWaitForFences(_device.GetLogicalDevice(), 1, &m_inflight_fence, VK_TRUE
		//	, std::numeric_limits<uint64_t>::max());
		//vkResetFences(_device.GetLogicalDevice(), 1, &m_inflight_fence);	//将fence变为unsignaled

		//从交换链获取一张图像
		uint32_t img_index;
		auto result = vkAcquireNextImageKHR(_device.GetLogicalDevice(), _swapchain.GetSwapchain(), std::numeric_limits<uint64_t>::max(),
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
		if (vkQueueSubmit(_device.GetGraphicQueue(), 1, &submit_info, VK_NULL_HANDLE) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to queue submit!");
		}

		//返回渲染后的图像到交换链进行呈现
		VkPresentInfoKHR present_info = {};
		present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		present_info.waitSemaphoreCount = 1;
		present_info.pWaitSemaphores = signalSemaphores;

		VkSwapchainKHR swap_chains[] = { _swapchain.GetSwapchain()};
		present_info.swapchainCount = 1;
		present_info.pSwapchains = swap_chains;
		present_info.pImageIndices = &img_index;

		vkQueuePresentKHR(_device.GetPresentQueue(), &present_info);
		vkQueueWaitIdle(_device.GetPresentQueue());
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

		vkDeviceWaitIdle(_device.GetLogicalDevice());		//等待执行结束

		vkDestroySemaphore(_device.GetLogicalDevice(), m_img_avaliable_semaphore, nullptr);
		vkDestroySemaphore(_device.GetLogicalDevice(), m_render_finish_semaphore, nullptr);
		vkDestroyFence(_device.GetLogicalDevice(), m_inflight_fence, nullptr);

		_depth_img->Destroy(_device);

		for (auto frame : _frame_buffers)
		{
			frame->Destroy(_device);
		}
		vkFreeCommandBuffers(_device.GetLogicalDevice(), _command_pool.GetCommandPool(), static_cast<uint32_t>(m_command_buffers.size())
			, m_command_buffers.data());

		_pipeline.Destroy(_device);
		_render_pass.Destroy(_device);

		_swapchain.Destroy(_device);

		_swapchain.ReCreate(_device, _surface);
		_render_pass.Init(_device, _swapchain);
		_pipeline.Init(_device, _render_pass, _swapchain, *_descriptor_layout->GetLayoutP());
		create_depth_resource();
		create_framebuffer();
		create_command_buffer();
		create_semaphore();
	}

	#pragma region step

	//为交换链的图片创建帧缓冲
	void VulkanContext::create_framebuffer()
	{
		_frame_buffers.resize(_swapchain.GetImgCount());
		for (int i = 0; i < _swapchain.GetImgCount(); i++)
		{
			std::vector<VkImageView> attachments = {
				_swapchain.GetImgView()[i],
				_depth_img_view->GetImageView()
			};

			_frame_buffers[i] = FrameBuffer::Create(_device, attachments, _render_pass, _swapchain.swapchain_info.extent.width
			, _swapchain.swapchain_info.extent.height);
		}
	}

	void VulkanContext::create_depth_resource()
	{
		VkFormat depth_format = Utils::FindSupportedFormat(_device
			, { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT }
			, VK_IMAGE_TILING_OPTIMAL
			, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);

		_depth_img = Image::Create(_device
			, _swapchain.swapchain_info.extent.width
			, _swapchain.swapchain_info.extent.height
			, depth_format
			, VK_IMAGE_TILING_OPTIMAL
			, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT
			, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		
		_depth_img_view = ImageView::Create(_device, *_depth_img, depth_format, VK_IMAGE_ASPECT_DEPTH_BIT);

		tansition_img_layout(_depth_img->GetImage(), depth_format,
			VK_IMAGE_LAYOUT_UNDEFINED,
			VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
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

		if (vkCreateSampler(_device.GetLogicalDevice(), &create_info, nullptr, &m_texture_sampler) != VK_SUCCESS)
			throw std::runtime_error("falied to create sampler!");
	}

	void VulkanContext::create_vertex_buffer()
	{
		uint32_t vertex_size = sizeof(SkinnedMesh::Vertex) * _skinned_mesh.GetVerticesSize();

		auto staging_buffer = Buffer::Create(_device
			, VK_BUFFER_USAGE_TRANSFER_SRC_BIT
			, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, vertex_size);

		//往暂存缓冲中填充数据
		staging_buffer->InputData(_device, vertex_size, _skinned_mesh.GetVerticesData());

		_vertex_buffer = Buffer::Create(_device
			, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT
			, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			, vertex_size);

		//将暂存buffer的数据传入到vertex buffer中
		_vertex_buffer->InputData(_device, vertex_size, *staging_buffer, _command_pool);
		staging_buffer->Destroy(_device);

		//*********************************************** Index buffer *****************************************************

		uint32_t index_size = sizeof(uint32_t) * _skinned_mesh.GetIndicesSize();
		auto index_staging_buffer = Buffer::Create(_device
		, VK_BUFFER_USAGE_TRANSFER_SRC_BIT
		, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		, index_size);

		index_staging_buffer->InputData(_device, index_size, _skinned_mesh.GetIndicesData());

		//往暂存缓冲中填充数据
		_index_buffer = Buffer::Create(_device
			, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT
			, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			, index_size);

		//将暂存buffer的数据传入到index buffer中
		_index_buffer->InputData(_device, index_size, *index_staging_buffer, _command_pool);
		index_staging_buffer->Destroy(_device);

		//********************************************* Uniform buffer ***************************************************

		int uniform_size = sizeof(UniformBufferObject);
		_uniform_buffer = Buffer::Create(_device
			, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT
			, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
			, uniform_size);
	}

	void VulkanContext::assimp_load_model()
	{
		const aiScene* scene = _importer.ReadFile(
			"../Engine/assets/models/racer/Flair.fbx"
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
		auto staging_buffer = Buffer::Create(_device
			, VK_BUFFER_USAGE_TRANSFER_SRC_BIT
			, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
			, img_size);

		staging_buffer->InputData(_device, img_size, pixels);

		stbi_image_free(pixels);

		_texture_img = Image::Create(_device
			, img_width
			, img_height
			, VK_FORMAT_R8G8B8A8_UNORM
			, VK_IMAGE_TILING_OPTIMAL
			, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT
			, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		//把数据从暂存缓冲传入到image中
		tansition_img_layout(_texture_img->GetImage(), VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		_texture_img->InputData(_device, img_width, img_height, *staging_buffer, _command_pool);
		tansition_img_layout(_texture_img->GetImage(), VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

		staging_buffer->Destroy(_device);

		_texture_img_view = ImageView::Create(_device, *_texture_img, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT);

		_skinned_mesh.LoadMesh(scene, scene->mMeshes[0]);
		_skinned_mesh.LoadAnimation(scene->mAnimations[0]);
	}

	void VulkanContext::create_descriptor_sets()
	{
		VkDescriptorSetAllocateInfo alloc_info = {};
		alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		alloc_info.descriptorPool = _descriptor_pool.GetPool();
		alloc_info.descriptorSetCount = 1;
		alloc_info.pSetLayouts = _descriptor_layout->GetLayoutP();

		if (vkAllocateDescriptorSets(_device.GetLogicalDevice(), &alloc_info, &m_descriptor_set) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to allocate descriptor set!");
		}

		VkDescriptorBufferInfo buffer_info = {};
		buffer_info.buffer = _uniform_buffer->GetBuffer();
		buffer_info.offset = 0;
		buffer_info.range = sizeof(UniformBufferObject);

		VkDescriptorImageInfo img_info = {};
		img_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		img_info.imageView = _texture_img_view->GetImageView();
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

		vkUpdateDescriptorSets(_device.GetLogicalDevice(), static_cast<uint32_t>(descriptor_writes.size()), descriptor_writes.data(), 0, nullptr);
	}

	void VulkanContext::create_command_buffer()
	{
		m_command_buffers.resize(_frame_buffers.size());

		VkCommandBufferAllocateInfo alloc_info = {};
		alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		alloc_info.commandPool = _command_pool.GetCommandPool();
		alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		alloc_info.commandBufferCount = (uint32_t)m_command_buffers.size();

		if (vkAllocateCommandBuffers(_device.GetLogicalDevice(), &alloc_info, m_command_buffers.data())
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
			renderpass_info.renderPass = _render_pass.GetRenderPass();
			renderpass_info.framebuffer = _frame_buffers[i]->GetFrameBuffer();
			renderpass_info.renderArea.offset = { 0, 0 };
			renderpass_info.renderArea.extent = _swapchain.swapchain_info.extent;

			std::array<VkClearValue, 2> clear_values = {};
			clear_values[0].color = {0.1f, 0.1f, 0.1f, 1.0f};
			clear_values[1].depthStencil = { 1.0f , 0 };
			renderpass_info.clearValueCount = static_cast<uint32_t>(clear_values.size());
			renderpass_info.pClearValues = clear_values.data();

			//开始renderpass
			vkCmdBeginRenderPass(m_command_buffers[i], &renderpass_info, VK_SUBPASS_CONTENTS_INLINE);

			//绑定渲染管线
			vkCmdBindPipeline(m_command_buffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, _pipeline.GetPipeline());

			VkBuffer vertex_buffers[] = { _vertex_buffer->GetBuffer()};
			VkDeviceSize offsets[] = { 0 };
			vkCmdBindVertexBuffers(m_command_buffers[i], 0, 1, vertex_buffers, offsets);
			vkCmdBindDescriptorSets(m_command_buffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, _pipeline.GetPipelineLayout()
				, 0, 1, &m_descriptor_set, 0, nullptr);
			vkCmdBindIndexBuffer(m_command_buffers[i], _index_buffer->GetBuffer(), 0, VK_INDEX_TYPE_UINT32);

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

		if (vkCreateSemaphore(_device.GetLogicalDevice(), &create_info, nullptr, &m_img_avaliable_semaphore) != VK_SUCCESS
			|| vkCreateSemaphore(_device.GetLogicalDevice(), &create_info, nullptr, &m_render_finish_semaphore) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create semaphore!");
		}

		if (vkCreateFence(_device.GetLogicalDevice(), &fence_create_info, nullptr, &m_inflight_fence) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create fence!");
		}
	}

	#pragma endregion

	void VulkanContext::OnUpdate(float delta)
	{
		_camera_controller.OnUpdate(delta);
	}

	void VulkanContext::OnWindowResizeEvent(const WindowResizeEvent& e)
	{
		wanna_recreate_swapchain();
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
			, _swapchain.swapchain_info.extent.width / (float)_swapchain.swapchain_info.extent.height
		, 0.1f, 1000.0f);

		//ubo.time.x = time;

		ubo.projection[1][1] *= -1;

		std::vector<glm::mat4> bone_vector;
		_skinned_mesh.UpdateAnimation(time, bone_vector);

		for (int i = 0; i < bone_vector.size(); i++)
			ubo.bones[i] = bone_vector[i];

		_uniform_buffer->InputData(_device, sizeof(ubo), &ubo);
	}

	static bool hasStencilComponent(VkFormat format) {
		return format == VK_FORMAT_D32_SFLOAT_S8_UINT ||
			format == VK_FORMAT_D24_UNORM_S8_UINT;
	}

	void VulkanContext::tansition_img_layout(VkImage img, VkFormat format, VkImageLayout old_layout, VkImageLayout new_layout)
	{
		auto com_buffer = CommandBuffer::Create();
		com_buffer->BeginCommand(_device, _command_pool);

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

		vkCmdPipelineBarrier(com_buffer->GetBuffer(), sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);

		com_buffer->EndCommand(_device, _command_pool);
	}
}