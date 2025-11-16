#pragma once

namespace srx
{
	//存储了渲染方法和当前使用的渲染API
	class RendererAPI
	{
	public:
		RendererAPI() = default;
		virtual ~RendererAPI() = default;

		static Ref<RendererAPI> Create();
	};
}