cmake_minimum_required(VERSION 3.30)
project(UntitledRenderingFramework)

set(CMAKE_CXX_STANDARD 20)

#link_libraries(opengl32 glfw3 epoxy SDL3)
link_libraries(GL glfw epoxy dl SDL3)

add_executable(UntitledRenderingFramework main.cpp
        stb_image_impl.cpp
        RenderTarget.h
        Window.h
        GlfwWindow.cpp
        GlfwWindow.h
        SdlWindow.cpp
        SdlWindow.h
        GraphicsShader.h
        GlGraphicsShader.cpp
        GlGraphicsShader.h
        VertexBuffer.h
        GlVertexBuffer.cpp
        GlVertexBuffer.h
        VertexArray.h
        GlVertexArray.cpp
        GlVertexArray.h
)
