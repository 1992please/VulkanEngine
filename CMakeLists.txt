cmake_minimum_required(VERSION 3.22.0)
project(VulkanEngine)
add_executable(${PROJECT_NAME} source/main.cpp)

# GLFW
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(thirdparty/glfw)
target_link_libraries(${PROJECT_NAME} glfw)

# GLM
target_include_directories(${PROJECT_NAME} PRIVATE thirdparty/glm)

# solution organisational work
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMakeTargets")