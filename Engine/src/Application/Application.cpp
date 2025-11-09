#include "PCH.h"
#include "Application.h"
#include "Render/Vulkan/VulkanContext.h"
#include "Window/Window.h"

namespace srx
{

	void Application::Init()
	{
		Log::Init("assets/log/log.log");
		Log::SetLevel(LogLevel::Trace);
		Window::GetInstance()->Init(800, 600, "vulkan_example");
		Window::GetInstance()->SetEventCallback(std::bind(&Application::OnEvent, this, std::placeholders::_1));

		VulkanContext::GetInstance()->init(Window::GetInstance()->GetWindow());
	}

	void Application::Run()
	{
		while (!Window::GetInstance()->IsShouldClose())
		{
			auto now = std::chrono::high_resolution_clock::now();
			std::chrono::duration<float> delta = now - _last_time;
			Window::GetInstance()->HandleEvent();
			VulkanContext::GetInstance()->OnUpdate(delta.count());
			VulkanContext::GetInstance()->draw_frame();
			_last_time = now;
			Log::LogError("Hellow World!");
		}
	}

	void Application::Clear()
	{
		VulkanContext::GetInstance()->clear();
		Window::GetInstance()->Clear();
	}

	void Application::OnEvent(const Event& e)
	{
		VulkanContext::GetInstance()->OnEvent(e);
	}
}