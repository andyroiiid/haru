//
// Created by andyroiiid on 11/26/2022.
//

#include "AStaticModel.h"

#include <haru/render/Material.h>

#include "MdlLoader.h"

AStaticModel::AStaticModel() {
    MdlLoader loader("data/models/desk.mdl");
    m_texture = loader.GetTexture();
    m_mesh = loader.GetMesh();
    GetTransform().SetPosition({0.0f, 1.0f, 2.0f});
}

void AStaticModel::Draw(Renderer &renderer) {
    Material material{&m_texture};
    renderer.DrawMesh(m_mesh, GetTransform().GetMatrix(), &material);
}
