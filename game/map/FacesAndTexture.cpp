//
// Created by andyroiiid on 12/2/2022.
//

#include "FacesAndTexture.h"

#include <haru/render/Material.h>

FacesAndTexture::FacesAndTexture(const std::vector<VertexBase> &vertices, const std::string &textureName)
        : m_mesh(vertices, GL_TRIANGLES),
          m_texture(Texture::FromFile("textures/" + textureName + ".png")) {
}

void FacesAndTexture::Draw(Renderer &renderer, const glm::mat4 &modelMatrix) {
    Material material{&m_texture};
    renderer.DrawMesh(m_mesh, modelMatrix, &material);
}
