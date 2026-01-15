file(GLOB CORE_SOURCES "*.cpp")

add_library(core STATIC ${CORE_SOURCES})
target_include_directories(core PUBLIC ${CMAKE_SOURCE_DIR}/include/vectra/core)
target_include_directories(core
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../../external/glad/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../../external/linkit/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../../include
)

# Link dependencies - use PUBLIC for headers that are exposed in core's public headers
target_link_libraries(core
    PUBLIC
        linkit
        glm::glm
        assimp::assimp
        glfw
        stb_image
        nlohmann_json::nlohmann_json
)
