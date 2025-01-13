cmake_minimum_required(VERSION 3.12)

project(fogRendering)

set(CMAKE_CXX_STANDARD 17)

add_definitions (-DDEBUG)

file(GLOB ASSETS "./assets")
file(COPY ${ASSETS} DESTINATION ${CMAKE_BINARY_DIR})


include_directories(
	SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}/thirdParty/include
)

link_directories(
	SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}/thirdParty/lib
)

add_subdirectory(wrapper)
add_subdirectory(application)
add_subdirectory(glframework)

add_executable(fogRendering "main.cpp" "glad.c")

target_link_libraries(fogRendering glfw3.lib assimp-vc143-mtd.lib zlibstaticd.lib wrapper app fw)