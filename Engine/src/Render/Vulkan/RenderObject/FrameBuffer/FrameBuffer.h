#pragma once
#include "Render/Vulkan/Core/Device.h"
#include "Render/Vulkan/Core/RenderPass.h"
#include "Render/Vulkan/RenderObject/Image/ImageView.h"
#define GLFW_INCLUED_VULKAN
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <memory>
#include <vector>
namespace ev
{
	class FrameBuffer final
	{
	public:
		static std::shared_ptr<FrameBuffer> Create(const Device& device, const std::vector<ImageView>& attachments
			, const RenderPass& render_pass, uint32_t width, uint32_t height);
		static std::shared_ptr<FrameBuffer> Create(const Device& device, const std::vector<VkImageView>& attachments
			, const RenderPass& render_pass, uint32_t width, uint32_t height);
		inline const VkFramebuffer& GetFrameBuffer() const { return _buffer; }
		void Destroy(const Device& device);
	private:
		void Init(const Device& device, const std::vector<VkImageView>& attachments
			, const RenderPass& render_pass, uint32_t width, uint32_t height);
		VkFramebuffer _buffer;
	};
}