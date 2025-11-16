#pragma once

namespace srx
{
	class VulkanPhysicalDevice final
	{
	public:
		struct QueueFamilyIndices
		{
			uint32_t graphic = -1;		//图形队列
			uint32_t compute = -1;		//计算队列
			uint32_t transfer = -1;		//传输队列
		};
		VulkanPhysicalDevice();
		virtual ~VulkanPhysicalDevice();
		bool IsSupportedExtension(const std::string& extension_name);
		bool IsSupportedLayer(const std::string& layer_name);
		inline VkPhysicalDevice GetPhysicalDevice() { return _PhysicalDevice; }
		inline QueueFamilyIndices GetQueueFamilyIndices() { return _QueueFamilyIndices; }
	private:
		QueueFamilyIndices GetQueueFamilyIndices(const std::vector<VkQueueFamilyProperties>& queue_family, uint32_t require_queue);
	private:
		//std::vector<VkQueueFamilyProperties> _QeueFamilyProperties;
		VkPhysicalDevice _PhysicalDevice;
		QueueFamilyIndices _QueueFamilyIndices;
		std::set<std::string> _ExtensionPropertires;	//支持的Extension
		std::set<std::string> _LayerProerties;				//支持的Layer
	};

	class VulkanDevice final
	{
	public:
		VulkanDevice(Ref<VulkanPhysicalDevice> physical_device
			, VkPhysicalDeviceFeatures enable_features);
		virtual ~VulkanDevice();
		void Destroy();
	private:
		VkDevice _Device;
		VkPhysicalDeviceFeatures _EnableFeatures;
	};
}