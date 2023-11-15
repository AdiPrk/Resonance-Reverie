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

Shader& ResourceManager::LoadShader(const std::string& vShaderFile, const std::string& fShaderFile, std::string name)
{
    Shader& shader = Shaders[name];

    shader = loadShaderFromFile(vShaderFile, fShaderFile);
    shader.SetUniformsFromCode();    

    Shader::BindUBO(shader, "Matricies", Shader::uboMatricesBindingPoint);
    Shader::BindUBO(shader, "Time", Shader::uboTimeBindingPoint);

    return Shaders[name];
}

Shader& ResourceManager::GetShader(std::string name)
{
    return Shaders[name];
}

void ResourceManager::LoadTexturesFromDirectory(const char* directory)
{
    for (const auto& entry : std::filesystem::directory_iterator(directory)) {
        if (entry.is_regular_file()) {
            std::filesystem::path filePath = entry.path();
            std::string filename = filePath.filename().string();
            // Assuming the key for the texture is the filename without the extension
            std::string key = filePath.stem().string();
            LoadTexture(filePath.string(), key);
        }
    }
}

void ResourceManager::LoadShadersFromDirectory(const char* directory)
{
    std::map<std::string, std::string> vertexShaders;
    std::map<std::string, std::string> fragmentShaders;

    // First, iterate over the shader files and sort them into vertex and fragment shaders
    for (const auto& entry : std::filesystem::directory_iterator(directory)) {
        if (entry.is_regular_file()) {
            std::filesystem::path filePath = entry.path();
            std::string filename = filePath.filename().string();

            if (filePath.extension() == ".vert") {
                std::string key = filePath.stem().string();
                vertexShaders[key] = filePath.string();
            }
            else if (filePath.extension() == ".frag") {
                std::string key = filePath.stem().string();
                fragmentShaders[key] = filePath.string();
            }
        }
    }

    // Now, for each vertex shader, find its corresponding fragment shader and load them
    for (const auto& [key, vertPath] : vertexShaders) {
        auto fragIter = fragmentShaders.find(key);
        if (fragIter != fragmentShaders.end()) {
            // Found a matching fragment shader
            LoadShader(vertPath, fragIter->second, key);
        }
        else {
            // Handle the case where a matching fragment shader is not found
            std::cerr << "Fragment shader for " << key << " not found." << std::endl;
        }
    }
}

Texture2D& ResourceManager::LoadTexture(const std::string& file, std::string name)
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

void ResourceManager::UpdateAllShaderTimes(float time)
{
    for (auto& [name, shader] : Shaders)
    {
        if (shader.HasUniform("iTime"))
        {
            shader.SetFloat("iTime", time);
        }
    }
}

Shader ResourceManager::loadShaderFromFile(const std::string& vShaderFile, const std::string& fShaderFile)
{
    // 1. retrieve the vertex/fragment source code from filePath
    std::string vertexCode;
    std::string fragmentCode;
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
    }
    catch (std::exception e)
    {
        std::cout << "ERROR::SHADER: Failed to read shader files" << std::endl;
    }
    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();
    // 2. now create shader object from source code
    Shader shader;
    shader.Compile(vShaderCode, fShaderCode);
    return shader;
}

Texture2D ResourceManager::loadTextureFromFile(const std::string& file)
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
    unsigned char* data = stbi_load(file.c_str(), &width, &height, &nrChannels, 0);

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