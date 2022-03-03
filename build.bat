@echo off
@echo "------------------------- building -------------------------"
cmake --build ./build/ --config Release
@echo "------------------------- running -------------------------"
.\build\Release\VulkanEngine.exe