#include <Engine/PCH/pch.h>
#include "shader.h"
#include <Engine/ResourceManager/resourceManager.h>

namespace Dog {

    float Shader::iTime = 0.0f;
    GLuint Shader::uboMatrices = 0;
    GLuint Shader::uboTime = 0;
    GLuint Shader::uboMatricesBindingPoint = 0;
    GLuint Shader::uboTimeBindingPoint = 1;

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
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
    }
    void Shader::SetUnsigned(const std::string& name, unsigned int value)
    {
        this->Use();
        glUniform1ui(glGetUniformLocation(ID, name.c_str()), value);
    }
    void Shader::SetInteger(const std::string& name, int value)
    {
        this->Use();
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
    }
    void Shader::SetVector2f(const std::string& name, float x, float y)
    {
        this->Use();
        glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
    }
    void Shader::SetVector2f(const std::string& name, const glm::vec2& value)
    {
        this->Use();
        glUniform2f(glGetUniformLocation(ID, name.c_str()), value.x, value.y);
    }
    void Shader::SetVector3f(const std::string& name, float x, float y, float z)
    {
        this->Use();
        glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
    }
    void Shader::SetVector3f(const std::string& name, const glm::vec3& value)
    {
        this->Use();
        glUniform3f(glGetUniformLocation(ID, name.c_str()), value.x, value.y, value.z);
    }
    void Shader::SetVector4f(const std::string& name, float x, float y, float z, float w)
    {
        this->Use();
        glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
    }
    void Shader::SetVector4f(const std::string& name, const glm::vec4& value)
    {
        this->Use();
        glUniform4f(glGetUniformLocation(ID, name.c_str()), value.x, value.y, value.z, value.w);
    }
    void Shader::SetMatrix4(const std::string& name, const glm::mat4& matrix)
    {
        this->Use();
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, false, glm::value_ptr(matrix));
    }

    void Shader::SetUniformHandle(const std::string& name, GLuint64 handle) {
        glUniformHandleui64ARB(glGetUniformLocation(ID, name.c_str()), handle);
    }

    bool Shader::HasUniform(const std::string& name)
    {
        return Uniforms.find(name) != Uniforms.end();
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

    void Shader::SetupUBO()
    {
        glGenBuffers(1, &uboMatrices);
        glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
        glBufferData(GL_UNIFORM_BUFFER, 3 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
        glBindBufferRange(GL_UNIFORM_BUFFER, 0, uboMatrices, 0, 3 * sizeof(glm::mat4));

        glGenBuffers(1, &uboTime);
        glBindBuffer(GL_UNIFORM_BUFFER, uboTime);
        glBufferData(GL_UNIFORM_BUFFER, 3 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
        glBindBufferRange(GL_UNIFORM_BUFFER, 1, uboTime, 0, sizeof(float));
    }

    void Shader::SetProjectionUBO(glm::mat4& projection)
    {
        glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(projection));
    }

    void Shader::SetViewUBO(glm::mat4& view)
    {
        glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
        glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view));
    }

    void Shader::SetProjectionViewUBO(glm::mat4& projectionView)
    {
        glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
        glBufferSubData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(projectionView));
    }

    void Shader::SetViewAndProjectionView(glm::mat4& view, glm::mat4& projectionView)
    {
        glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
        glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view));
        glBufferSubData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(projectionView));
    }

    void Shader::SetTimeUBO(float time) {
        glBindBuffer(GL_UNIFORM_BUFFER, uboTime);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(float), &time);
    }

    void Shader::BindUBO(const std::string& blockName, unsigned int bindingPoint) {
        unsigned int blockIndex = glGetUniformBlockIndex(ID, blockName.c_str());
        if (blockIndex != GL_INVALID_INDEX) {
            glUniformBlockBinding(ID, blockIndex, bindingPoint);
        }
    }

}