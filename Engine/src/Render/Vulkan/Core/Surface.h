#pragma once
#include "Instance.h"
#define GLFW_INCLUED_VULKAN
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
namespace ev
{
	class Surface final
	{
	public:
		void Init(GLFWwindow* window, const Instance& instance);
		void Destroy(const Instance& instance);
		inline const VkSurfaceKHR& GetSurface() const { return _surface; }
	private:
		VkSurfaceKHR _surface;
	};
}