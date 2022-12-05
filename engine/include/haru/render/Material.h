//
// Created by andyroiiid on 11/25/2022.
//

#pragma once

#include "haru/render/Texture.h"

struct Material {
    Texture *Diffuse = nullptr;
    Texture *Emissive = nullptr;

    void Bind(Texture &diffuseFallback, Texture &emissiveFallback) const {
        (Diffuse ? *Diffuse : diffuseFallback).Bind(0);
        (Emissive ? *Emissive : emissiveFallback).Bind(1);
    }
};
