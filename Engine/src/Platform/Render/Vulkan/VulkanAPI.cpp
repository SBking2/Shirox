#include "PCH.h"
#include "VulkanAPI.h"
namespace srx
{
	void VulkanAPI::Init(const Window* window)
	{
		_context = new VulkanContext(window);
		_swapchain = new VulkanSwapchain();
		_swapchain->Init(_context);
	}

	void VulkanAPI::Draw()
	{

	}

	void VulkanAPI::Destroy()
	{
		_swapchain->Destroy();
		delete _swapchain;
		delete _context;
	}
}