add_executable(game
        mdl/MdlPalette.h
        mdl/MdlNormals.h
        mdl/MdlStructs.h
        mdl/MdlLoader.cpp mdl/MdlLoader.h

        actors/Actor.h
        actors/ADirectionalLight.cpp actors/ADirectionalLight.h
        actors/ACamera.cpp actors/ACamera.h
        actors/APlayerNoClip.cpp actors/APlayerNoClip.h
        actors/APlayer.cpp actors/APlayer.h
        actors/APhysBox.cpp actors/APhysBox.h
        actors/APointLight.cpp actors/APointLight.h
        actors/ALevelGeometry.cpp actors/ALevelGeometry.h
        actors/AStaticModel.cpp actors/AStaticModel.h

        map/Entity.h map/Entity.cpp
        map/ParseMap.cpp map/ParseMap.h
        map/LoadEntities.cpp map/LoadEntities.h

        Scene.cpp Scene.h
        GameStatics.h
        Game.cpp Game.h
        main.cpp)

target_link_libraries(game PRIVATE engine)

set_msvc_runtime_library(game)