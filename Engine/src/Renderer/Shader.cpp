#include "PCH.h"
#include "Shader.h"
#include "RendererContext.h"
#include "Platform/Renderer/Vulkan/VulkanShader.h"
namespace srx
{
	Ref<Shader> Shader::Create()
	{
		switch (RendererContext::CurrentEngine())
		{
		case RendererEngineType::Vulkan: return CreateRef<VulkanShader>();
		case RendererEngineType::None:
		default:
			SRX_ASSERT(false, "Î´ÖªµÄRenderAPI!");
			return nullptr;
		}
	}
}