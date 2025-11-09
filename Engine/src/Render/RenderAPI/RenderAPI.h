#pragma once
#include "Window/Window.h"
namespace srx
{
	enum class RenderEngine
	{
		None = 0,
		Vulkan = 1
	};

	class RenderAPI
	{
	public:
		static Ref<RenderAPI> Create(RenderEngine engine_type);
		virtual void Init(const Window* window) = 0;
		virtual void Draw() = 0;
		virtual void Destroy() = 0;
	};
}