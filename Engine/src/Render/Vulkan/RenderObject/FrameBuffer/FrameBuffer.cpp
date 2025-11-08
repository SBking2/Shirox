#include "FrameBuffer.h"
#include <stdexcept>
namespace ev
{
	std::shared_ptr<FrameBuffer> FrameBuffer::Create(const Device& device, const std::vector<ImageView>& attachments, const RenderPass& render_pass, uint32_t width, uint32_t height)
	{
		std::shared_ptr<FrameBuffer> buffer = std::make_shared<FrameBuffer>();
		std::vector<VkImageView> views;
		for (const auto& attachment : attachments)
			views.push_back(attachment.GetImageView());
		buffer->Init(device, views, render_pass, width, height);
		return buffer;
	}

	std::shared_ptr<FrameBuffer> FrameBuffer::Create(const Device& device, const std::vector<VkImageView>& attachments, const RenderPass& render_pass, uint32_t width, uint32_t height)
	{
		std::shared_ptr<FrameBuffer> buffer = std::make_shared<FrameBuffer>();
		buffer->Init(device, attachments, render_pass, width, height);
		return buffer;
	}

	void FrameBuffer::Destroy(const Device& device)
	{
		vkDestroyFramebuffer(device.GetLogicalDevice(), _buffer, nullptr);
	}

	void FrameBuffer::Init(const Device& device, const std::vector<VkImageView>& attachments
		, const RenderPass& render_pass, uint32_t width, uint32_t height)
	{
		VkFramebufferCreateInfo framebuffer_create_info = {};
		framebuffer_create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebuffer_create_info.renderPass = render_pass.GetRenderPass();
		framebuffer_create_info.attachmentCount = static_cast<uint32_t>(attachments.size());
		framebuffer_create_info.pAttachments = attachments.data();
		framebuffer_create_info.width = width;
		framebuffer_create_info.height = height;
		framebuffer_create_info.layers = 1;

		if (vkCreateFramebuffer(device.GetLogicalDevice(), &framebuffer_create_info, nullptr, &_buffer)
			!= VK_SUCCESS)
			throw std::runtime_error("failed to create framebuffer!");
	}
}