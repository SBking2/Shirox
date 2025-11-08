#include "PCH.h"
#include "ImageView.h"
#include <stdexcept>
namespace srx
{
	std::shared_ptr<ImageView> ImageView::Create(const Device& device, const Image& img, VkFormat format, AspectMask aspect_mask)
	{
		std::shared_ptr<ImageView> view = std::make_shared<ImageView>();
		view->Init(device, img, format, aspect_mask);
		return view;
	}

	void ImageView::Destroy(const Device& device)
	{
		vkDestroyImageView(device.GetLogicalDevice(), _img_view, nullptr);
	}

	void ImageView::Init(const Device& device, const Image& img, VkFormat format, AspectMask aspect_mask)
	{
		VkImageViewCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		create_info.image = img.GetImage();
		create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
		create_info.format = format;
		create_info.subresourceRange.aspectMask = aspect_mask;
		create_info.subresourceRange.baseMipLevel = 0;
		create_info.subresourceRange.levelCount = 1;
		create_info.subresourceRange.baseArrayLayer = 0;
		create_info.subresourceRange.layerCount = 1;

		if (vkCreateImageView(device.GetLogicalDevice(), &create_info, nullptr, &_img_view) != VK_SUCCESS)
			throw std::runtime_error("failed to create texture img_view!");
	}
}