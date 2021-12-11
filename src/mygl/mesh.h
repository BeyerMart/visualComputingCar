#pragma once

#include "base.h"

#include <vector>

enum eDataIdx { Position = 0, Normal = 1, UV = 2 };

struct Vertex
{
    Vector3D pos;
    Vector3D normal;
    Vector2D uv;
};


struct Mesh
{
    GLuint vao = 0;
    GLuint vbo = 0;
    GLuint ebo = 0;

    unsigned int size_vbo = 0;
    unsigned int size_ibo = 0;
};


Mesh meshCreate(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
void meshDelete(const Mesh& mesh);
