cmake_minimum_required(VERSION 3.16)
project(DuskCatchers LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Download raylib at configure time so nobody has to install it by hand.
# Pinned to a release tag so the build doesn't change underneath us.
include(FetchContent)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) # we only need the library, not raylib's demos
FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG 5.5
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(raylib)

add_executable(dusk_catchers
    src/main.cpp
    src/Game.cpp
    src/Critter.cpp
    src/LightString.cpp
    src/Firefly.cpp
    src/CommonFirefly.cpp
    src/ShyFirefly.cpp
    src/PairFirefly.cpp
    src/GoldenFirefly.cpp
    src/Lantern.cpp
    src/Evening.cpp
    src/Obstacle.cpp
    src/Wind.cpp
    src/Bridge.cpp
)
target_link_libraries(dusk_catchers PRIVATE raylib)

# macOS needs these system frameworks for windows, graphics and input.
if(APPLE)
    target_link_libraries(dusk_catchers PRIVATE
        "-framework IOKit" "-framework Cocoa" "-framework OpenGL")
endif()
