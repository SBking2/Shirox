#include "PCH.h"
#include "Pipeline.h"
#include "RendererContext.h"
#include "Platform/Renderer/Vulkan/VulkanPipeline.h"
namespace srx
{
	Pipeline::Pipeline(const PipelineSpecification& spec) :_Specification(spec)
	{

	}

	Ref<Pipeline> Pipeline::Create(const PipelineSpecification& spec)
	{
		switch (RendererContext::CurrentEngine())
		{
		case RendererEngineType::Vulkan: return CreateRef<VulkanPipeline>();
		case RendererEngineType::None:
		default:
			SRX_ASSERT(false, "Î´ÖªµÄRenderAPI!");
			return nullptr;
		}
	}
}