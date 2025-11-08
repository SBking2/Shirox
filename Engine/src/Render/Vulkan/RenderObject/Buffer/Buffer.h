#pragma once
#include "Render/Vulkan/Core/Device.h"
#include "Render/Vulkan/RenderObject/CommandBuffer/CommandBuffer.h"
#include "Render/Vulkan/Core/CommandPool.h"
#define GLFW_INCLUED_VULKAN
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <memory>
namespace ev
{
	typedef uint32_t BufferUsage;
	typedef uint32_t BufferProperty;
	typedef uint64_t BufferSize;

	class Buffer final
	{
	public:
		static std::shared_ptr<Buffer> Create(const Device& device
			, BufferUsage usage, BufferProperty properties, BufferSize size);
		void Destroy(const Device& device);
		void InputData(const Device& device, const BufferSize& size, const void* data);
		void InputData(const Device& device, const BufferSize& size, const Buffer& buffer, const CommandPool& pool);
		inline const VkBuffer& GetBuffer() const { return _buffer; }
		inline const VkDeviceMemory& GetMemory() const { return _memory; }
	private:
		void Init(const Device& device
			, BufferUsage usage, BufferProperty properties, BufferSize size);
		VkBuffer _buffer;
		VkDeviceMemory _memory;
	};
}