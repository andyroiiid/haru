add_executable(game
        actors/Actor.h
        actors/ADirectionalLight.cpp actors/ADirectionalLight.h
        actors/AFlyCamera.cpp actors/AFlyCamera.h
        actors/APhysxBox.cpp actors/APhysxBox.h
        actors/APointLight.cpp actors/APointLight.h
        actors/ALevelGeom.cpp actors/ALevelGeom.h

        Scene.cpp Scene.h
        Game.cpp Game.h
        main.cpp)

target_link_libraries(game PRIVATE engine)

set_msvc_runtime_library(game)