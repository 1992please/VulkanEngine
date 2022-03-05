@echo off
@echo "------------------------- compile shaders -------------------------"
"C:\VulkanSDK\1.3.204.1\Bin\glslc.exe" shaders/simple_shader.vert -o shaders/simple_shader.vert.spv
"C:\VulkanSDK\1.3.204.1\Bin\glslc.exe" shaders/simple_shader.frag -o shaders/simple_shader.frag.spv