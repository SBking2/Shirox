#include "PCH.h"
#include "RenderCommandBuffer.h"
#include "RendererContext.h"
#include "Platform/Renderer/Vulkan/VulkanRenderCommandBuffer.h"
namespace srx
{
	Ref<RenderCommandBuffer> RenderCommandBuffer::Create()
	{
		switch (RendererContext::CurrentEngine())
		{
		case RendererEngineType::Vulkan: return CreateRef<VulkanRenderCommandBuffer>();
		case RendererEngineType::None:
		default: 
			SRX_ASSERT(false, "Î´ÖªµÄRenderAPI!");
			return nullptr;
		}
	}
}