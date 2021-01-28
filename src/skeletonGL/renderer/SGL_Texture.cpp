// ╔═╗╦╔═╔═╗╦  ╔═╗╔╦╗╔═╗╔╗╔╔═╗╦
// ╚═╗╠╩╗║╣ ║  ║╣  ║ ║ ║║║║║ ╦║
// ╚═╝╩ ╩╚═╝╩═╝╚═╝ ╩ ╚═╝╝╚╝╚═╝╩═╝
// ─┐ ┬┌─┐┌┐┌┌─┐┌┐ ┬ ┬┌┬┐┌─┐ ─┐ ┬┬ ┬┌─┐
// ┌┴┬┘├┤ ││││ │├┴┐└┬┘ │ ├┤  ┌┴┬┘└┬┘┌─┘
// ┴ └─└─┘┘└┘└─┘└─┘ ┴  ┴ └─┘o┴ └─ ┴ └─┘
// Author:  SENEX @ XENOBYTE.XYZ
// License: MIT License
// Website: https://xenobyte.xyz/projects/?nav=skeletongl

/**
 * @file    src/skeletonGL/utility/SGL_Texture.cpp
 * @author  SENEX @ XENOBYTE.XYZ
 * @date    26/01/2021
 * @version 2.1
 *
 * @brief Wraps around all necessary texture functionality
 *
 * @section DESCRIPTION
 *
 * Encapsulates all the required texture functionality
 */
// C++
#include <iostream>
// Skeletongl
#include "SGL_Texture.hpp"

/**
 * @brief Constructor
 * @return nothing
 * @section DESCRIPTION
 *
 * The texture internal format determines the actual size of the texture's
 * buffer in the GPU, a format of GL_RGBA takes 32 bits or 4 bytes per pixel
 */
SGL_Texture::SGL_Texture() :
    width(0), height(0), internalFormat(GL_RGBA8), imageFormat(GL_RGB), wrapS(GL_REPEAT),
    wrapT(GL_REPEAT), filterMin(GL_LINEAR), filterMax(GL_NEAREST)
{

}

/**
 * @brief Destructor
 * @return nothing
 */
SGL_Texture::~SGL_Texture()
{

}

/**
 * @brief Generates a texture
 * @param oglm Window OpenGL context
 * @param width Texture width
 * @param height Texture height
 * @param data Pointer to the texture data
 *
 * @return nothing
 * */
void SGL_Texture::generate(SGL_OpenGLManager &oglm, GLuint width, GLuint height, unsigned char *data) noexcept
{
    oglm.genTextures(1, &this->ID);
    this->width = width;
    this->height = height;
    // Create texture
    oglm.bindTexture(GL_TEXTURE_2D, this->ID);
    oglm.texImage2D(GL_TEXTURE_2D, 0, this->internalFormat, width, height, 0, this->imageFormat, GL_UNSIGNED_BYTE, data);
    // Set texture wrap and filter modes
    oglm.texParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, this->wrapS);
    oglm.texParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, this->wrapT);
    oglm.texParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, this->filterMin);
    oglm.texParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, this->filterMax);
    // Unnbind it
    oglm.bindTexture(GL_TEXTURE_2D, 0);
}

/**
 * @brief Activates the texture
 * @return nothing
 */
void SGL_Texture::bind(SGL_OpenGLManager &oglm) const noexcept
{
    oglm.bindTexture(GL_TEXTURE_2D, this->ID);
}

