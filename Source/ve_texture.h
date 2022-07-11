#pragma once

#include "ve_device.h"

// std
#include <memory>
#include <string>

namespace ve
{
	class VeTexture
	{
	public:
		VeTexture(VeDevice& device, const std::string& textureFilepath);
		VeTexture(
			VeDevice& device,
			VkFormat format,
			VkExtent3D extent,
			VkImageUsageFlags usage,
			VkSampleCountFlagBits sampleCount);
		~VeTexture();

		// delete copy constructors
		VeTexture(const VeTexture&) = delete;
		VeTexture& operator=(const VeTexture&) = delete;

		VkImageView imageView() const { return mTextureImageView; }
		VkSampler sampler() const { return mTextureSampler; }
		VkImage getImage() const { return mTextureImage; }
		VkImageView getImageView() const { return mTextureImageView; }
		VkDescriptorImageInfo getImageInfo() const { return mDescriptor; }
		VkImageLayout getImageLayout() const { return mTextureLayout; }
		VkExtent3D getExtent() const { return mExtent; }
		VkFormat getFormat() const { return mFormat; }

		void updateDescriptor();
		void transitionLayout(
			VkCommandBuffer commandBuffer, VkImageLayout oldLayout, VkImageLayout newLayout);

		static std::unique_ptr<VeTexture> createTextureFromFile(
			VeDevice& device, const std::string& filepath);

	private:
		void createTextureImage(const std::string& filepath);
		void createTextureImageView(VkImageViewType viewType);
		void createTextureSampler();

		VkDescriptorImageInfo mDescriptor{};

		VeDevice& mDevice;
		VkImage mTextureImage = nullptr;
		VkDeviceMemory mTextureImageMemory = nullptr;
		VkImageView mTextureImageView = nullptr;
		VkSampler mTextureSampler = nullptr;
		VkFormat mFormat;
		VkImageLayout mTextureLayout;
		uint32_t mMipLevels{ 1 };
		uint32_t mLayerCount{ 1 };
		VkExtent3D mExtent{};
	};

}  // namespace lve
