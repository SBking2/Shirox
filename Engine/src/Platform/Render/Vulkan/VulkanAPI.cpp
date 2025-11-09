#include "PCH.h"
#include "VulkanAPI.h"
namespace srx
{
	void VulkanAPI::Init(const Window* window)
	{
		_context = CreateRef<VulkanContext>();
		_swapchain = CreateRef<VulkanSwapchain>();
		_context->Init(window);
		_swapchain->Init(_context);
	}

	void VulkanAPI::Draw()
	{

	}

	void VulkanAPI::Destroy()
	{
		_swapchain->Destroy();
		_context->Clear();
	}
}