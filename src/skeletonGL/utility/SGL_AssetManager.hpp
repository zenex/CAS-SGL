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
 * @file    src/skeletonGL/window/SGL_AssetManager.cpp
 * @author  SENEX @ XENOBYTE.XYZ
 * @date    26/01/2021
 * @version 2.1
 *
 * @brief Loads and manages all assets, including compiling and linking shaders
 *
 * @section DESCRIPTION
 *
 * This class loads and converts asset files into usable resources.
 * Image files supported: PNG, JPG, BMP
 * Font formats supported: TTF
 *
 */
#ifndef SRC_SKELETONGL_ASSETS_ASSET_MANAGER_HPP
#define SRC_SKELETONGL_ASSETS_ASSET_MANAGER_HPP
// C++
#include <map>
#include <string>
#include <vector>
#include <memory>
// SkeletonGL
#include "../renderer/SGL_OpenGLManager.hpp"
#include "../renderer/SGL_Texture.hpp"
#include "../renderer/SGL_Shader.hpp"
#include "SGL_DataStructures.hpp"
#include "SGL_Utility.hpp"

/**
 *  @brief Manages all rendering resources
 */
class SGL_AssetManager
{
private:
    std::shared_ptr<SGL_OpenGLManager> WMOGLM;                    ///< Owned by the windowManager
    std::map<std::string, SGL_Shader> shaders;                    ///< Map of all available shaders
    std::map<std::string, SGL_Texture> textures;                  ///< Map of all available textures
    std::uint64_t pTextureGPUMemory;                              ///< How much GPU memory the program is using

    // Parses, compiles and links a shader, geometry shader is optional
    SGL_Shader loadShaderFromFile(SHADER_TYPE shaderType, const GLchar *vertexSource, const GLchar *fragmentSource, const GLchar *geometrySource = nullptr);
    // Creates an OpenGL texture from a supported image
    SGL_Texture loadTextureFromFile(const GLchar *file, GLboolean alpha);

public:
    // Constructor
    SGL_AssetManager(std::shared_ptr<SGL_OpenGLManager> oglm);
    // Destructor
    ~SGL_AssetManager();

    //loads and generates a shader program from a source file
    SGL_Shader loadShaders(const GLchar *vertexSource, const GLchar *fragmentSource, const GLchar *geometrySource, std::string name, SHADER_TYPE shaderType);
    //loads and generates a texture from a file, throws SGL_Exception if shit happens
    SGL_Texture loadTexture(const GLchar *file, GLboolean alpha, std::string name);

    // Finds and retrieves a stored shader, throws on error (can't render without a shader)
    // Note that these const functions used to return map[key] instead of map.at(key) and thus
    // weren't "constable" since map[key] creates an entry if not found
    SGL_Shader getShader(std::string name) const;
    //finds and retrieves a stored texture
    SGL_Texture getTexture(std::string name) const;

    // This map contains an accessible container
    // with all the loaded shaders and their respective
    // types, this to differentiate between shaders
    // that the main camera's MVP matrix must update
    // (only non FBO shaders so far)
    std::map<std::string, SHADER_TYPE> shaderTypes;

    // Returns the total GPU memory allocated
    int getTextureMemoryGPU() const;
};
#endif // SRC_SKELETONGL_ASSETS_ASSET_MANAGER_HPP
