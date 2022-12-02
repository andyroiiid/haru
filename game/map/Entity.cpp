//
// Created by andyroiiid on 11/30/2022.
//

#include "map/Entity.h"

#include <sstream>

const std::string &EntityDefinition::GetProperty(const std::string &key, const std::string &fallback) const {
    auto pair = Properties.find(key);
    if (pair == Properties.end()) {
        return fallback;
    }
    return pair->second;
}

bool EntityDefinition::GetPropertyInteger(const std::string &key, int &value) const {
    const std::string &literal = GetProperty(key);
    if (literal.empty()) return false;

    std::stringstream literalStream(literal);
    int x;
    literalStream >> x;
    if (literalStream.bad()) return false;

    value = x;
    return true;
}

bool EntityDefinition::GetPropertyVec3(const std::string &key, glm::vec3 &value) const {
    const std::string &literal = GetProperty(key);
    if (literal.empty()) return false;

    std::stringstream literalStream(literal);
    float x, y, z;
    literalStream >> x >> y >> z;
    if (literalStream.bad()) return false;

    value = {x, y, z};
    return true;
}
