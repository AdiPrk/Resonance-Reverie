#include <PCH/pch.h>

#include "texture.h"
#include <Engine/ResourceManager/resourceManager.h>

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

void Texture2D::Generate(unsigned int width, unsigned int height, unsigned char* data, unsigned int numSprites)
{
    this->Width = width;
    this->Height = height;
    this->NumSprites = numSprites;

    if (data) {
        std::cout << "Creating " << Columns << "x" << Rows << " Texture, Size: " << width << "x" << height << std::endl;
    }
    else {
        std::cout << "Loading Empty Texture:" << std::endl;
        std::cout << "Creating " << Columns << "x" << Rows << " Texture, Size: " << width << "x" << height << std::endl;
    }
    // create Texture
    if (numSprites == 1) {
        glBindTexture(GL_TEXTURE_2D, this->ID);

        glTexImage2D(GL_TEXTURE_2D, 0, this->Internal_Format, Width, Height, 0, this->Image_Format, GL_UNSIGNED_BYTE, data);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, this->Wrap_S);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, this->Wrap_T);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, this->Filter_Min);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, this->Filter_Max);
    }
    else {
        // glBindTexture(GL_TEXTURE_2D_ARRAY, this->ID);
        // glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_RGB8, width, height, 1);
        // glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 0, width, height, 1, Internal_Format, GL_UNSIGNED_BYTE, data);

        glBindTexture(GL_TEXTURE_2D_ARRAY, this->ID);

        // rgba8 works, rgba doesn't, ?
        glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_RGBA8, SpriteWidth, SpriteHeight, NumSprites);
        
        glPixelStorei(GL_UNPACK_ROW_LENGTH, Width);

        for (int i = 0; i < static_cast<int>(NumSprites); ++i)
        {
            int xOffSet = i % Columns * SpriteWidth;
            int yOffSet = (i / Columns) * SpriteHeight;
            glPixelStorei(GL_UNPACK_SKIP_PIXELS, xOffSet);
            glPixelStorei(GL_UNPACK_SKIP_ROWS, yOffSet);

            // printf("Index: %i, xOffset: %i, yOffset: %i\n", i, xOffSet, yOffSet);

            glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, SpriteWidth, SpriteHeight, 1, Internal_Format, GL_UNSIGNED_BYTE, data);
        }

        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    } 

    // Generate a handle for the texture and make it resident
    textureHandle = glGetTextureHandleARB(this->ID);
    glMakeTextureHandleResidentARB(textureHandle);

    // Reset stuff
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
    glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
}

void Texture2D::Bind() const
{
    if (this->ID != ResourceManager::CurrentTextureID) {
        glBindTexture(GL_TEXTURE_2D, this->ID);
        ResourceManager::CurrentTextureID = this->ID;
    }
}