#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define _USE_MATH_DEFINES
#include <math.h>

#include <string>

#include "math/vector2d.h"
#include "math/vector3d.h"
#include "math/vector4d.h"
#include "math/matrix3d.h"
#include "math/matrix4d.h"


GLFWwindow* windowCreate(const std::string &title, unsigned int width, unsigned int height);
void windowDelete(GLFWwindow* window);

void screenshotToPNG(const std::string &filepath);

GLenum glCheckError_(const char *file, int line);
#define glCheckError() glCheckError_(__FILE__, __LINE__)
