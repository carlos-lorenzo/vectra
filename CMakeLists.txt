cmake_minimum_required(VERSION 3.16...4.0)
project(vectra)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# Set policies for FetchContent
if(POLICY CMP0169)
    cmake_policy(SET CMP0169 NEW)
endif()

# --- 1. OpenGL (System Driver - Must be found on system) ---
find_package(OpenGL REQUIRED)

# --- 2. GLFW (Windowing) ---
# Automatically download GLFW
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG        3.4
    GIT_SHALLOW    TRUE
)
# Disable GLFW bloat
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(glfw)

# --- 3. GLM (Math) ---
# Use a newer GLM version that has compatible CMake
FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG        1.0.1
    GIT_SHALLOW    TRUE
)
set(GLM_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLM_ENABLE_CXX_17 ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(glm)

# Create alias if not already defined
if(TARGET glm AND NOT TARGET glm::glm)
    add_library(glm::glm ALIAS glm)
endif()

# --- 4. Assimp (Asset Import) ---
# Warning: Assimp takes a while to compile the first time

# Windows-specific fixes for MSVC runtime
if(WIN32)
    if(POLICY CMP0091)
        cmake_policy(SET CMP0091 NEW)
    endif()
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>" CACHE STRING "" FORCE)
endif()

FetchContent_Declare(
    assimp
    GIT_REPOSITORY https://github.com/assimp/assimp.git
    GIT_TAG        v5.4.3
    GIT_SHALLOW    TRUE
)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_INJECT_DEBUG_POSTFIX OFF CACHE BOOL "" FORCE)
set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)
set(ASSIMP_NO_EXPORT ON CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ZLIB ON CACHE BOOL "" FORCE)
set(ASSIMP_WARNINGS_AS_ERRORS OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(assimp)

# Create alias to match your existing linking (assimp::assimp)
if(TARGET assimp AND NOT TARGET assimp::assimp)
    add_library(assimp::assimp ALIAS assimp)
endif()

# --- 5. GLAD (OpenGL Loader - included in repo) ---
set(GLAD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/glad")
if(EXISTS "${GLAD_DIR}/src/glad.c")
    add_library(glad STATIC "${GLAD_DIR}/src/glad.c")
    target_include_directories(glad PUBLIC "${GLAD_DIR}/include")
else()
    message(FATAL_ERROR "GLAD not found at ${GLAD_DIR}. Please ensure external/glad is present.")
endif()

# --- 6. ImGui (Git Submodule) ---
set(IMGUI_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/external/imgui")
if(EXISTS "${IMGUI_ROOT}/imgui.cpp")
    set(IMGUI_SOURCES
        "${IMGUI_ROOT}/imgui.cpp"
        "${IMGUI_ROOT}/imgui_demo.cpp"
        "${IMGUI_ROOT}/imgui_draw.cpp"
        "${IMGUI_ROOT}/imgui_tables.cpp"
        "${IMGUI_ROOT}/imgui_widgets.cpp"
        "${IMGUI_ROOT}/backends/imgui_impl_glfw.cpp"
        "${IMGUI_ROOT}/backends/imgui_impl_opengl3.cpp"
    )

    add_library(imgui STATIC ${IMGUI_SOURCES})

    # PUBLIC ensures include dirs propagate to all targets linking imgui
    target_include_directories(imgui PUBLIC
        "${IMGUI_ROOT}"
        "${IMGUI_ROOT}/backends"
    )

    target_link_libraries(imgui PUBLIC glfw OpenGL::GL)
else()
    message(FATAL_ERROR
        "ImGui submodule not found at ${IMGUI_ROOT}.\n"
        "Please run: git submodule update --init --recursive"
    )
endif()

# --- 7. Linkit (External Library) ---
add_subdirectory(external/linkit)

# --- 8. Project Subdirectories (must come after all dependencies are defined) ---
add_subdirectory(src/rendering)
add_subdirectory(src/physics)
add_subdirectory(src/core)

# --- 9. Main Executable ---
set(SOURCES
    src/main.cpp
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
    src/physics/colliders/collider_sphere.cpp
    src/physics/collision_data.cpp
    src/physics/collision_contact.cpp
    src/physics/collision_handler.cpp
    src/physics/colliders/collider_box.cpp
    src/physics/collider_primitive.cpp
    src/rendering/engine_ui.cpp
    src/core/engine.cpp
        src/physics/forces/object_anchored_spring.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

# --- 10. Resource Copying ---
# Create resource directories in build folder
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/resources/shaders)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/resources/models)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/resources/textures)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/resources/fonts)

# Copy resources only if source directories exist
if(EXISTS "${CMAKE_SOURCE_DIR}/resources/shaders")
    add_custom_command(
        TARGET ${PROJECT_NAME} PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/resources/shaders"
        "${CMAKE_BINARY_DIR}/resources/shaders"
        COMMENT "Copying shader files to build directory"
    )
endif()

if(EXISTS "${CMAKE_SOURCE_DIR}/resources/models")
    add_custom_command(
        TARGET ${PROJECT_NAME} PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/resources/models"
        "${CMAKE_BINARY_DIR}/resources/models"
        COMMENT "Copying model files to build directory"
    )
endif()

if(EXISTS "${CMAKE_SOURCE_DIR}/resources/textures")
    add_custom_command(
        TARGET ${PROJECT_NAME} PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/resources/textures"
        "${CMAKE_BINARY_DIR}/resources/textures"
        COMMENT "Copying texture files to build directory"
    )
endif()

if(EXISTS "${CMAKE_SOURCE_DIR}/resources/fonts")
    add_custom_command(
        TARGET ${PROJECT_NAME} PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/resources/fonts"
        "${CMAKE_BINARY_DIR}/resources/fonts"
        COMMENT "Copying font files to build directory"
    )
endif()

# --- 11. Resources Path Definition ---
set(RESOURCES_DIR "${CMAKE_BINARY_DIR}/resources")
target_compile_definitions(${PROJECT_NAME} PRIVATE "RESOURCES_PATH=\"${RESOURCES_DIR}\"")

# --- 12. Link Libraries ---
target_link_libraries(${PROJECT_NAME}
    PRIVATE
        OpenGL::GL
        glfw
        glad
        glm::glm
        imgui
        rendering
        physics
        core
        assimp::assimp
    INTERFACE
        linkit
)

# --- 13. Include Directories ---
target_include_directories(${PROJECT_NAME}
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/external"
        "${CMAKE_CURRENT_SOURCE_DIR}/resources"
)
