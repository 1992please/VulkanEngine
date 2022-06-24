#include "ve_model.h"

#include "ve_utils.h"

// libs
#include <tiny_obj_loader.h>
#include <glm/gtx/hash.hpp>

// std
#include <cassert>
#include <cstring>
#include <iostream>

namespace std
{
	template<>
	struct hash<ve::VeModel::Vertex>
	{
		size_t operator()(ve::VeModel::Vertex const& vertex) const
		{
			size_t seed = 0;
			ve::hashCombine(seed, vertex.position, vertex.color, vertex.normal, vertex.uv);
			return seed;
		}
	};
}

namespace ve
{
	VeModel::VeModel(VeDevice& device, const VeModel::Builder& builder)
		: veDevice(device), hasIndexBuffer(false)
	{
		createVertexBuffers(builder.vertices);
		createIndexBuffers(builder.indices);
	}

	VeModel::~VeModel()
	{
	}

	std::unique_ptr<VeModel> VeModel::createModelFromFile(VeDevice& device, const std::string& filepath)
	{
		Builder builder{};
		builder.loadModel(filepath);
		std::cout << "Vertex count: " << builder.vertices.size() << "\n";
		return std::make_unique<VeModel>(device, builder);
	}

	void VeModel::bind(VkCommandBuffer commandBuffer)
	{
		VkDeviceSize offset = 0;
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, &vertexBuffer->getBuffer(), &offset);
		if (hasIndexBuffer)
			vkCmdBindIndexBuffer(commandBuffer, indexBuffer->getBuffer(), 0, VK_INDEX_TYPE_UINT32);
	}

	void VeModel::draw(VkCommandBuffer commandBuffer)
	{
		if (hasIndexBuffer)
			vkCmdDrawIndexed(commandBuffer, indexCount, 1, 0, 0, 0);
		else
			vkCmdDraw(commandBuffer, vertexCount, 1, 0, 0);
	}



	void VeModel::createVertexBuffers(const std::vector<Vertex>& vertices)
	{
		vertexCount = static_cast<uint32_t>(vertices.size());
		assert(vertexCount >= 3 && "Vertex count must be at least 3");

		uint32_t vertexSize = sizeof(vertices[0]);

		// NOTE host means cpu and device means gpu
		// VK_BUFFER_USAGE_TRANSFER_SRC_BIT tell vulkan that the buffer we are creating is going to be used as the source location for our memroy transfer operation.
		// VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT  staging buffer will need to be host visible
		// VK_MEMORY_PROPERTY_HOST_COHERENT_BIT whenever we change data on the host side to automatically flush that data to the device side

		VeBuffer stagingBuffer{
			veDevice,
			vertexSize,
			vertexCount,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		};

		stagingBuffer.map();
		stagingBuffer.writeToBuffer((void*)vertices.data());
		stagingBuffer.unmap();

	// VK_BUFFER_USAGE_TRANSFER_DST_BIT this will be the destination of transfering data from staging buffer.
	// VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT the most optimal but it can be accessed only by a staging buffer.

		vertexBuffer = std::make_unique<VeBuffer>(
			veDevice,
			vertexSize,
			vertexCount,
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		);

		veDevice.copyBuffer(stagingBuffer.getBuffer(), vertexBuffer->getBuffer(), vertexBuffer->getBufferSize());
	}

	void VeModel::createIndexBuffers(const std::vector<uint32_t>& indices)
	{
		indexCount = static_cast<uint32_t>(indices.size());
		hasIndexBuffer = indexCount > 0;

		if (!hasIndexBuffer)
			return;

		uint32_t indexSize = sizeof(indices[0]);
		// NOTE host means cpu and device means gpu
		// VK_BUFFER_USAGE_TRANSFER_SRC_BIT tell vulkan that the buffer we are creating is going to be used as the source location for our memroy transfer operation.
		// VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT  staging buffer will need to be host visible
		// VK_MEMORY_PROPERTY_HOST_COHERENT_BIT whenever we change data on the host side to automatically flush that data to the device side

		VeBuffer stagingBuffer{
			veDevice,
			indexSize,
			indexCount,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		};

		stagingBuffer.map();
		stagingBuffer.writeToBuffer((void*)indices.data());


		// VK_BUFFER_USAGE_TRANSFER_DST_BIT this will be the destination of transfering data from staging buffer.
		// VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT the most optimal but it can be accessed only by a staging buffer.
		indexBuffer = std::make_unique<VeBuffer>(
			veDevice,
			indexSize,
			indexCount,
			VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		);

		veDevice.copyBuffer(stagingBuffer.getBuffer(), indexBuffer->getBuffer(), stagingBuffer.getBufferSize());
	}

	std::vector<VkVertexInputBindingDescription> VeModel::Vertex::getBindingDescriptions()
	{
		std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
		bindingDescriptions[0].binding = 0;
		bindingDescriptions[0].stride = sizeof(Vertex);
		bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		return bindingDescriptions;
	}

	std::vector<VkVertexInputAttributeDescription> VeModel::Vertex::getAttributeDescriptions()
	{
		std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};

		attributeDescriptions.push_back({ 0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, position) });
		attributeDescriptions.push_back({ 1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, color) });
		attributeDescriptions.push_back({ 2, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, normal) });
		attributeDescriptions.push_back({ 3, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, uv) });

		return attributeDescriptions;
	}

	void VeModel::Builder::loadModel(const std::string& filepath)
	{
		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string warn, err;

		if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filepath.c_str()))
			throw std::runtime_error(warn + err);

		vertices.clear();
		indices.clear();

		std::unordered_map<Vertex, uint32_t> uniqueVertices{};
		for (const tinyobj::shape_t& shape : shapes)
		{
			for (const tinyobj::index_t& index : shape.mesh.indices)
			{
				Vertex vertex{};
				if (index.vertex_index >= 0)
				{
					vertex.position = {
						attrib.vertices[3 * index.vertex_index + 0],
						attrib.vertices[3 * index.vertex_index + 1],
						attrib.vertices[3 * index.vertex_index + 2]
					};

					vertex.color = {
						attrib.colors[3 * index.vertex_index + 0],
						attrib.colors[3 * index.vertex_index + 1],
						attrib.colors[3 * index.vertex_index + 2]
					};
				}

				if (index.normal_index >= 0)
				{
					vertex.normal = {
						attrib.normals[3 * index.normal_index + 0],
						attrib.normals[3 * index.normal_index + 1],
						attrib.normals[3 * index.normal_index + 2]
					};
				}

				if (index.texcoord_index >= 0)
				{
					vertex.uv = {
						attrib.texcoords[2 * index.texcoord_index + 0],
						attrib.texcoords[2 * index.texcoord_index + 1]
					};
				}
				if (uniqueVertices.count(vertex) == 0)
				{
					uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
					vertices.push_back(vertex);
				}
				indices.push_back(uniqueVertices[vertex]);
			}
		}
	}
}