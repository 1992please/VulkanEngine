@echo off
@echo "------------------------- configuring -------------------------"
cmake -S . -B build/
echo "------------------------- building -------------------------"
cmake --build ./build/ --config Debug
@echo "------------------------- running -------------------------"
.\build\Debug\VulkanEngine.exe