#pragma once
#include "RendererAPI.h"
namespace srx
{
	class Renderer final
	{
	public:
		static void Init();
	private:
		inline static Ref<RendererAPI> s_RenderAPI = nullptr;
	};
}