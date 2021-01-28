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
 * @file    skeletonGL.hpp
 * @author  SENEX @ XENOBYTE.XYZ
 * @date    26/01/2021
 * @version 2.0
 * @brief Monolithic include file for lazy people
 * @section DESCRIPTION
 *
 * Includes all the SkeletonGL files in a single header file, the library is pretty small
 * so you might as well.
*/


#ifndef SRC_SKELETONGL_SKELETONGL_H
#define SRC_SKELETONGL_SKELETONGL_H


// General utility
#include "utility/SGL_AssetManager.hpp"
#include "utility/SGL_DataStructures.hpp"
#include "utility/SGL_Utility.hpp"
// Rendering
#include "renderer/SGL_OpenGLManager.hpp"
#include "renderer/SGL_Camera.hpp"
#include "renderer/SGL_PostProcessor.hpp"
#include "renderer/SGL_Shader.hpp"
#include "renderer/SGL_Renderer.hpp"
#include "renderer/SGL_Texture.hpp"
// Window management
#include "window/SGL_Window.hpp"

#endif // SRC_SKELETONGL_SKELETONGL_H
