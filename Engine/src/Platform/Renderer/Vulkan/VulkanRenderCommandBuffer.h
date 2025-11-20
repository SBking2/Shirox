#pragma once
#include "Renderer/RenderCommandBuffer.h"
#include <vulkan/vulkan.h>
namespace srx
{
	class VulkanRenderCommandBuffer : public RenderCommandBuffer
	{
	public:
		VulkanRenderCommandBuffer();
		virtual ~VulkanRenderCommandBuffer();
		inline VkCommandBuffer GetVkCommandBuffer() { return _CommandBuffer; }
		void Begin() override;
		void End() override;
		void Submit() override;
	private:
		VkCommandPool _CommandPool;
		VkCommandBuffer _CommandBuffer;
	};
}