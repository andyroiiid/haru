//
// Created by andyroiiid on 11/30/2022.
//

#include "map/LoadEntities.h"

#include <haru/core/Debug.h>
#include <tracy/Tracy.hpp>

#include "GameStatics.h"
#include "Scene.h"
#include "actors/AWorldSpawn.h"
#include "actors/ALightPoint.h"
#include "actors/APropStatic.h"

void LoadWorldSpawn(const EntityDefinition &definition) {
    GameStatics::GetScene()->CreateActor<AWorldSpawn>(definition.Brushes);
}

void LoadInfoPlayerStart(const EntityDefinition &definition) {
}

void LoadLightPoint(const EntityDefinition &definition) {
    glm::vec3 origin;
    DebugCheckCritical(definition.GetPropertyPosition("origin", origin), "light_point doesn't have a valid origin!");

    glm::vec3 color{1.0f, 1.0f, 1.0f};
    (void) definition.GetPropertyColor("color", color);

    int range = 20;
    (void) definition.GetPropertyInteger("range", range);

    GameStatics::GetScene()->CreateActor<ALightPoint>(origin, color, static_cast<float>(range));
}

void LoadPropStatic(const EntityDefinition &definition) {
    const std::string &model = definition.GetProperty("model");
    DebugCheckCritical(!model.empty(), "prop_static doesn't have a valid model!");

    glm::vec3 origin;
    DebugCheckCritical(definition.GetPropertyPosition("origin", origin), "prop_static doesn't have a valid origin!");

    GameStatics::GetScene()->CreateActor<APropStatic>(model, origin);
}

typedef void (*EntityLoader)(const EntityDefinition &definition);

static const std::map<std::string, EntityLoader> s_EntityLoaders{
        {"worldspawn",        LoadWorldSpawn},
        {"info_player_start", LoadInfoPlayerStart},
        {"light_point",       LoadLightPoint},
        {"prop_static",       LoadPropStatic}
};

void LoadEntities(const std::vector<EntityDefinition> &entities) {
    ZoneScoped;

    for (auto &entity: entities) {
        const std::string &className = entity.GetProperty("classname");
        auto loader = s_EntityLoaders.find(className);
        DebugCheckCritical(loader != s_EntityLoaders.end(), "Unknown entity type %s", className.c_str());
        loader->second(entity);
    }
}
