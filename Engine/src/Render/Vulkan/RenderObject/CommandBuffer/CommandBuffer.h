#pragma once
#include "Render/Vulkan/Core/Device.h"
#include "Render/Vulkan/Core/CommandPool.h"
#define GLFW_INCLUED_VULKAN
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <memory>
namespace ev
{
	class CommandBuffer final
	{
	public:
		static std::shared_ptr<CommandBuffer> Create();
		void BeginCommand(const Device& device, const CommandPool& pool);
		void EndCommand(const Device& device, const CommandPool& pool);
		inline const VkCommandBuffer& GetBuffer() const { return _command_buffer; }
	private:
		VkCommandBuffer _command_buffer;
	};
}