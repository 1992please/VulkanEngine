#include "ve_pipeline.h"

//std
#include <fstream>
#include <iostream>

namespace ve
{
    VePipeline::VePipeline(const std::string& vertFilePath, const std::string& fragFilePath)
    {
        createGraphicsPipeline(vertFilePath, fragFilePath);
    }

    std::vector<char> VePipeline::readFile(const std::string& filePath)
    {
        // std::ios::ate to seek to the end of the file immediatly
        // std::ios::binary to read the file as binary to avoid any unwanted text tranformation from happening
        std::ifstream file(filePath, std::ios::ate | std::ios::binary);

        if(!file.is_open())
        {
            throw std::runtime_error("failed to open file: " + filePath);
        }

        size_t fileSize = static_cast<size_t>(file.tellg());
        std::vector<char> buffer(fileSize);

        file.seekg(0);
        file.read(buffer.data(), fileSize);

        file.close();
        return buffer;
    }

    void VePipeline::createGraphicsPipeline(const std::string& vertFilePath, const std::string& fragFilePath)
    {
        std::vector<char> vertCode = readFile(vertFilePath);
        std::vector<char> fragCode = readFile(fragFilePath);

        std::cout<< "Vertex Shader Code Size: " << vertCode.size() << "\n";
        std::cout<< "Frag Shader Code Size: " << fragCode.size() << "\n";

    }
}