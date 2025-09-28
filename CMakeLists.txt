cmake_minimum_required(VERSION 4.0)
project(vectra)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find OpenGL package
find_package(OpenGL REQUIRED)

# Add GLFW (you may need to install this)
# Option 1: If you have GLFW installed system-wide
find_package(glfw3 REQUIRED)

# Option 2: Or use FetchContent to download it
# include(FetchContent)
# FetchContent_Declare(
#   glfw
#   GIT_REPOSITORY https://github.com/glfw/glfw.git
#   GIT_TAG 3.3.8
# )
# FetchContent_MakeAvailable(glfw)

# Add your source files
set(SOURCES
        src/main.cpp
        # Add your other source files here
)

# Add GLAD (assuming you'll download it manually)
set(GLAD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/glad")
add_library(glad "${GLAD_DIR}/src/glad.c")
target_include_directories(glad PUBLIC "${GLAD_DIR}/include")



# Add this before add_executable
add_subdirectory(external/linkit)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Link libraries
target_link_libraries(${PROJECT_NAME}
        PRIVATE OpenGL::GL
        PRIVATE glfw
        PRIVATE glad
        PRIVATE linkit
)

# Include directories
target_include_directories(${PROJECT_NAME}
        PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include"
        PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/external"
)