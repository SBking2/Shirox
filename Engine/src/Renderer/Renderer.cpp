#include "PCH.h"
#include "Renderer.h"
#include "RendererContext.h"
namespace srx
{
	void Renderer::Init()
	{
		s_RenderAPI = RendererAPI::Create();	//≥ı ºªØRendererAPI
	}
}