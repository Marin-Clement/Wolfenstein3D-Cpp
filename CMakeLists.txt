cmake_minimum_required(VERSION 3.5)

project(WolfensteinGL LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Locate GLEW
set(GLEW_INCLUDE_DIRS /opt/homebrew/Cellar/glew/2.2.0_1/include)
set(GLEW_LIBRARIES /opt/homebrew/Cellar/glew/2.2.0_1/lib/libGLEW.dylib)

# Locate GLFW3
set(GLFW_INCLUDE_DIRS /opt/homebrew/Cellar/glfw/3.4/include)
set(GLFW_LIBRARIES /opt/homebrew/Cellar/glfw/3.4/lib/libglfw.dylib)

# Locate glm
set(GLM_INCLUDE_DIRS /opt/homebrew/Cellar/glm/1.0.1/include)
set(GLM_LIBRARIES /opt/homebrew/Cellar/glm/1.0.1/lib/libglm.dylib)

find_package(OpenGL REQUIRED)

include_directories(${OPENGL_INCLUDE_DIR})
include_directories(${GLEW_INCLUDE_DIRS})
include_directories(${GLFW_INCLUDE_DIRS})
include_directories(${GLM_INCLUDE_DIRS})
include_directories(${STB_INCLUDE_DIRS})

add_executable(WolfensteinGL main.cpp
        stb_image.h
        shaderUtils.cpp)
target_link_libraries(WolfensteinGL ${OPENGL_gl_LIBRARY} ${GLFW_LIBRARIES} ${GLEW_LIBRARIES})