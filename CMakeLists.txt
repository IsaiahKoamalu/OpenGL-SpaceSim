cmake_minimum_required(VERSION 3.28)
project(SpaceSimulation VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG 3.4
)
FetchContent_MakeAvailable(glfw)

# GLM (header-only)
FetchContent_Declare(
        glm
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        GIT_TAG 1.0.1
)
FetchContent_MakeAvailable(glm)

find_package(OpenGL REQUIRED)

add_library(engine
        src/main.cpp
        include/Rendering/Shader.h

)

add_executable(SpaceSimulation
        src/main.cpp
        include/Rendering/Shader.h
        src/Shader.cpp
        include/Components/CameraComponent.h
        include/Rendering/Mesh.h
        src/Rendering/Mesh.cpp
        src/Systems/RenderSystem.cpp
        src/Systems/InputSystem.cpp
        src/Systems/IntegrateSystem.cpp
        src/Systems/CameraFollowSystem.cpp
        src/Systems/PointLightSystem.cpp
        third_party/glad/src/glad.c
)

target_include_directories(SpaceSimulation
        PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        third_party/glad/include
)

target_link_libraries(SpaceSimulation
        PRIVATE
        glfw
        glm::glm
        OpenGL::GL
)

target_compile_definitions(SpaceSimulation PRIVATE PROJECT_SOURCE_DIR="${CMAKE_SOURCE_DIR}")

if (MSVC)
    target_compile_options(SpaceSimulation PRIVATE /W4 /permissive-)
else()
    target_compile_options(SpaceSimulation PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Copy shaders next to the executable every build
add_custom_target(copy_shaders ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/shaders
        ${CMAKE_CURRENT_BINARY_DIR}/shaders)

add_dependencies(SpaceSimulation copy_shaders)