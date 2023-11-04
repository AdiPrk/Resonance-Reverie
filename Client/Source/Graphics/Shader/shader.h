#pragma once

#include <PCH/pch.h>

// General purpose shader object. Compiles from file, generates
// compile/link-time error messages and hosts several utility 
// functions for easy management.
class Shader
{
public:
    // state
    unsigned int ID;
    // constructor
    Shader() : ID() {}
    // sets the current shader as active
    Shader& Use();
    // compiles the shader from given source code
    void    Compile(const char* vertexSource, const char* fragmentSource); // note: geometry source code is optional 
    void    SetUniformsFromCode();
    // utility functions
    void    SetFloat(const std::string& name, float value);
    void    SetInteger(const std::string& name, int value);
    void    SetVector2f(const std::string& name, float x, float y);
    void    SetVector2f(const std::string& name, const glm::vec2& value);
    void    SetVector3f(const std::string& name, float x, float y, float z);
    void    SetVector3f(const std::string& name, const glm::vec3& value);
    void    SetVector4f(const std::string& name, float x, float y, float z, float w);
    void    SetVector4f(const std::string& name, const glm::vec4& value);
    void    SetMatrix4(const std::string& name, const glm::mat4& matrix);
    void    SetUniformHandle(const std::string& name, GLuint64 handle);
    bool    HasUniform(const std::string& name);

    // a global variable across all shaders
    static float iTime;

    // uniforms
    std::map<std::string, GLuint> Uniforms;

private:
    // checks if compilation or linking failed and if so, print the error logs
    void    checkCompileErrors(unsigned int object, std::string type);
};