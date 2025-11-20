#include "PCH.h"
#include "RendererAPI.h"
#include "RendererContext.h"
#include "Platform/Renderer/Vulkan/VulkanRenderer.h"
namespace srx
{
	Ref<RendererAPI> RendererAPI::Create()
	{
		switch (RendererContext::CurrentEngine())
		{
		case RendererEngineType::Vulkan: 
			return CreateRef<VulkanRenderer>();
			break;
		case RendererEngineType::None:
		default:
			SRX_ASSERT(false, "Unknown Renderer API!");
		}
	}
}