#include <PCH/pch.h>
#include "shader.h"
#include <Source/Graphics/ResourceManager/resourceManager.h>

float Shader::iTime = 0.0f;

Shader& Shader::Use()
{
    if (this->ID != ResourceManager::CurrentShaderProgram) {
        glUseProgram(this->ID);
        ResourceManager::CurrentShaderProgram = this->ID;
    }
    
    return *this;
}

void Shader::Compile(const char* vertexSource, const char* fragmentSource)
{
    unsigned int sVertex, sFragment;

    // vertex Shader
    sVertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(sVertex, 1, &vertexSource, NULL);
    glCompileShader(sVertex);
    checkCompileErrors(sVertex, "VERTEX");

    // fragment Shader
    sFragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(sFragment, 1, &fragmentSource, NULL);
    glCompileShader(sFragment);
    checkCompileErrors(sFragment, "FRAGMENT");

    // shader program
    this->ID = glCreateProgram();
    glAttachShader(this->ID, sVertex);
    glAttachShader(this->ID, sFragment);
    glLinkProgram(this->ID);
    checkCompileErrors(this->ID, "PROGRAM");

    // delete the shaders as they're linked into our program now and no longer necessary
    glDeleteShader(sVertex);
    glDeleteShader(sFragment);
}

void Shader::SetUniformsFromCode()
{
    GLint numUniforms = 0;
    glGetProgramiv(ID, GL_ACTIVE_UNIFORMS, &numUniforms);

    GLchar uniformName[256]; // Adjust size as needed
    for (GLint i = 0; i < numUniforms; ++i) {
        GLint arraySize = 0;
        GLenum type = 0;
        GLsizei actualLength = 0;
        glGetActiveUniform(ID, i, sizeof(uniformName), &actualLength, &arraySize, &type, uniformName);

        GLint location = glGetUniformLocation(ID, uniformName);
        if (location != -1) { // -1 indicates that the uniform is not active
            std::string name(uniformName, actualLength); // Construct a string with the actual name length
            Uniforms[name] = location;
        }
    }
}

void Shader::SetFloat(const std::string& name, float value)
{
        this->Use();
    glUniform1f(Uniforms[name], value);
}
void Shader::SetInteger(const std::string& name, int value)
{
    this->Use();
    glUniform1i(Uniforms[name], value);
}
void Shader::SetVector2f(const std::string& name, float x, float y)
{
    this->Use();
    glUniform2f(Uniforms[name], x, y);
}
void Shader::SetVector2f(const std::string& name, const glm::vec2& value)
{
    this->Use();
    glUniform2f(Uniforms[name], value.x, value.y);
}
void Shader::SetVector3f(const std::string& name, float x, float y, float z)
{
    this->Use();
    glUniform3f(Uniforms[name], x, y, z);
}
void Shader::SetVector3f(const std::string& name, const glm::vec3& value)
{
    this->Use();
    glUniform3f(Uniforms[name], value.x, value.y, value.z);
}
void Shader::SetVector4f(const std::string& name, float x, float y, float z, float w)
{
    this->Use();
    glUniform4f(Uniforms[name], x, y, z, w);
}
void Shader::SetVector4f(const std::string& name, const glm::vec4& value)
{
    this->Use();
    glUniform4f(Uniforms[name], value.x, value.y, value.z, value.w);
}
void Shader::SetMatrix4(const std::string& name, const glm::mat4& matrix)
{
    this->Use();
    glUniformMatrix4fv(Uniforms[name], 1, false, glm::value_ptr(matrix));
}

bool Shader::HasUniform(const std::string& name)
{
    return Uniforms.find(name) != Uniforms.end();
}


void Shader::SetUniformHandle(const std::string& name, GLuint64 handle) {
    glUniformHandleui64ARB(Uniforms[name], handle);
}

void Shader::checkCompileErrors(unsigned int object, std::string type)
{
    int success;
    char infoLog[1024];
    if (type != "PROGRAM")
    {
        glGetShaderiv(object, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(object, 1024, NULL, infoLog);
            std::cout << "| ERROR::SHADER: Compile-time error: Type: " << type << "\n"
                << infoLog << "\n -- --------------------------------------------------- -- "
                << std::endl;
        }
    }
    else
    {
        glGetProgramiv(object, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(object, 1024, NULL, infoLog);
            std::cout << "| ERROR::Shader: Link-time error: Type: " << type << "\n"
                << infoLog << "\n -- --------------------------------------------------- -- "
                << std::endl;
        }
    }
}