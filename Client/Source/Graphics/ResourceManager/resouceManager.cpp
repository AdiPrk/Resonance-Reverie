#include <PCH/pch.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#include "resourceManager.h"

// Instantiate static variables
std::map<std::string, Texture2D>    ResourceManager::Textures;
std::map<std::string, Shader>       ResourceManager::Shaders;

GLuint ResourceManager::CurrentShaderProgram = 0;
GLuint ResourceManager::CurrentTextureID = 0;
GLuint ResourceManager::CurrentVAO = 0;

Shader& ResourceManager::LoadShader(const char* vShaderFile, const char* fShaderFile, const char* gShaderFile, std::string name)
{
    Shaders[name] = loadShaderFromFile(vShaderFile, fShaderFile, gShaderFile);
    return Shaders[name];
}

Shader& ResourceManager::GetShader(std::string name)
{
    return Shaders[name];
}

Texture2D& ResourceManager::LoadTexture(const char* file, std::string name)
{
    Textures[name] = loadTextureFromFile(file);
    return Textures[name];
}

Texture2D& ResourceManager::GetTexture(std::string name)
{
    return Textures[name];
}

void ResourceManager::Clear()
{
    // (properly) delete all shaders	
    for (const auto& iter : Shaders)
        glDeleteProgram(iter.second.ID);

    // (properly) delete all textures
    for (const auto& iter : Textures)
        glDeleteTextures(1, &iter.second.ID);
}

void ResourceManager::BindVAO(GLuint quadVAO)
{
    if (CurrentVAO != quadVAO) {
        glBindVertexArray(quadVAO);
        CurrentVAO = quadVAO;
    }
}

Shader ResourceManager::loadShaderFromFile(const char* vShaderFile, const char* fShaderFile, const char* gShaderFile)
{
    // 1. retrieve the vertex/fragment source code from filePath
    std::string vertexCode;
    std::string fragmentCode;
    std::string geometryCode;
    try
    {
        // open files
        std::ifstream vertexShaderFile(vShaderFile);
        std::ifstream fragmentShaderFile(fShaderFile);
        std::stringstream vShaderStream, fShaderStream;
        // read file's buffer contents into streams
        vShaderStream << vertexShaderFile.rdbuf();
        fShaderStream << fragmentShaderFile.rdbuf();
        // close file handlers
        vertexShaderFile.close();
        fragmentShaderFile.close();
        // convert stream into string
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
        // if geometry shader path is present, also load a geometry shader
        if (gShaderFile != nullptr)
        {
            std::ifstream geometryShaderFile(gShaderFile);
            std::stringstream gShaderStream;
            gShaderStream << geometryShaderFile.rdbuf();
            geometryShaderFile.close();
            geometryCode = gShaderStream.str();
        }
    }
    catch (std::exception e)
    {
        std::cout << "ERROR::SHADER: Failed to read shader files" << std::endl;
    }
    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();
    const char* gShaderCode = geometryCode.c_str();
    // 2. now create shader object from source code
    Shader shader;
    shader.Compile(vShaderCode, fShaderCode, gShaderFile != nullptr ? gShaderCode : nullptr);
    return shader;
}

Texture2D ResourceManager::loadTextureFromFile(const char* file)
{
    // create texture object
    Texture2D texture;

    // check if file exists
    std::ifstream infile(file);
    if (!infile.good())
    {
        std::cerr << "Error: File does not exist: " << file << std::endl;
        return texture; // You might want to return a null or default texture here
    }
    infile.close();

    // load image
    int width, height, nrChannels;
    unsigned char* data = stbi_load(file, &width, &height, &nrChannels, 0);

    // check if data is loaded successfully
    if (!data)
    {
        std::cerr << "Error: Failed to load image: " << file << std::endl;
        return texture; // You might want to return a null or default texture here
    }

    // Set the internal and image formats based on the number of channels
    if (nrChannels == 4)
    {
        texture.Internal_Format = GL_RGBA;
        texture.Image_Format = GL_RGBA;
    }
    else if (nrChannels == 3)
    {
        texture.Internal_Format = GL_RGB;
        texture.Image_Format = GL_RGB;
    }
    // Add more conditions here for other formats if needed

    // now generate texture
    texture.Generate(width, height, data);

    // and finally free image data
    stbi_image_free(data);

    return texture;
}