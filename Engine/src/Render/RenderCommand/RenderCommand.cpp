#include "PCH.h"
#include "RenderCommand.h"
#include "Window/Window.h"
namespace srx
{
	RenderAPI* RenderCommand::_render_api = nullptr;	//初始化静态成员变量

	void RenderCommand::Init()
	{
		_render_api = RenderAPI::Create(RenderEngine::Vulkan);
		_render_api->Init(Window::GetInstance());
	}

	void RenderCommand::Draw()
	{
		_render_api->Draw();
	}

	void RenderCommand::Destory()
	{
		_render_api->Destroy();
		delete _render_api;
	}
}
