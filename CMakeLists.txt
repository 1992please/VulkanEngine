cmake_minimum_required(VERSION 3.22.0)

project(VulkanEngine)
set(MainTarget "${PROJECT_NAME}")
set(MISC_FOLDER "Misc")
add_executable(${MainTarget})
target_include_directories(${MainTarget} PRIVATE source/)

# GLFW
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(thirdparty/glfw)
target_link_libraries(${MainTarget} glfw)
set_target_properties(glfw PROPERTIES FOLDER "${MISC_FOLDER}/Thirdparty/GLFW3")
set_target_properties(update_mappings PROPERTIES FOLDER "${MISC_FOLDER}/Thirdparty/GLFW3")
set_target_properties(uninstall PROPERTIES FOLDER "${MISC_FOLDER}/Thirdparty/GLFW3")

# GLM
add_compile_definitions(GLM_FORCE_DEPTH_ZERO_TO_ONE)
target_include_directories(${MainTarget} PRIVATE thirdparty/glm)

# Vulkan
find_package(Vulkan REQUIRED)
target_include_directories(${MainTarget} PRIVATE "${Vulkan_INCLUDE_DIR}")
target_link_libraries(${MainTarget} "${Vulkan_LIBRARY}")

# tinyobjloader
add_subdirectory(thirdparty/tinyobjloader)
target_link_libraries(${MainTarget} tinyobjectloader)
set_target_properties(tinyobjectloader PROPERTIES FOLDER "${MISC_FOLDER}/Thirdparty/")

# set c++ standard
set_property(TARGET ${MainTarget} PROPERTY CXX_STANDARD 20)
set_property(TARGET ${MainTarget} PROPERTY CXX_STANDARD_REQUIRED ON)

# set debugging working directory
set_property(TARGET ${MainTarget} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

# set startup project
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${MainTarget})

#deciding files to add to project
file(GLOB_RECURSE PROJECT_SOURCE_FILES LIST_DIRECTORIES false RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} 
	source/*.h source/*.hpp source/*.c?? 
	shaders/*.frag shaders/*.vert 
	thirdparty/glm/util/glm.natvis
)
file(GLOB_RECURSE SHADER_SOURCE_FILES LIST_DIRECTORIES false RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} shaders/*.frag shaders/*.vert)

target_sources(${MainTarget} PRIVATE ${PROJECT_SOURCE_FILES})

# set up visual studio groups
foreach(FILE ${PROJECT_SOURCE_FILES})
	get_filename_component(PARENT_DIR "${FILE}" DIRECTORY)

	# skip src or include and changes /'s to \\'s
	set(GROUP "${PARENT_DIR}")
	string(REPLACE "/" "\\" GROUP "${GROUP}")

	source_group("${GROUP}" FILES "${FILE}")
endforeach()

############## Build SHADERS #######################
 
find_program(GLSL_VALIDATOR glslangValidator HINTS 
	${Vulkan_GLSLANG_VALIDATOR_EXECUTABLE} 
	/usr/bin 
	/usr/local/bin 
	${VULKAN_SDK_PATH}/Bin
	${VULKAN_SDK_PATH}/Bin32
	$ENV{VULKAN_SDK}/Bin/ 
	$ENV{VULKAN_SDK}/Bin32/
)

# get all .vert and .frag files in shaders directory
file(GLOB_RECURSE GLSL_SOURCE_FILES
	"${PROJECT_SOURCE_DIR}/shaders/*.frag"
	"${PROJECT_SOURCE_DIR}/shaders/*.vert"
)
 
foreach(GLSL ${SHADER_SOURCE_FILES})
	# get_filename_component(FILE_NAME ${GLSL} NAME)
	string(REPLACE "shaders/" "compiled_shaders/" SPIRV "${CMAKE_CURRENT_SOURCE_DIR}/${GLSL}.spv")
	set(GLSL "${PROJECT_SOURCE_DIR}/${GLSL}")
	message("GLSL: ${GLSL}, \nSPIRV: ${SPIRV}\n")
	# set(SPIRV "${PROJECT_SOURCE_DIR}/compiled_shaders/${FILE_NAME}.spv")
	add_custom_command(
		OUTPUT ${SPIRV}
		COMMAND ${GLSL_VALIDATOR} -V ${GLSL} -o ${SPIRV}
		DEPENDS ${GLSL}
		COMMENT "Compiling ${GLSL} shader..."
	)
	list(APPEND SPIRV_BINARY_FILES ${SPIRV})
endforeach(GLSL)

add_custom_target(
	Shaders
	DEPENDS ${SPIRV_BINARY_FILES}
)

add_dependencies(${MainTarget} Shaders)

# solution organisational work
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "${MISC_FOLDER}/CMakeTargets")
set_property(TARGET ${MainTarget} PROPERTY FOLDER Engine)
set_property(TARGET Shaders PROPERTY FOLDER Engine)