#pragma once
#include "Render/Vulkan/Core/Device.h"
#include "Render/Vulkan/RenderObject/Image/Image.h"
#define GLFW_INCLUED_VULKAN
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <memory>
namespace ev
{
	typedef uint32_t AspectMask;

	class ImageView
	{
	public:
		static std::shared_ptr<ImageView> Create(const Device& device, const Image& img, VkFormat format, AspectMask aspect_mask);
		void Destroy(const Device& device);
		inline const VkImageView& GetImageView() const { return _img_view; }
	private:
		void Init(const Device& device, const Image& img, VkFormat format, AspectMask aspect_mask);
		VkImageView _img_view;
	};
}