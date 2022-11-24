add_executable(game
        Actor.h

        ADirectionalLight.cpp ADirectionalLight.h
        AFlyCamera.cpp AFlyCamera.h
        APhysxBox.cpp APhysxBox.h
        APointLight.cpp APointLight.h
        ALevelGeom.cpp ALevelGeom.h

        Scene.cpp Scene.h
        Game.cpp Game.h
        main.cpp)

target_link_libraries(game PRIVATE engine)

set_msvc_runtime_library(game)