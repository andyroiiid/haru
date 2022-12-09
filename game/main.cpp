//
// Created by andyroiiid on 11/21/2022.
//

#include <simdjson.h>
#include <haru/core/Debug.h>
#include <haru/system/Window.h>
#include <haru/system/FileSystem.h>

#include "Game.h"

// Use discrete GPU by default.
extern "C" {
// http://developer.download.nvidia.com/devzone/devcenter/gamegraphics/files/OptimusRenderingPolicies.pdf
__declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;

// https://gpuopen.com/learn/amdpowerxpressrequesthighperformance/
__declspec(dllexport) unsigned long AmdPowerXpressRequestHighPerformance = 0x00000001;
}

template<class T>
T UnpackJsonValue(simdjson::ondemand::value &value, const std::string &name) {
    T result;
    simdjson::error_code error = value.get(result);
    DebugCheckCritical(
            error == simdjson::SUCCESS,
            "Failed to get value from JSON pointer path '%s': %s",
            name.c_str(),
            simdjson::error_message(error)
    );
    return result;
}

template<class T>
T FindJsonField(simdjson::ondemand::object &object, const std::string &key) {
    simdjson::ondemand::value value;
    simdjson::error_code error = object.find_field(key).get(value);
    DebugCheckCritical(
            error == simdjson::SUCCESS,
            "JSON field '%s' doesn't exist: %s",
            key.c_str(),
            simdjson::error_message(error)
    );
    return UnpackJsonValue<T>(value, key);
}

class JsonDocument {
public:
    explicit JsonDocument(simdjson::padded_string &&json) : m_json(std::move(json)) {
        simdjson::error_code error = m_parser.iterate(m_json).get(m_document);
        DebugCheckCritical(
                error == simdjson::SUCCESS,
                "Failed to load json document: %s",
                simdjson::error_message(error)
        );
    }

    explicit JsonDocument(const std::string &raw)
            : JsonDocument(simdjson::padded_string(raw)) {}

    template<class T>
    T FindField(const std::string &key) {
        simdjson::ondemand::object object;
        simdjson::error_code error = m_document.get(object);
        DebugCheckCritical(
                error == simdjson::SUCCESS,
                "Json document is not a valid object: %s",
                simdjson::error_message(error)
        );
        return FindJsonField<T>(object, key);
    }

private:
    simdjson::padded_string m_json;

    // Try to reuse this: https://github.com/simdjson/simdjson/blob/master/doc/basics.md#parser-document-and-json-scope
    simdjson::ondemand::parser m_parser;

    simdjson::ondemand::document m_document;
};

int main() {
    FileSystem fileSystem;
    fileSystem.Mount("data", "/");

    JsonDocument gameJson(FileSystem::ReadFile("game.json"));
    auto name = gameJson.FindField<std::string_view>("name");
    auto width = gameJson.FindField<double>("width");
    auto height = gameJson.FindField<double>("height");
    auto maximized = gameJson.FindField<bool>("maximized");
    auto startMap = gameJson.FindField<std::string_view>("startMap");

    Window window{std::string(name), glm::ivec2{width, height}, maximized};

    Game game{std::string(startMap)};

    window.MainLoop(game);
}
