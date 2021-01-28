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
 * @file    src/skeletonGL/utility/SGL_Texture.hpp
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


#ifndef SRC_SKELETONGL_RENDERER_TEXTURE2D_H
#define SRC_SKELETONGL_RENDERER_TEXTURE2D_H

// C++
#include <string>
// Dependencies
#include <GL/glew.h>
#include <SDL2/SDL.h>
// SkeletonGL
#include "SGL_OpenGLManager.hpp"


/**
 * @brief Defines an SGL_Texture to be used by a sprite
 */
class SGL_Texture
{
public:
    GLuint ID;                                 ///< Internal OpenGL manager identifier
    GLuint width, height;                      ///< Texture dimensions
    GLuint internalFormat;                     ///< Texture internal format
    GLuint imageFormat;                        ///< Image file format
    GLuint wrapS, wrapT, filterMin, filterMax; ///< Texture settings

    // Constructor
    SGL_Texture();

    // Destructor
    ~SGL_Texture();

    // Generates an SGL_Texture
    void generate(SGL_OpenGLManager &oglm, GLuint width, GLuint height, unsigned char *data) noexcept;

    // Activates the texture
    void bind(SGL_OpenGLManager &oglm) const noexcept;

};

#endif // SRC_SKELETONGL_RENDERER_TEXTURE2D_H
