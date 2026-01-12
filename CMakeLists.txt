cmake_minimum_required(VERSION 4.0)
project(vectra)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# --- 1. OpenGL (System Driver - Must be found on system) ---
find_package(OpenGL REQUIRED)

# --- 2. GLFW (Windowing) ---
# Automatically download key GLFW version
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG        3.4  # Or 3.3.8
)
# Disable GLFW bloat
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(glfw)

# --- 3. GLM (Math) ---
FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG        0.9.9.8
)
set(GLM_TEST_ENABLE OFF CACHE BOOL "" FORCE)

# Use Populate to bypass GLM's incompatible CMakeLists.txt
FetchContent_GetProperties(glm)
if(NOT glm_POPULATED)
    # Suppress CMP0169 warning for direct population
    if(POLICY CMP0169)
        cmake_policy(SET CMP0169 OLD)
    endif()

    FetchContent_Populate(glm)

    if(POLICY CMP0169)
        cmake_policy(SET CMP0169 NEW)
    endif()

    add_library(glm INTERFACE)
    target_include_directories(glm INTERFACE "${glm_SOURCE_DIR}")
    add_library(glm::glm ALIAS glm)
endif()

# --- 4. Assimp (Asset Import) ---
# Warning: Assimp takes a while to compile the first time
FetchContent_Declare(
    assimp
    GIT_REPOSITORY https://github.com/assimp/assimp.git
    GIT_TAG        v5.3.1
)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_INJECT_DEBUG_POSTFIX OFF CACHE BOOL "" FORCE)
set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(assimp)

# Create alias to match your existing linking (assimp::assimp)
if(TARGET assimp AND NOT TARGET assimp::assimp)
    add_library(assimp::assimp ALIAS assimp)
endif()

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
        src/physics/colliders/collider_sphere.cpp
        src/physics/collision_data.cpp
        src/physics/collision_contact.cpp
        src/physics/collision_handler.cpp
        src/physics/colliders/collider_box.cpp
        src/physics/collider_primitive.cpp
        src/rendering/engine_ui.cpp
        src/core/engine.cpp

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
        COMMENT "Copying model files to build directory"
)

add_custom_command(
        TARGET ${PROJECT_NAME} PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/resources/textures/
        ${CMAKE_BINARY_DIR}/resources/textures/
        COMMENT "Copying texture files to build directory"
)


# Add this before add_executable
add_subdirectory(external/linkit)

# ImGui submodule configuration
set(IMGUI_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/external/imgui")
set(IMGUI_SOURCES
    "${IMGUI_ROOT}/imgui.cpp"
    "${IMGUI_ROOT}/imgui_demo.cpp"
    "${IMGUI_ROOT}/imgui_draw.cpp"
    "${IMGUI_ROOT}/imgui_tables.cpp"
    "${IMGUI_ROOT}/imgui_widgets.cpp"
    "${IMGUI_ROOT}/backends/imgui_impl_glfw.cpp"
    "${IMGUI_ROOT}/backends/imgui_impl_opengl3.cpp"
)

if(EXISTS "${IMGUI_ROOT}/imgui.cpp")
    add_library(imgui STATIC ${IMGUI_SOURCES})

    target_include_directories(imgui PUBLIC
        "${IMGUI_ROOT}"
        "${IMGUI_ROOT}/backends"
    )

    target_link_libraries(imgui PUBLIC OpenGL::GL)

    # Link against the FetchContent targets
    target_link_libraries(imgui PUBLIC glfw)
else()
    message(WARNING "ImGui submodule not found at ${IMGUI_ROOT}. Please run: git submodule update --init --recursive")
endif()
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
        PRIVATE imgui
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
