file(GLOB RENDERING_SOURCES "*.cpp")

add_library(rendering STATIC ${RENDERING_SOURCES}
        engine_ui.cpp)
target_include_directories(rendering PUBLIC ${CMAKE_SOURCE_DIR}/include/vectra/rendering)
target_include_directories(rendering
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../../external/glad/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../../include
)

# Link dependencies - use PUBLIC for headers exposed in rendering's public headers
target_link_libraries(rendering
    PUBLIC
        linkit
        glm::glm
        imgui
        assimp::assimp
        stb_image
        glfw
)
