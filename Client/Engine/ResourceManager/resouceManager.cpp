#include <PCH/pch.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#include "resourceManager.h"

// Instantiate static variables
std::map<std::string, Texture2D> ResourceManager::Textures;
std::map<std::string, Shader>    ResourceManager::Shaders;

AudioManager ResourceManager::audioManager;

GLuint ResourceManager::CurrentShaderProgram = -1;
GLuint ResourceManager::CurrentTextureID = -1;
GLuint ResourceManager::CurrentVAO = -1;

Shader& ResourceManager::LoadShader(const std::string& vShaderFile, const std::string& fShaderFile, std::string name)
{
    Shader& shader = Shaders[name];

    shader = loadShaderFromFile(vShaderFile, fShaderFile);
    shader.SetUniformsFromCode();    

    shader.BindUBO("Matricies", Shader::uboMatricesBindingPoint);
    shader.BindUBO("Time", Shader::uboTimeBindingPoint);

    return Shaders[name];
}

Shader& ResourceManager::GetShader(std::string name)
{
    return Shaders[name];
}

void ResourceManager::LoadTexturesFromDirectory(const char* directory)
{
    for (const auto& entry : std::filesystem::directory_iterator(directory)) {
        if (!entry.is_regular_file()) continue;

        std::filesystem::path filePath = entry.path();
        std::string filename = filePath.filename().string();

        unsigned int columns = 1, rows = 1;
        std::string key = filePath.stem().string();

        if (filename.find("ss") == 0) {
            sscanf_s(key.c_str(), "ss%ux%u", &columns, &rows);
        }
        
        LoadTexture(filePath.string(), key, columns, rows);
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

Texture2D& ResourceManager::LoadTexture(const std::string& file, std::string name, unsigned columns, unsigned rows)
{
    Textures[name] = loadTextureFromFile(file, columns, rows);
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

void ResourceManager::PlaySound(const std::string& source, bool isLooping, float pitch, float volumeMultiplier, bool isSpatial, const glm::vec2& soundPos, float soundRange)
{
    Sound sound(audioManager);
    sound.Play("Assets/Audio/" + source, isLooping, pitch, volumeMultiplier, isSpatial, soundPos, soundRange);
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

Texture2D ResourceManager::loadTextureFromFile(const std::string& file, unsigned columns, unsigned rows)
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
    std::cout << "Texture Loaded: " << file.c_str() << std::endl;

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

    // Calculate the width and height of a single sprite
    texture.SpriteWidth = width / columns;
    texture.SpriteHeight = height / rows;
    texture.Rows = rows;
    texture.Columns = columns;
    texture.Index = 0;
    texture.IsSpriteSheet = columns != 1 || rows != 1;
    
    texture.Generate(width, height, data, rows * columns);
    std::cout << std::endl;

    // and finally free image data
    stbi_image_free(data);

    return texture;
}