#pragma once
#include "Singleton/Singleton.h"
#include "Event/Event.h"
#include "Event/EventDispatcher.h"
#include "Renderer/Camera/CameraController.h"
#include "Renderer/Camera/Camera.h"
#include "Renderer/Mesh/SkinnedMesh.h"
#include "Core/Instance.h"
#include "Core/Device.h"
#include "Core/Surface.h"
#include "Core/Swapchain.h"
#include "Core/RenderPass.h"
#include "Core/RenderPipeline.h"
#define GLFW_INCLUED_VULKAN
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <GLM/glm.hpp>
#include <vector>
#include <string>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace ev
{
	class VulkanContext : public Singleton<VulkanContext>
	{
	public:
		VulkanContext();
		void init(GLFWwindow* window);
		void draw_frame();
		void clear();
		void OnEvent(const Event& e);
		void OnUpdate(float delta);
	private:
		void create_descriptor_layout();
		void create_framebuffer();
		void create_command_pool();
		void create_depth_resource();
		void create_texture();
		void create_texture_view();
		void create_texture_sampler();
		void load_model();
		void assimp_load_model();
		void create_plane();
		void create_vertex_buffer();
		void create_descriptor_pool();
		void create_descriptor_sets();
		void create_command_buffer();
		void create_semaphore();
	private:
		void recreate_swapchain();
	private:
		void create_buffer(VkDeviceSize size, VkBufferUsageFlagBits usage, VkMemoryPropertyFlags properties
			, VkBuffer& buffer, VkDeviceMemory& memory);
		void create_image(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling
			, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& memory);
		uint32_t find_memory_type(uint32_t filter, VkMemoryPropertyFlags properties);
		void copy_buffer(VkBuffer src_buffer, VkBuffer dst_buffer, VkDeviceSize size);
		void copy_buffer_image(VkBuffer buffer, VkImage img, uint32_t width, uint32_t height);
		void update_uniform_data();
		VkCommandBuffer begin_command_buffer();
		void end_command_buffer(VkCommandBuffer command_buffer);
		void tansition_img_layout(VkImage img, VkFormat format, VkImageLayout old_layout, VkImageLayout new_layout);
		inline void wanna_recreate_swapchain() { m_is_wanna_recreate_swapchain = true; }
		void OnWindowResizeEvent(const WindowResizeEvent& e);
	public:
		GLFWwindow* m_window;
		bool m_is_wanna_recreate_swapchain;

		struct UniformBufferObject
		{
			glm::mat4 model;
			glm::mat4 view;
			glm::mat4 projection;
			glm::mat4 bones[100];
			//glm::vec4 time;
		};

		Assimp::Importer _importer;
		SkinnedMesh _skinned_mesh;

		Camera _camera;
		CameraController _camera_controller;

		Instance _instance;
		Surface _surface;
		Device _device;
		Swapchain _swapchain;
		RenderPass _render_pass;
		RenderPipeline _pipeline;

		std::vector<VkFramebuffer> m_swapchain_framebuffers;

		VkCommandPool m_command_pool;

		VkBuffer m_vertex_buffer;
		VkDeviceMemory m_vertex_buffer_memory;
		VkBuffer m_index_buffer;
		VkDeviceMemory m_index_buffer_memory;
		VkBuffer m_uniform_buffer;
		VkDeviceMemory m_uniform_memory;

		VkImage m_depth_img;
		VkImageView m_depth_img_view;
		VkDeviceMemory m_depth_img_memory;

		VkImage m_texture_img;
		VkDeviceMemory m_texture_img_memory;
		VkImageView m_texture_img_view;
		VkSampler m_texture_sampler;

		VkDescriptorSetLayout m_descriptor_layout;
		VkDescriptorSet m_descriptor_set;
		VkDescriptorPool m_descriptor_pool;

		std::vector<VkCommandBuffer> m_command_buffers;

		VkSemaphore m_img_avaliable_semaphore;
		VkSemaphore m_render_finish_semaphore;
		VkFence m_inflight_fence;
	};
}