#pragma once
#include "Render/Vulkan/Core/Device.h"
#include "Render/Vulkan/RenderObject/Buffer/Buffer.h"
#include "Render/Vulkan/RenderObject/CommandBuffer/CommandBuffer.h"
#include "Render/Vulkan/Core/CommandPool.h"
#define GLFW_INCLUED_VULKAN
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <memory>
namespace ev
{
	typedef uint32_t ImageUsage;
	typedef uint32_t ImageProperty;

	class Image final
	{
	public:
		static std::shared_ptr<Image> Create(const Device& device, uint32_t width, uint32_t height, VkFormat format
			, VkImageTiling tiling
			, ImageUsage usage, ImageProperty properties);
		void Destroy(const Device& device);
		void InputData(const Device& device, uint32_t width, uint32_t height, const Buffer& buffer, const CommandPool& pool);
		inline const VkImage& GetImage() const { return _img; }
	private:
		void Init(const Device& device, uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling
			, ImageUsage usage, ImageProperty properties);
		VkImage _img;
		VkDeviceMemory _memory;
	};
}