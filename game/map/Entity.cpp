//
// Created by andyroiiid on 11/30/2022.
//

#include "Entity.h"

const std::string &EntityDefinition::GetProperty(const std::string &key, const std::string &fallback) const {
    auto pair = Properties.find(key);
    if (pair == Properties.end()) {
        return fallback;
    }
    return pair->second;
}
