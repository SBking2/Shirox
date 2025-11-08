#pragma once
#include "Core/Singleton/Singleton.h"
#include "Core/Event/Event.h"
#include "Core/Event/EventDispatcher.h"
#include "Render/Camera/CameraController.h"
#include "Render/Camera/Camera.h"
#include "Render/Mesh/SkinnedMesh.h"
#include "Core/Instance.h"
#include "Core/Device.h"
#include "Core/Surface.h"
#include "Core/Swapchain.h"
#include "Core/RenderPass.h"
#include "Core/RenderPipeline.h"
#include "RenderObject/DescriptorSetLayout/DescriptorSetLayout.h"
#include "Core/CommandPool.h"
#include "Core/DescriptorPool.h"
#include "RenderObject/CommandBuffer/CommandBuffer.h"
#include "RenderObject/Buffer/Buffer.h"
#include "RenderObject/Image/Image.h"
#include "RenderObject/Image/ImageView.h"
#include "RenderObject/FrameBuffer/FrameBuffer.h"
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
		void create_framebuffer();
		void create_depth_resource();
		void create_texture_sampler();
		void assimp_load_model();
		void create_vertex_buffer();
		void create_descriptor_sets();
		void create_command_buffer();
		void create_semaphore();
	private:
		void recreate_swapchain();
	private:
		void update_uniform_data();
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
		CommandPool _command_pool;
		DescriptorPool _descriptor_pool;

		std::vector<std::shared_ptr<FrameBuffer>> _frame_buffers;

		std::shared_ptr<Buffer> _vertex_buffer;
		std::shared_ptr<Buffer> _index_buffer;
		std::shared_ptr<Buffer> _uniform_buffer;

		std::shared_ptr<Image> _depth_img;
		std::shared_ptr<ImageView> _depth_img_view;

		std::shared_ptr<Image> _texture_img;
		std::shared_ptr<ImageView> _texture_img_view;
		VkSampler m_texture_sampler;

		std::shared_ptr<DescriptorSetLayout> _descriptor_layout;
		VkDescriptorSet m_descriptor_set;

		std::vector<VkCommandBuffer> m_command_buffers;

		VkSemaphore m_img_avaliable_semaphore;
		VkSemaphore m_render_finish_semaphore;
		VkFence m_inflight_fence;
	};
}