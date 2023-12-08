#pragma once

#include <PCH/pch.h>

#include <Engine/Graphics/Shader/shader.h>
#include <Engine/Graphics/texture/texture.h>
#include <Engine/Audio/audioManager.h>

namespace Dog {

    // A static ResourceManager class with a lot of useful features
    class ResourceManager
    {
    public:
        // resource storage
        static std::map<std::string, Shader>    Shaders;
        static std::map<std::string, Texture2D> Textures;

        // retrieves the stored asset
        static Shader& GetShader(std::string name);
        static Texture2D& GetTexture(std::string name);

        // Loads in all assets from the given directory
        static void LoadTexturesFromDirectory(const char* directory);
        static void LoadShadersFromDirectory(const char* directory);

        // Loads in the asset from the given files
        static Shader& LoadShader(const std::string& vShaderFile, const std::string& fShaderFile, std::string name);
        static Texture2D& LoadTexture(const std::string& file, std::string name, unsigned columns, unsigned rows);

        // properly de-allocates all loaded resources
        static void Clear();

        // Used to keep track of current states
        static GLuint CurrentShaderProgram;
        static GLuint CurrentTextureID;
        static GLuint CurrentVAO;

        // Bind a VAO
        static void BindVAO(GLuint quadVAO);

        // Update the iTime variable on all shaders
        static void UpdateAllShaderTimes(float time);

        static AudioManager& GetAudioManager() { return audioManager; }
        static void PlaySound(const std::string& source, bool isLooping = false, float pitch = 1.0f, float volumeMultiplier = 1.0f, bool isSpatial = false, const glm::vec2& soundPos = { 0, 0 }, float soundRange = 500.0f);

    private:
        // private constructor, that is we do not want any actual resource manager objects. Its members and functions should be publicly available (static).
        ResourceManager() { }

        // loads and generates a shader from file
        static Shader    loadShaderFromFile(const std::string& vShaderFile, const std::string& fShaderFile);

        // loads a single texture from file
        static Texture2D loadTextureFromFile(const std::string& file, unsigned columns, unsigned rows);

        // single audiomanager instance
        static AudioManager audioManager;
    };

    // A thread pool!
    namespace ThreadPool {

        static unsigned int numThreads = std::thread::hardware_concurrency() - 1;
        static BS::thread_pool threadPool(numThreads);

    }

}