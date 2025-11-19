#pragma once
#include "Renderer/RenderCommandBuffer.h"
#include <vulkan/vulkan.h>
namespace srx
{
	class VulkanRenderCommandBuffer : public RenderCommandBuffer
	{
	public:
		void Begin() override;
		void End() override;
		void Submit() override;
	private:
		VkCommandPool _CommandPool;
		VkCommandBuffer _CommandBuffer;
	};
}