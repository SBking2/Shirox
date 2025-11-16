#pragma once

namespace srx
{
	enum class RendererEngineType
	{
		None = 0,
		Vulkan = 1
	};

	class RendererContext
	{
	public:
		RendererContext() = default;
		virtual ~RendererContext() = default;

		virtual void Init() = 0;
		virtual void Destroy() = 0;
		static void CreateContext();
		static RendererEngineType CurrentEngine() { return s_EngineType; }
		inline static Ref<RendererContext> GetContext() { s_Context; };
	private:
		inline static Ref<RendererContext> s_Context = nullptr;
		inline static RendererEngineType s_EngineType = RendererEngineType::Vulkan;
	};
}