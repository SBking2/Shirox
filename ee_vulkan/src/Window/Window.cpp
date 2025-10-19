#include "Window.h"
#include "Vulkan/VulkanContext.h"
namespace ev
{
	static void framebuffer_resize_callback(GLFWwindow* window, int width, int height)
	{
		WindowResizeEvent e(static_cast<uint32_t>(width), static_cast<uint32_t>(height));
		Window* win = (Window*)glfwGetWindowUserPointer(window);
		win->OnEvent(e);
	}

	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		KeyEvent e(key, scancode, action, mods);
		Window* win = (Window*)glfwGetWindowUserPointer(window);
		win->OnEvent(e);
	}

	void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
	{
		MouseButtonEvent e(button, action, mods);
		Window* win = (Window*)glfwGetWindowUserPointer(window);
		win->OnEvent(e);
	}

	void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
	{
		Window* win = (Window*)glfwGetWindowUserPointer(window);
		glm::vec2 last_mouse_pos = win->GetLastMousePos();
		MouseMoveEvent e(xpos, ypos, xpos - last_mouse_pos.x, ypos - last_mouse_pos.y);
		win->OnEvent(e);
		win->SetLastMousePos(glm::vec2(xpos, ypos));
	}

	void Window::Init(uint32_t width, uint32_t height, const std::string& title)
	{
		//GLFW窗口初始化
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);	//阻止自动创建OpenGL上下文
		_window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);

		glfwSetWindowUserPointer(_window, this);

		glfwSetFramebufferSizeCallback(_window, framebuffer_resize_callback);
		glfwSetKeyCallback(_window, key_callback);
		glfwSetMouseButtonCallback(_window, mouse_button_callback);
		glfwSetCursorPosCallback(_window, cursor_position_callback);
	}

	GLFWwindow* Window::GetWindow()
	{
		return _window;
	}

	void Window::Clear()
	{
		glfwDestroyWindow(_window);
		glfwTerminate();
	}

	void Window::SetCursorLockMode(bool is_lock)
	{
		glfwSetInputMode(_window, GLFW_CURSOR, is_lock ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
	}

	void Window::HandleEvent()
	{
		glfwPollEvents();
	}

	void Window::SetEventCallback(std::function<void(const Event& e)> callback)
	{
		_event_callback = callback;
	}

	void Window::OnEvent(const Event& e)
	{
		_event_callback(e);
	}

	bool Window::IsShouldClose()
	{
		return glfwWindowShouldClose(_window);
	}
}