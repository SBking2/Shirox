#pragma once
#include "Renderer/RendererContext.h"
#include "Platform/Renderer/Vulkan/VulkanDevice.h"
#include "Window/Window.h"
#include <vulkan/vulkan.h>
namespace srx
{
	class VulkanContext final : public RendererContext
	{
	//public:
	//	struct DeviceInfo
	//	{
	//		int graphic_queue_index = -1;
	//		int present_queue_index = -1;
	//		VkSurfaceCapabilitiesKHR capabilities;
	//		std::vector<VkSurfaceFormatKHR> formats;	//显卡支持的format
	//		std::vector<VkPresentModeKHR> present_modes;	//支持的present
	//	};
	//	inline VkDevice GetDevice() const { return _device; }
	//	inline VkSurfaceKHR GetSurface() const { return _surface; }
	//	inline const Window* GetWindow() const { return _window; }
	//	inline VkQueue GetGraphicQueue() const { return _graphic_queue; }
	//	inline VkQueue GetPresentQueue() const { return _present_queue; }
	//	void Init(const Window* window);
	//	void Clear();
	//private:
	//	void CreateInstance();
	//	void CreateDebugCallback();
	//	void CreateSurface(const Window* window);
	//	void CreateLogicalDevice();
	//	void FindBestDevice();
	//	void CreateCommandPool();
	//	void CreateDescriptorPool();
	//	int CheckDevice(DeviceInfo& device_info);
	//public:
	//	DeviceInfo device_info;
	//private:
	//	const Window* _window;

	//	VkInstance _instance;
	//	VkDebugUtilsMessengerEXT _debug_messager;	//存储回调函数信息
	//	VkSurfaceKHR _surface;
	//	VkPhysicalDevice _physical_device;
	//	VkDevice _device;
	//	VkCommandPool _command_pool;
	//	VkDescriptorPool _descriptor_pool;
	//	VkQueue _graphic_queue;
	//	VkQueue _present_queue;
	public:
		VulkanContext();
		virtual ~VulkanContext();
		void Init() override;
		void Destroy() override;
		VkInstance GetVkInstance() { return _Instance; }
		inline VkPhysicalDevice GetVkPhysicalDevice() { return _PhysicalDevice->GetVkPhysicalDevice(); }
		inline VkDevice GetVkDevice() { return _Device->GetVkDevice(); }
		inline Ref<VulkanDevice> GetDevice() { return _Device; }
	private:
		void CreateInstance();
		void CreateDebugCallback();
	private:
		VkInstance _Instance = nullptr;
		VkDebugUtilsMessengerEXT _DebugMessager = nullptr;
		Ref<VulkanPhysicalDevice> _PhysicalDevice;
		Ref<VulkanDevice> _Device;
	};
}