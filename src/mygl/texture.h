#pragma once

#include "base.h"

struct Texture
{
    GLuint id = 0;

    unsigned int width = 0;
    unsigned int height = 0;
};

Texture textureLoad(const std::string& path);
void textureDelete(const Texture& texture);
