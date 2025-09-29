file(GLOB RENDERING_SOURCES "*.cpp")

add_library(rendering STATIC ${RENDERING_SOURCES})
target_include_directories(rendering PUBLIC ${CMAKE_SOURCE_DIR}/include/vectra/rendering)
target_include_directories(rendering
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../../external/glad/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../../include
)