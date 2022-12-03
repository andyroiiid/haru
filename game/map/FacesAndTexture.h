//
// Created by andyroiiid on 12/2/2022.
//

#pragma once

#include <string>
#include <haru/core/MoveOnly.h>
#include <haru/render/MeshBase.h>
#include <haru/render/Renderer.h>

class Texture;

class FacesAndTexture {
public:
    MOVE_ONLY(FacesAndTexture)

    FacesAndTexture(const std::vector<VertexBase> &vertices, const std::string &textureName);

    void Draw(Renderer &renderer, const glm::mat4 &modelMatrix);

private:
    MeshBase m_mesh;
    MoveOnly<Texture *> m_texture;
};
