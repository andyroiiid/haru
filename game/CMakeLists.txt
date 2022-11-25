add_executable(game
        actors/Actor.h
        actors/ADirectionalLight.cpp actors/ADirectionalLight.h
        actors/ACamera.cpp actors/ACamera.h
        actors/APlayerNoClip.cpp actors/APlayerNoClip.h
        actors/APlayer.cpp actors/APlayer.h
        actors/APhysxBox.cpp actors/APhysxBox.h
        actors/APointLight.cpp actors/APointLight.h
        actors/ALevelGeometry.cpp actors/ALevelGeometry.h

        Scene.cpp Scene.h
        GameStatics.h
        Game.cpp Game.h
        main.cpp)

target_link_libraries(game PRIVATE engine)

set_msvc_runtime_library(game)