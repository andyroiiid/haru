add_executable(game
        mdl/MdlPalette.h
        mdl/MdlNormals.h
        mdl/MdlStructs.h
        mdl/MdlLoader.cpp mdl/MdlLoader.h

        actors/AWorldLight.cpp actors/AWorldLight.h
        actors/ACamera.cpp actors/ACamera.h
        actors/APlayerNoClip.cpp actors/APlayerNoClip.h
        actors/APlayer.cpp actors/APlayer.h
        actors/APhysBox.cpp actors/APhysBox.h
        actors/ALightPoint.cpp actors/ALightPoint.h
        actors/AWorldSpawn.cpp actors/AWorldSpawn.h
        actors/APropStatic.cpp actors/APropStatic.h
        actors/AFuncBrush.cpp actors/AFuncBrush.h
        actors/AFuncPhys.cpp actors/AFuncPhys.h

        map/Entity.h map/Entity.cpp
        map/FacesAndTexture.cpp map/FacesAndTexture.h
        map/Brushes.cpp map/Brushes.h
        map/ParseMap.cpp map/ParseMap.h
        map/LoadEntities.cpp map/LoadEntities.h

        Actor.h
        Scene.cpp Scene.h
        GameStatics.h
        Game.cpp Game.h
        main.cpp)

target_include_directories(game PRIVATE .)

target_link_libraries(game PRIVATE engine)

set_msvc_runtime_library(game)