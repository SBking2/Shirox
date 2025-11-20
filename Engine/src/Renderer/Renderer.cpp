#include "PCH.h"
#include "Renderer.h"
#include "RendererContext.h"
namespace srx
{
	void Renderer::Init()
	{
		s_RenderAPI = RendererAPI::Create();	//≥ı ºªØRendererAPI
	}

	void Renderer::BeginRenderPass(const Ref<RenderCommandBuffer>& render_command_buffer, const Ref<RenderPass>& render_pass)
	{
		s_RenderAPI->BeginRenderPass(render_command_buffer, render_pass);
	}

	void Renderer::RenderTriangle(const Ref<RenderCommandBuffer>& render_command_buffer, const Ref<RenderPass>& render_pass)
	{
		s_RenderAPI->RenderTriangle(render_command_buffer, render_pass);
	}

	void Renderer::EndRenderPass(const Ref<RenderCommandBuffer>& render_command_buffer, const Ref<RenderPass>& render_pass)
	{
		s_RenderAPI->EndRenderPass(render_command_buffer, render_pass);
	}
}