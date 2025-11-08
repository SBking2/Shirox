#pragma once
#include "Core/Event/Event.h"
#define GLFW_INCLUED_VULKAN

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
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
		std::chrono::high_resolution_clock::time_point _last_time;
	};
}