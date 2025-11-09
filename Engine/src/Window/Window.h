#pragma once
#define GLFW_INCLUED_VULKAN
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

namespace srx
{
	class Window : public Singleton<Window>
	{
	public:
		void Init(uint32_t width, uint32_t height, const std::string& title);
		GLFWwindow* GetWindow() const;
		void Clear();
		bool IsShouldClose();
		void HandleEvent();
		void SetEventCallback(std::function<void(const Event& e)> callback);
		void OnEvent(const Event& e);
		void SetCursorLockMode(bool is_lock);

		inline glm::vec2 GetLastMousePos() { return _last_mouse_pos; }
		inline void SetLastMousePos(const glm::vec2& pos) { _last_mouse_pos = pos; }
		inline void GetWdithAndHeight(uint32_t& width, uint32_t& height) const { width = this->width; height = this->height; }
	private:
		inline void SetWidthAndHeight(uint32_t width, uint32_t height) { this->width = width; this->height = height; }
		void OnResizeEvent(const WindowResizeEvent& e);
	private:
		std::function<void(const Event& e)> _event_callback;
		GLFWwindow* _window;
		uint32_t width, height;
		glm::vec2 _last_mouse_pos;
	};
}