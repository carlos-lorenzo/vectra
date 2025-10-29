file(GLOB PHYSICS_SOURCES "*.cpp")

add_library(physics STATIC ${PHYSICS_SOURCES})
target_include_directories(physics PUBLIC ${CMAKE_SOURCE_DIR}/include/vectra/physics)
target_include_directories(physics
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../../external/glad/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../../include
)
target_link_libraries(physics PUBLIC linkit)
target_link_libraries(rendering PRIVATE glm)