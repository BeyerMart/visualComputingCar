#pragma once

#include "mesh.h"

#include <vector>

/* cube geometry */
namespace cube
{
inline static const std::vector<Vector3D> vertexPos =
{
    {-1.0f, -1.0f, 1.0f}, 
    {-1.0f,  1.0f, 1.0f}, 
    { 1.0f,  1.0f, 1.0f}, 
    { 1.0f, -1.0f, 1.0f}, 

    {-1.0f, -1.0f, -1.0f},
    {-1.0f,  1.0f, -1.0f},
    { 1.0f,  1.0f, -1.0f},
    { 1.0f, -1.0f, -1.0f}
};
inline static const std::vector<unsigned int> indices =
{
    0, 1, 2,
    2, 3, 0,

    4, 5, 6,
    6, 7, 4,

    0, 1, 5,
    5, 4, 0,

    3, 2, 6,
    6, 7, 3,

    1, 5, 6,
    6, 2, 1,

    0, 4, 7,
    7, 3, 0
};
inline static const std::vector<Vertex> vertices = 
{
    {{-1.0, -1.0, 1.0}, {1.0, 0.0, 0.0, 1.0}},
    {{-1.0,  1.0, 1.0}, {0.0, 1.0, 0.0, 1.0}},
    {{ 1.0,  1.0, 1.0}, {0.0, 0.0, 1.0, 1.0}},
    {{ 1.0, -1.0, 1.0}, {1.0, 0.0, 1.0, 1.0}},

    {{-1.0, -1.0, -1.0}, {1.0, 0.0, 0.0, 1.0}},
    {{-1.0,  1.0, -1.0}, {0.0, 1.0, 0.0, 1.0}},
    {{ 1.0,  1.0, -1.0}, {0.0, 0.0, 1.0, 1.0}},
    {{ 1.0, -1.0, -1.0}, {1.0, 0.0, 1.0, 1.0}}
};
}

/* plane geometry */
namespace quad
{
inline static const std::vector<Vector3D> vertexPos =
{
    {-1.0f, 0.0f, -1.0f},
    {-1.0f, 0.0f,  1.0f},
    { 1.0f, 0.0f,  1.0f},
    { 1.0f, 0.0f, -1.0f}
};
inline static const std::vector<unsigned int> indices =
{
    0, 1, 2,
    2, 3, 0
};
}
