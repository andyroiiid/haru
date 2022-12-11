add_subdirectory(dependencies EXCLUDE_FROM_ALL)

add_library(engine
        include/haru/core/MoveOnly.h
        include/haru/core/Debug.h

        include/haru/math/Random.h
        include/haru/math/RandomGeom.h
        include/haru/math/Transform.h

        include/haru/system/Window.h
        include/haru/system/FileSystem.h
        include/haru/system/ImageFile.h
        include/haru/system/JsonFile.h
        src/system/Window.cpp
        src/system/FileSystem.cpp
        src/system/ImageFile.cpp

        include/haru/render/Framebuffer.h
        include/haru/render/ShadowMap.h
        include/haru/render/ShadowMatrixCalculator.h
        include/haru/render/UniformBuffer.h
        include/haru/render/Mesh.h
        include/haru/render/MeshPositionOnly.h
        include/haru/render/MeshPositionColor.h
        include/haru/render/MeshBase.h
        include/haru/render/MeshUtilities.h
        include/haru/render/Shader.h
        include/haru/render/Texture.h
        include/haru/render/Material.h
        include/haru/render/Renderer.h
        include/haru/render/DeferredShaders.h
        include/haru/render/DeferredRenderer.h
        src/render/Framebuffer.cpp
        src/render/ShadowMap.cpp
        src/render/ShadowMatrixCalculator.cpp
        src/render/MeshUtilities.cpp
        src/render/Shader.cpp
        src/render/Texture.cpp
        src/render/Renderer.cpp
        src/render/DeferredShaders.cpp
        src/render/DeferredRenderer.cpp

        include/haru/physics/PhysicsLayer.h
        include/haru/physics/PhysicsScene.h
        include/haru/physics/PhysicsSystem.h
        src/physics/PhysicsLayer.cpp
        src/physics/PhysicsScene.cpp
        src/physics/PhysicsSystem.cpp)

target_include_directories(engine PUBLIC include)

target_compile_definitions(engine PUBLIC GLFW_INCLUDE_NONE)

target_link_libraries(engine PUBLIC glfw glad glm stb PhysX physfs-static simdjson TracyClient)

set_msvc_runtime_library(engine)
