#pragma once

#include "ve_device.h"

// std
#include <string>

// lib
#include <glm/glm.hpp>

namespace ve
{
	class VeTexture
	{
	public:
		VeTexture(const std::string& pathToTexture, VeDevice& device);
		VeTexture(glm::vec4 color, VeDevice& device);
		~VeTexture();

		VeTexture(const VeTexture&) = delete;
		VeTexture& operator=(const VeTexture&) = delete;

	private:

		VeDevice& veDevice;
		VkImage textureImage;
		VkDeviceMemory textureImageMemory;
	};
}

