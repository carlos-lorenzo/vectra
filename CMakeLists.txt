cmake_minimum_required(VERSION 3.15)
project(vectra)

set(CMAKE_CXX_STANDARD 26)

add_executable(vectra src/main.cpp
        include/vectra/particle.cpp)

# Add the submodule directory to the build
add_subdirectory(lib/linkit)

# Link against the library from the submodule
target_link_libraries(vectra PRIVATE linkit)