file(GLOB PHYSICS_SOURCES "*.cpp")

add_library(physics STATIC ${PHYSICS_SOURCES}
        colliders/collider_sphere.cpp
        collision_contact.cpp
        collision_data.cpp
        collision_handler.cpp
        colliders/collider_box.cpp
        collider_primitive.cpp)
target_include_directories(physics PUBLIC ${CMAKE_SOURCE_DIR}/include/vectra/physics)
target_include_directories(physics
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../../external/glad/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../../include
)
target_link_libraries(physics PUBLIC linkit)
target_link_libraries(physics PRIVATE glm::glm)
