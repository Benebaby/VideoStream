cmake_minimum_required(VERSION 3.7)
project(VideoStream VERSION 1.0.0)


add_definitions(-D_CRT_SECURE_NO_WARNINGS)
add_definitions(-DVK_USE_PLATFORM_WIN32_KHR)
add_compile_definitions(ASSET_PATH="${PROJECT_SOURCE_DIR}/assets/")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

IF(MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++20")
ENDIF()

file(GLOB SOURCES
    src/*.h
    src/*.cpp
)

add_executable(VideoStream ${SOURCES})

set(OpenCV_DIR "C:/Program Files/OpenCV/build_opencv")
find_package(OpenCV REQUIRED)
include_directories( ${OpenCV_INCLUDE_DIRS} )
target_link_libraries( VideoStream ${OpenCV_LIBS} )