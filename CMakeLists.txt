cmake_minimum_required(VERSION 4.0)
project(vectra)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find OpenGL package
find_package(OpenGL REQUIRED)

find_package(glm CONFIG REQUIRED)




# assimp configuration
find_package(assimp REQUIRED)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_INJECT_DEBUG_POSTFIX OFF CACHE BOOL "" FORCE)
set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)




# Add your source files
set(SOURCES
        src/main.cpp
        # Add your other source files here
        src/rendering/shader.cpp
        src/physics/rigidbody.cpp
        src/core/gameobject.cpp
        src/rendering/mesh.cpp
        src/rendering/renderer.cpp
        src/core/scene.cpp
        src/rendering/camera.cpp
        src/rendering/model.cpp
        src/rendering/light_source.cpp
        src/physics/force_registry.cpp
        src/physics/forces/simple_gravity.cpp
        src/physics/forces/newtonian_gravity.cpp
        src/physics/forces/anchored_spring.cpp
        src/rendering/skybox.cpp
        src/physics/bounding_volumes/bounding_sphere.cpp
        src/rendering/debug_drawer.cpp
)

# Add GLAD (assuming you'll download it manually)
set(GLAD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/glad")
add_library(glad "${GLAD_DIR}/src/glad.c")
target_include_directories(glad PUBLIC "${GLAD_DIR}/include")



# Create resources directory in the build folder
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/resources/shaders)



# Create executable
add_executable(${PROJECT_NAME} ${SOURCES})

add_custom_command(
        TARGET ${PROJECT_NAME} PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/resources/shaders/
        ${CMAKE_BINARY_DIR}/resources/shaders/
        COMMENT "Copying shader files to build directory"
)

add_custom_command(
        TARGET ${PROJECT_NAME} PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/resources/models/
        ${CMAKE_BINARY_DIR}/resources/models/
        COMMENT "Copying shader files to build directory"
)


# Add this before add_executable
add_subdirectory(external/linkit)
add_subdirectory(src/rendering)
add_subdirectory(src/physics)
add_subdirectory(src/core)

set(RESOURCES_DIR "${CMAKE_BINARY_DIR}/resources")
target_compile_definitions(${PROJECT_NAME} PRIVATE "RESOURCES_PATH=\"${RESOURCES_DIR}\"")

# target_compile_definitions(rendering PRIVATE "RESOURCES_PATH=\"${RESOURCES_DIR}\"")

# Link libraries
target_link_libraries(${PROJECT_NAME}
        PRIVATE OpenGL::GL
        PRIVATE glfw
        PRIVATE glad
        PRIVATE glm::glm
        INTERFACE linkit
        PRIVATE rendering
        PRIVATE physics
        PRIVATE core
        PRIVATE assimp::assimp
)


# Include directories
target_include_directories(${PROJECT_NAME}
        PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include"
        PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/external"
        PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/resources"
)
