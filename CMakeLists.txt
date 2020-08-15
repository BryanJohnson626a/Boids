cmake_minimum_required(VERSION 3.13)

include(cmake/Bootstrap.cmake)
include(cmake/HunterGate.cmake)

# Initialize CMake Hunter
HunterGate(
        URL "https://github.com/ruslo/hunter/archive/v0.23.214.tar.gz"
        SHA1 "e14bc153a7f16d6a5eeec845fb0283c8fad8c358"
)

project(Boids)

hunter_add_package(SDL2)
hunter_add_package(glew)
hunter_add_package(glm)
hunter_add_package(Assimp)
hunter_add_package(soil)
hunter_add_package(imgui)

find_package(SDL2 CONFIG REQUIRED)
find_package(glew CONFIG REQUIRED)
find_package(glm REQUIRED)
find_package(Assimp CONFIG REQUIRED)
find_package(soil CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)

set(PROJECT_SOURCE_FILES
        Source/Boids.cpp
        Source/Boids.h
        Source/GameLoop.cpp
        Source/GameLoop.h
        Source/Engine/ProtoEngine.cpp
        Source/Engine/Types.h
        Source/Engine/Graphics.cpp
        Source/Engine/Graphics.h
        Source/Engine/Mesh.cpp
        Source/Engine/Mesh.h
        Source/Engine/Dice.cpp
        Source/Engine/Dice.h
        Source/Engine/FBO.cpp
        Source/Engine/FBO.h
        Source/Engine/Model.cpp
        Source/Engine/Model.h
        Source/Engine/Transformable.cpp
        Source/Engine/Transformable.h
        Source/Engine/Color.h
        Source/CustomTypes.h
        Source/Engine/UI.cpp
        Source/Engine/UI.h
        Source/Engine/imgui_impl_opengl3.cpp
        Source/Engine/imgui_impl_opengl3.h
        Source/Engine/imgui_impl_sdl.cpp
        Source/Engine/imgui_impl_sdl.h
        Source/GameUI.cpp
        Source/GameUI.h
        )

add_executable(Boids ${PROJECT_SOURCE_FILES})

target_compile_features(Boids PRIVATE
        cxx_std_17
        cxx_auto_type
        cxx_lambdas
        cxx_nullptr
        cxx_range_for
        cxx_variadic_templates
        )

target_link_libraries(Boids PRIVATE
        glew::glew
        SDL2::SDL2
        glm
        Assimp::assimp
        soil::soil
        imgui::imgui
        )

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")
