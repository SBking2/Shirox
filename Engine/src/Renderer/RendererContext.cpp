#include "PCH.h"
#include "RendererContext.h"
#include "Renderer/RendererAPI.h"
#include "Platform/Renderer/Vulkan/VulkanContext.h"
namespace srx
{
	void RendererContext::CreateContext()
	{
		switch (CurrentEngine())
		{
		case RendererEngineType::Vulkan: s_Context = CreateRef<VulkanContext>(); break;
		default:
			SRX_ASSERT(false, "Î´ÖªµÄRenderAPI!");
		}

		s_Context->Init();
	}
}