#include "PCH.h"
#include "Application.h"
#include "Renderer/Renderer.h"
#include "Window/Window.h"
#include "Renderer/RendererContext.h"
namespace srx
{

	void Application::Init()
	{
		RendererContext::CreateContext();	//需要先创建RendereContext再创建Window

		SRX_LOG_INIT("assets/log/log.log")
		Log::SetLevel(LogLevel::Trace);
		Window::GetInstance()->Init(800, 600, "vulkan_example");
		Window::GetInstance()->SetEventCallback(std::bind(&Application::OnEvent, this, std::placeholders::_1));

		Renderer::Init();
	}

	void Application::Run()
	{
		while (!Window::GetInstance()->IsShouldClose())
		{
			auto now = std::chrono::high_resolution_clock::now();
			std::chrono::duration<float> delta = now - _last_time;
			Window::GetInstance()->HandleEvent();
			
			_last_time = now;
		}
	}

	void Application::Clear()
	{
		RendererContext::GetContext()->Destroy();
		Window::GetInstance()->Clear();
	}

	void Application::OnEvent(const Event& e)
	{

	}
}