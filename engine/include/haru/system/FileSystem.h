//
// Created by andyroiiid on 11/20/2022.
//

#pragma once

#include <string>

#include "haru/core/MoveOnly.h"

class FileSystem {
public:
    NO_MOVE_OR_COPY(FileSystem)

    FileSystem();

    ~FileSystem();

    FileSystem &Mount(const std::string &newDir, const std::string &mountPoint, bool appendToPath = true);

    static std::string ReadFile(const std::string &filename);
};
