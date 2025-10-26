#pragma once
#include <vector>
#define GLFW_INCLUED_VULKAN
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
namespace ev
{
	class Device final
	{
	public:
		void Init();
		void Destroy();
		inline const VkDevice& GetLogicalDevice() { return _logical_device; }
		inline const VkPhysicalDevice& GetPhysicalDevice() { return _physical_device; }
		inline const VkQueue& GetGraphicQueue() { return _graphic_queue; }
		inline const VkQueue& GetPresentQueue() { return _present_queue; }
		inline void SetPhysicalDevice(const VkPhysicalDevice& device) { _physical_device = device; }
	public:
		struct DeviceInfo
		{
			int graphic_queue_index = -1;
			int present_queue_index = -1;
			VkSurfaceCapabilitiesKHR capabilities;
			std::vector<VkSurfaceFormatKHR> formats;
			std::vector<VkPresentModeKHR> present_modes;
		};
		DeviceInfo device_info;
	private:
		VkPhysicalDevice _physical_device = VK_NULL_HANDLE;
		VkDevice _logical_device;
		VkQueue _graphic_queue;
		VkQueue _present_queue;
	};
}