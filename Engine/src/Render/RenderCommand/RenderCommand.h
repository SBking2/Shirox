#pragma once
#include "Render/RenderAPI/RenderAPI.h"
namespace srx
{
	class RenderCommand final
	{
	public:
		static void Init();
		static void Draw();
		static void Destory();
	private:
		static RenderAPI* _render_api;
	};
}
