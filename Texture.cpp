#pragma once
#define STB_IMAGE_IMPLEMENTATION

#include <iostream>
#include "Texture.h"
#include "stb_image.h"

Texture::Texture(GLenum TextureTarget, const std::string& FileName)
{
    m_textureTarget = TextureTarget;
    m_fileName = FileName;
}

bool Texture::Load()
{
    stbi_set_flip_vertically_on_load(0);
    int width = 0, height = 0, bpp = 0;
    unsigned char* image_data = stbi_load(m_fileName.c_str(), &width, &height, &bpp, 0);

    if (!image_data) {
        printf("Can't load texture from '%s' - %s\n", m_fileName.c_str(), stbi_failure_reason());
        exit(0);
    }
    printf("Width %d, height %d, bpp %d\n", width, height, bpp);

    glGenTextures(1, &m_textureObj);
    glBindTexture(m_textureTarget, m_textureObj);
    if (m_textureTarget == GL_TEXTURE_2D) {
        switch (bpp) {
        case 1:
            glTexImage2D(m_textureTarget, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, image_data);
            break;
        case 3:
            glTexImage2D(m_textureTarget, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image_data);
            break;
        case 4:
            glTexImage2D(m_textureTarget, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
            break;
        default:
            printf("Not implemented case in %s:%d\n", __FILE__, __LINE__); \
                exit(0);
        }
    }
    else {
        printf("Support for texture target %x is not implemented\n", m_textureTarget);
        exit(1);
    }
    glTexParameterf(m_textureTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(m_textureTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(m_textureTarget, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(m_textureTarget, GL_TEXTURE_WRAP_T, GL_REPEAT);
   // glBindTexture(m_textureTarget, 0);
    stbi_image_free(image_data);
    return true;
}

void Texture::Bind(GLenum TextureUnit)
{
    glActiveTexture(TextureUnit);
    glBindTexture(m_textureTarget, m_textureObj);
}