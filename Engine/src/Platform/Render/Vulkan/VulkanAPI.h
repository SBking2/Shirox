#pragma once
#include "Render/RenderAPI/RenderAPI.h"
#include "VulkanContext.h"
#include "VulkanSwapchain.h"
#include <vulkan/vulkan.h>
namespace srx
{
	class VulkanAPI final : public RenderAPI
	{
	public:
		void Init(const Window* window) override;
		void Draw() override;
		void Destroy() override;
	private:
		VulkanContext* _context;
		VulkanSwapchain* _swapchain;
	};
}