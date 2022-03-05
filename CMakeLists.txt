cmake_minimum_required(VERSION 3.22.0)

project(VulkanEngine)
set(TARGET "${PROJECT_NAME}")
add_executable(${TARGET})

# GLFW
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(thirdparty/glfw)
target_link_libraries(${TARGET} glfw)

# GLM
target_include_directories(${TARGET} PRIVATE thirdparty/glm)

# Vulkan
find_package(Vulkan REQUIRED)
target_include_directories(${TARGET} PRIVATE "${Vulkan_INCLUDE_DIR}")
target_link_libraries(${TARGET} "${Vulkan_LIBRARY}")

# solution organisational work
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMakeTargets")
set_property(TARGET ${PROJECT_NAME} PROPERTY FOLDER Engine)

# set c++ standard
set_property(TARGET ${TARGET} PROPERTY CXX_STANDARD 20)
set_property(TARGET ${TARGET} PROPERTY CXX_STANDARD_REQUIRED ON)

#deciding files to add to project
file(GLOB_RECURSE PROJECT_SOURCE_FILES LIST_DIRECTORIES false RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} source/*.h source/*.hpp source/*.c??)

target_sources(${TARGET} PRIVATE ${PROJECT_SOURCE_FILES})

# set up visual studio groups
foreach(FILE ${PROJECT_SOURCE_FILES})
	get_filename_component(PARENT_DIR "${FILE}" PATH)

	# skip src or include and changes /'s to \\'s
	set(GROUP "${PARENT_DIR}")
	string(REPLACE "/" "\\" GROUP "${GROUP}")

	source_group("${GROUP}" FILES "${FILE}")
endforeach()