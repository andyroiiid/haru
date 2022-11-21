//
// Created by andyroiiid on 11/20/2022.
//

#pragma once

#include <string>
#include <glm/vec2.hpp>

#include "haru/MoveOnly.h"

class ImageFile {
public:
    MOVE_ONLY(ImageFile)

    explicit ImageFile(const std::string &filename);

    ~ImageFile();

    [[nodiscard]] const glm::ivec2 &Size() const { return m_size; }

    [[nodiscard]] const unsigned char *Data() const { return m_data; }

private:
    MoveOnly<glm::ivec2>      m_size;
    MoveOnly<unsigned char *> m_data;
};