cmake_minimum_required(VERSION 3.15)
project(Wolfenstein3D)

set(CMAKE_CXX_STANDARD 14)

# Specify the directories for source files and headers
set(SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)
set(HEADER_DIR ${CMAKE_SOURCE_DIR}/include)

# Find required packages
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

# Include directories
include_directories(${HEADER_DIR} ${OPENGL_INCLUDE_DIRS} ${GLUT_INCLUDE_DIRS})

# List of source files
file(GLOB SOURCES "${SOURCE_DIR}/*.cpp")

# Define the executable target
add_executable(Wolfenstein3D ${SOURCES})

# Link against the required libraries
target_link_libraries(Wolfenstein3D "-framework GLUT" "-framework OpenGL")
