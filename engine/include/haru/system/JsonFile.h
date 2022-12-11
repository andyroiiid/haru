//
// Created by andyroiiid on 12/11/2022.
//

#include <simdjson.h>

#include "haru/core/Debug.h"
#include "haru/system/FileSystem.h"

template<class T>
T UnpackJsonValue(simdjson::ondemand::value &value, const std::string &name) {
    T result;
    simdjson::error_code error = value.get(result);
    DebugCheckCritical(
            error == simdjson::SUCCESS,
            "Failed to unpack JSON value '%s': %s",
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

template<>
std::string FindJsonField(simdjson::ondemand::object &object, const std::string &key) {
    return std::string{FindJsonField<std::string_view>(object, key)};
}

class JsonFile {
public:
    explicit JsonFile(const std::string &filename) : m_json(FileSystem::ReadFile(filename)) {
        simdjson::error_code error = m_parser.iterate(m_json).get(m_document);
        DebugCheckCritical(
                error == simdjson::SUCCESS,
                "Failed to load json document: %s",
                simdjson::error_message(error)
        );
    }

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
