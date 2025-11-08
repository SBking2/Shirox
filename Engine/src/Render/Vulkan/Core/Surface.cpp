#include "PCH.h"
#include "Surface.h"
#include <stdexcept>
namespace srx
{
	void Surface::Init(GLFWwindow* window, const Instance& instance)
	{
		if (glfwCreateWindowSurface(instance.GetInstance(), window, nullptr, &_surface) != VK_SUCCESS)
			throw std::runtime_error("failed to create surface!");
	}
	void Surface::Destroy(const Instance& instance)
	{
		vkDestroySurfaceKHR(instance.GetInstance(), _surface, nullptr);
	}
}