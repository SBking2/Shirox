#pragma once
#include "RendererAPI.h"
#include "RenderCommandBuffer.h"
#include "RenderPass.h"
namespace srx
{
	class Renderer final
	{
	public:
		static void Init();
		static void BeginRenderPass(const Ref<RenderCommandBuffer>& render_command_buffer, const Ref<RenderPass>& render_pass);
		static void RenderTriangle(const Ref<RenderCommandBuffer>& render_command_buffer, const Ref<RenderPass>& render_pass);
		static void EndRenderPass(const Ref<RenderCommandBuffer>& render_command_buffer, const Ref<RenderPass>& render_pass);
	private:
		inline static Ref<RendererAPI> s_RenderAPI = nullptr;
	};
}