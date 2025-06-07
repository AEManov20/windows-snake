cmake_minimum_required(VERSION 3.30)
# include(./vcpkg-cmake-integration/vcpkg.cmake)

project(UntitledRenderingFramework)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_FLAGS_DEBUG "-g -O0")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")

link_libraries(GL glfw epoxy dl SDL3)

add_executable(UntitledRenderingFramework src/main.cpp
        src/stb_image_impl.cpp
        src/RenderTarget.h
        src/Window.h
        src/GlfwWindow.cpp
        src/GlfwWindow.h
        src/SdlWindow.cpp
        src/SdlWindow.h
        src/GraphicsShader.h
        src/GlGraphicsShader.cpp
        src/GlGraphicsShader.h
        src/VertexBuffer.h
        src/GlVertexBuffer.cpp
        src/GlVertexBuffer.h
        src/VertexArray.h
        src/GlVertexArray.cpp
        src/GlVertexArray.h
        src/Audio/AudioBuffer.h
        src/Audio/ChannelLayout.h
        src/Audio/SampleConversions.h
        src/Audio/AudioOutput.h
        src/Audio/AudioSource.h
        src/Audio/SampleConversions.cpp
        src/Audio/AudioOutput.cpp
        src/Audio/ChannelLayout.cpp
        src/Audio/AudioSource.cpp
        src/Audio/AudioSourceSine.cpp
)
