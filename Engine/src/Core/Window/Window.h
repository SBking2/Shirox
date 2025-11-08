#pragma once
#include "Core/Singleton/Singleton.h"
#include "Core/Event/Event.h"

#define GLFW_INCLUED_VULKAN
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <GLM/glm.hpp>

#include <string>
#include <functional>
namespace ev
{
	class Window : public Singleton<Window>
	{
	public:
		void Init(uint32_t width, uint32_t height, const std::string& title);
		GLFWwindow* GetWindow();
		void Clear();
		bool IsShouldClose();
		void HandleEvent();
		void SetEventCallback(std::function<void(const Event& e)> callback);
		void OnEvent(const Event& e);
		void SetCursorLockMode(bool is_lock);

		inline glm::vec2 GetLastMousePos() { return _last_mouse_pos; }
		inline void SetLastMousePos(const glm::vec2& pos) { _last_mouse_pos = pos; }
	private:
		std::function<void(const Event& e)> _event_callback;
		GLFWwindow* _window;
		glm::vec2 _last_mouse_pos;
	};
}