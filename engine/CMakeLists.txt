add_subdirectory(dependencies EXCLUDE_FROM_ALL)

add_library(engine
        include/haru/MoveOnly.h
        include/haru/Debug.h
        include/haru/Window.h
        include/haru/Files.h
        include/haru/Framebuffer.h
        include/haru/UniformBuffer.h
        include/haru/Mesh.h
        include/haru/MeshBase.h
        include/haru/MeshUtilities.h
        include/haru/Shader.h
        include/haru/ImageFile.h
        include/haru/Texture.h
        include/haru/Renderer.h
        include/haru/DeferredRenderer.h

        src/Window.cpp
        src/Files.cpp
        src/Framebuffer.cpp
        src/MeshUtilities.cpp
        src/Shader.cpp
        src/ImageFile.cpp
        src/Texture.cpp
        src/DeferredRenderer.cpp)

target_include_directories(engine PUBLIC include)

target_compile_definitions(engine PUBLIC GLFW_INCLUDE_NONE)

target_link_libraries(engine PUBLIC glfw glad glm stb)