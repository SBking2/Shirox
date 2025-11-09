#pragma once
#include "Render/RenderAPI/RenderAPI.h"
#include "VulkanContext.h"
#include "VulkanSwapchain.h"
namespace srx
{
	class VulkanAPI final : public RenderAPI
	{
	public:
		void Init(const Window* window) override;
		void Draw() override;
		void Destroy() override;
	private:
		Ref<VulkanContext> _context;
		Ref<VulkanSwapchain> _swapchain;
	};
}