#include "PCH.h"
#include "RenderPass.h"
#include "RendererContext.h"
#include "Platform/Renderer/Vulkan/VulkanRenderPass.h"
namespace srx
{
	RenderPass::RenderPass(const RenderPassSpecification& spec):_Specification(spec)
	{

	}

	Ref<RenderPass> RenderPass::Create(const RenderPassSpecification& spec)
	{
		switch (RendererContext::CurrentEngine())
		{
		case RendererEngineType::Vulkan: return CreateRef<VulkanRenderPass>();
		case RendererEngineType::None:
		default:
			SRX_ASSERT(false, "Î´ÖªµÄRenderAPI!");
			return nullptr;
		}
	}
}