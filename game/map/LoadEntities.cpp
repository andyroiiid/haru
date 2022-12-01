//
// Created by andyroiiid on 11/30/2022.
//

#include "LoadEntities.h"

#include <haru/core/Debug.h>

#include "../GameStatics.h"
#include "../Scene.h"
#include "../actors/ALevelGeometry.h"
#include "../actors/AStaticModel.h"

void LoadWorldSpawn(const EntityDefinition &definition) {
    GameStatics::GetScene()->CreateActor<ALevelGeometry>(definition.Brushes);
}

void LoadInfoPlayerStart(const EntityDefinition &definition) {
}

void LoadPropStatic(const EntityDefinition &definition) {
    const std::string &model = definition.GetProperty("model");
    DebugCheckCritical(!model.empty(), "prop_static doesn't have a valid model!");

    glm::vec3 origin;
    DebugCheckCritical(definition.GetPropertyPosition("origin", origin), "prop_static doesn't have a valid origin!");

    GameStatics::GetScene()->CreateActor<AStaticModel>(model, origin);
}

typedef void (*EntityLoader)(const EntityDefinition &definition);

static const std::map<std::string, EntityLoader> s_EntityLoaders{
        {"worldspawn",        LoadWorldSpawn},
        {"info_player_start", LoadInfoPlayerStart},
        {"prop_static",       LoadPropStatic}
};

void LoadEntities(const std::vector<EntityDefinition> &entities) {
    for (auto &entity: entities) {
        const std::string &className = entity.GetProperty("classname");
        auto loader = s_EntityLoaders.find(className);
        DebugCheckCritical(loader != s_EntityLoaders.end(), "Unknown entity type %s", className.c_str());
        loader->second(entity);
    }
}
