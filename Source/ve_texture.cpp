#include "ve_texture.h"
#include "ve_buffer.h"
// libs
#include <stb_image.h>

// std
#include <stdexcept>

namespace ve
{

	VeTexture::VeTexture(const std::string& pathToTexture, VeDevice& device)
		: veDevice(device), imagePath(pathToTexture)
	{
		createImage();
		createImageView();
		createSampler();
	}

	VkDescriptorImageInfo VeTexture::descriptorInfo()
	{
		return VkDescriptorImageInfo{
			sampler,
			imageView,
			VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
		};
	}

	void VeTexture::createImage()
	{
		int texWidth, texHeight, texChannels;
		stbi_uc* pixels = stbi_load(imagePath.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
		VkDeviceSize imageSize = texWidth * texHeight * 4;

		if (!pixels) {
			throw std::runtime_error("failed to load texture image!");
		}

		VeBuffer stagingBuffer{
			veDevice,
			imageSize,
			1,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		};

		stagingBuffer.map();
		stagingBuffer.writeToBuffer((void*)pixels);
		stagingBuffer.unmap();

		stbi_image_free(pixels);

		VkImageCreateInfo imageInfo{};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.extent.width = static_cast<uint32_t>(texWidth);
		imageInfo.extent.height = static_cast<uint32_t>(texHeight);
		imageInfo.extent.depth = 1;
		imageInfo.mipLevels = 1;
		imageInfo.arrayLayers = 1;
		imageInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
		imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageInfo.flags = 0; // Optional

		veDevice.createImageWithInfo(
			imageInfo,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			image,
			imageMemory);

		// transition the image to be of type transfer dst
		veDevice.transitionImageLayout(image, VK_FORMAT_R8G8B8A8_SRGB,
			VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		// copy buffer data to image
		veDevice.copyBufferToImage(stagingBuffer.getBuffer(), image,
			static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight), 1);
		// To be able to start sampling from the texture image in the shader, we need one last transition to prepare it for shader access
		veDevice.transitionImageLayout(image, VK_FORMAT_R8G8B8A8_SRGB,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	}

	void VeTexture::createImageView()
	{
		imageView = veDevice.createImageView(image, VK_FORMAT_R8G8B8A8_SRGB);
	}

	void VeTexture::createSampler()
	{
		// The magFilter and minFilter fields specify how to interpolate texels that are magnified or minified. 
		VkSamplerCreateInfo samplerInfo{};
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.magFilter = VK_FILTER_LINEAR;
		samplerInfo.minFilter = VK_FILTER_LINEAR;
		samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.maxAnisotropy = veDevice.properties.limits.maxSamplerAnisotropy;
		// The borderColor field specifies which color is returned when sampling beyond the image with clamp to border addressing mode. It is possible to return black, white or transparent in either float or int formats. You cannot specify an arbitrary color.
		samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		// If this field is VK_TRUE, then you can simply use coordinates within the [0, texWidth) and [0, texHeight) range
		samplerInfo.unnormalizedCoordinates = VK_FALSE;
		// can be used for shadow maps
		samplerInfo.compareEnable = VK_FALSE;
		samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
		// mipmaping 
		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		samplerInfo.mipLodBias = 0.0f;
		samplerInfo.minLod = 0.0f;
		samplerInfo.maxLod = 0.0f;

		if (vkCreateSampler(veDevice.device(), &samplerInfo, nullptr, &sampler) != VK_SUCCESS)
			throw std::runtime_error("failed to create texture sampler!");
	}

	VeTexture::~VeTexture()
	{
		vkDestroySampler(veDevice.device(), sampler, nullptr);
		vkDestroyImageView(veDevice.device(), imageView, nullptr);

		vkDestroyImage(veDevice.device(), image, nullptr);
		vkFreeMemory(veDevice.device(), imageMemory, nullptr);
	}

	VeTexture::VeTexture(glm::vec4 color, VeDevice& device)
		: veDevice(device)
	{

	}

}