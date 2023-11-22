#include <PCH/pch.h>

#include "texture.h"
#include <Source/Graphics/ResourceManager/resourceManager.h>

Texture2D::Texture2D()
    : Width(0)
    , Height(0)
    , Rows(1)
    , Columns(1)
    , Index(0)
    , IsSpriteSheet(false)
    , Internal_Format(GL_RGB)
    , Image_Format(GL_RGB)
    , Wrap_S(GL_REPEAT)
    , Wrap_T(GL_REPEAT)
    , Filter_Min(GL_LINEAR)
    , Filter_Max(GL_LINEAR)
    , textureHandle()
{
    glGenTextures(1, &this->ID);
}

void Texture2D::Generate(unsigned int width, unsigned int height, unsigned char* data)
{
    this->Width = width;
    this->Height = height;

    // create Texture
    glBindTexture(GL_TEXTURE_2D, this->ID);
    glTexImage2D(GL_TEXTURE_2D, 0, this->Internal_Format, Width, Height, 0, this->Image_Format, GL_UNSIGNED_BYTE, data);

    // set Texture wrap and filter modes
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, this->Wrap_S);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, this->Wrap_T);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, this->Filter_Min);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, this->Filter_Max);

    // Generate a handle for the texture and make it resident
    textureHandle = glGetTextureHandleARB(this->ID);
    glMakeTextureHandleResidentARB(textureHandle);
}

void Texture2D::Bind() const
{
    if (this->ID != ResourceManager::CurrentTextureID) {
        glBindTexture(GL_TEXTURE_2D, this->ID);
        ResourceManager::CurrentTextureID = this->ID;
    }
}