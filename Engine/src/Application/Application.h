#pragma once
#include "Layer.h"
namespace srx
{
	class Application
	{
	public:
		void Init();
		void Run();
		void Clear();
	private:
		void OnEvent(const Event& e);
		std::vector<Ref<Layer>> _Layers;
		std::chrono::high_resolution_clock::time_point _last_time;
	};
}