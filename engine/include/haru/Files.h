//
// Created by andyroiiid on 11/20/2022.
//

#pragma once

#include <string>
#include <filesystem>

std::string ReadFile(const std::string &filename);

// IterateDirectory(directory, [](const auto &entry) {
//     Do something...
// });
template<class Func>
void IterateDirectory(const std::string &directory, Func &&func) {
    using namespace std::filesystem;
    for (const auto &entry: directory_iterator(path(directory))) {
        func(entry);
    }
}