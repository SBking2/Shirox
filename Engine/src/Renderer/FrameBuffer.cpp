#include "PCH.h"
#include "FrameBuffer.h"
#include "RendererContext.h"
#include "Platform/Renderer/Vulkan/VulkanFrameBuffer.h"
namespace srx
{
	Ref<FrameBuffer> FrameBuffer::Create()
	{
		switch (RendererContext::CurrentEngine())
		{
		case RendererEngineType::Vulkan: return CreateRef<VulkanFrameBuffer>();
		case RendererEngineType::None:
		default:
			SRX_ASSERT(false, "Î´ÖªµÄRenderAPI!");
			return nullptr;
		}
	}
}