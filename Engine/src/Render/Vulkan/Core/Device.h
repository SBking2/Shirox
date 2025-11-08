#pragma once
#include "Surface.h"
#include <vector>
#define GLFW_INCLUED_VULKAN
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
namespace srx
{
	class Device final
	{
	public:
		struct DeviceInfo
		{
			int graphic_queue_index = -1;
			int present_queue_index = -1;
			VkSurfaceCapabilitiesKHR capabilities;
			std::vector<VkSurfaceFormatKHR> formats;
			std::vector<VkPresentModeKHR> present_modes;
		};
		void Init(const Instance& instance, const Surface& surface);
		void Destroy();
		uint32_t FindMemoryType(uint32_t filter, VkMemoryPropertyFlags properties) const;
		inline const VkDevice& GetLogicalDevice() const { return _logical_device; }
		inline const VkPhysicalDevice& GetPhysicalDevice() const { return _physical_device; }
		inline const VkQueue& GetGraphicQueue() const { return _graphic_queue; }
		inline const VkQueue& GetPresentQueue() const { return _present_queue; }
	private:
		void FindBestDevice(const Instance& instance, const Surface& surface);
		int CheckDevice(VkPhysicalDevice physical_device, DeviceInfo& device_info, const VkSurfaceKHR& surface);
		void CreateDevice(const VkPhysicalDevice& physical_device);
	public:
		DeviceInfo device_info;
	private:
		VkPhysicalDevice _physical_device = VK_NULL_HANDLE;
		VkDevice _logical_device;
		VkQueue _graphic_queue;
		VkQueue _present_queue;
		
	};
}