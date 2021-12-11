#pragma once

#include "base.h"

struct ShaderProgram
{
    GLuint id = 0;
    GLuint _vertexID = 0;
    GLuint _fragmentID = 0;
};

ShaderProgram shaderLoad(const std::string& vertexPath, const std::string& fragmentPath);
ShaderProgram shaderCreate(const std::string& vertexSource, const std::string& fragmentSource);
void shaderDelete(const ShaderProgram& program);

void shaderUniform(ShaderProgram& shader, const std::string& name, const Matrix4D& value);
void shaderUniform(ShaderProgram& shader, const std::string& name, const Vector3D& vec);
void shaderUniform(ShaderProgram& shader, const std::string& name, const Vector4D& vec);
void shaderUniform(ShaderProgram& shader, const std::string& name, int value);
void shaderUniform(ShaderProgram& shader, const std::string& name, float value);
