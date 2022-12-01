//
// Created by andyroiiid on 11/26/2022.
//

#include "AStaticModel.h"

#include <haru/render/Material.h>

#include "../mdl/MdlLoader.h"

AStaticModel::AStaticModel(const std::string &model, const glm::vec3 &origin) {
    MdlLoader loader("data/" + model);
    m_texture = loader.GetTexture();
    m_mesh = loader.GetMesh();
    GetTransform().SetPosition(origin);
}

void AStaticModel::Draw(Renderer &renderer) {
    Material material{&m_texture};
    renderer.DrawMesh(m_mesh, GetTransform().GetMatrix(), &material);
}
