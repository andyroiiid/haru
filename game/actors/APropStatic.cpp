//
// Created by andyroiiid on 11/26/2022.
//

#include "actors/APropStatic.h"

#include <haru/render/Material.h>

#include "mdl/MdlLoader.h"

APropStatic::APropStatic(const std::string &model, const glm::vec3 &origin) {
    MdlLoader loader(model);
    m_texture = loader.GetTexture();
    m_mesh = loader.GetMesh();
    GetTransform().SetPosition(origin);
}

void APropStatic::Draw(Renderer &renderer) {
    Material material{&m_texture};
    renderer.DrawMesh(m_mesh, GetTransform().GetMatrix(), &material);
}
