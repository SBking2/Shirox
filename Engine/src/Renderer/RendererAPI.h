#pragma once
#include "RenderCommandBuffer.h"
#include "RenderPass.h"
namespace srx
{
	//存储了渲染方法和当前使用的渲染API
	class RendererAPI
	{
	public:
		virtual void Init() = 0;
		virtual void BeginRenderPass(const Ref<RenderCommandBuffer>& render_command_buffer, const Ref<RenderPass>& render_pass) = 0;
		virtual void RenderTriangle(const Ref<RenderCommandBuffer>& render_command_buffer, const Ref<RenderPass>& render_pass) = 0;
		virtual void EndRenderPass(const Ref<RenderCommandBuffer>& render_command_buffer, const Ref<RenderPass>& render_pass) = 0;

		static Ref<RendererAPI> Create();
	};
}