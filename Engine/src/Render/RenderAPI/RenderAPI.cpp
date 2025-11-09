#include "PCH.h"
#include "RenderAPI.h"
#include "Platform/Render/Vulkan/VulkanAPI.h"
namespace srx
{
	RenderAPI* RenderAPI::Create(RenderEngine engine_type)
	{
		switch (engine_type)
		{
		case srx::RenderEngine::Vulkan:
			return new VulkanAPI();
			break;
		case srx::RenderEngine::None:
		default:
			std::runtime_error("Unknow Render Engine!");
			return nullptr;
			break;
		}
	}
}