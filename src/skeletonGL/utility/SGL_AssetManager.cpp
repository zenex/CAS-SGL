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
 * @file    src/skeletonGL/utility/SGL_AssetManager.cpp
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

// C++
#include <iostream>
#include <sstream>
#include <fstream>
// SkeletonGL
#include "SGL_AssetManager.hpp"
// STB image
#define STB_IMAGE_IMPLEMENTATION
#include "../deps/stb_image.hpp"


/**
 * @brief Main and only constructor
 *
 * @param oglm Main window SGL_OpenGLManager
 * @return nothing
 */
SGL_AssetManager::SGL_AssetManager(std::shared_ptr<SGL_OpenGLManager> oglm)
{
    WMOGLM = oglm;
    pTextureGPUMemory = 0;
}

/**
 * @brief Destructor
 * @return nothing
 */
SGL_AssetManager::~SGL_AssetManager()
{

}

/**
 * @brief Compiles and links a shader program from the provided shader files
 *
 * @param vertexSource C string to the vertex shader file
 * @param fragmentSource C string to the fragment shader file
 * @param geometrySource C string to the geometry shader file (optional)
 * @param name The shader program name as string
 * @param shaderType What kind of shader is this?
 * @return SGL_Shader requested, if found
 */
SGL_Shader SGL_AssetManager::loadShaders(const GLchar *vertexSource, const GLchar *fragmentSource, const GLchar *geometrySource, std::string name, SHADER_TYPE shaderType)
{
    if (shaders.count(name) > 0)
    {
        SGL_Log("Shader already exists: " + name, LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_DEFAULT);
        return shaders[name];
    }
    else
    {
        shaders[name] = loadShaderFromFile(shaderType, vertexSource, fragmentSource, geometrySource);
        shaders[name].name = name;
        shaderTypes[name] = shaderType;
        return shaders[name];
    }
}

/**
 * @brief Fetch a previously loaded shader program, simply throw / crash the program if it fails
 *
 * @param name Name of the shader to be returned
 * @return SGL_Shader The requested shader
 */
SGL_Shader SGL_AssetManager::getShader(std::string name) const
{
    std::string msg = "SGL_AssetManager::getShader | Shader not found: " + name;
    if (shaders.count(name) <= 0)
        throw SGL_Exception(msg.c_str());

    return shaders.at(name);
}


/**
 * @brief Load a texture from a supported iamge file and generate an OpenGL texture
 *
 * @param file C string to the image file
 * @param alpha Does thee image have an alpha channel (PNG exclusive, not all PNGs have it though)
 * @param name Name of the final texture
 * @return SGL_Texture The requested texture
 */
SGL_Texture SGL_AssetManager::loadTexture(const GLchar *file, GLboolean alpha, std::string name)
{
    if (textures.count(name) > 0)
    {
        SGL_Log("2DTexture already exists: " + name, LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_DEFAULT);
        return textures[name];
    }
    else
    {
        SGL_Log("Generating texture: " + std::string(file), LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_DEFAULT);
        textures[name] = loadTextureFromFile(file, alpha);
        return textures[name];
    }
}

/**
 * @brief Fetch a previously loaded texture, unlike getShader, we can provide a default texture instead of crashing
 *
 * @param name String to the image file
 * @return SGL_Texture The requested texture or the default one
 */
SGL_Texture SGL_AssetManager::getTexture(std::string name) const
{
    if (textures.count(name) <= 0)
        return textures.at(SGL::DEFAULT_TEXTURE);
    else
        return textures.at(name);
}

/**
 * @brief Compile and link a shader program, any error will lead to a crash
 *
 * @param shaderType What kind of shader is this?
 * @param vertexSource C string to the vertex shader file
 * @param fragmentSource C string to the fragment shader file
 * @param geometrySource C string to the geometry shader file (optional)
 * @return SGL_Shader The final shader program
 */
SGL_Shader SGL_AssetManager::loadShaderFromFile(SHADER_TYPE shaderType, const GLchar *vertexSource, const GLchar *fragmentSource, const GLchar *geometrySource)
{
    // Retrieve the source from the files
    std::string vertexCode;
    std::string fragmentCode;
    std::string geometryCode;


    try
    {
        // Open that shit up
        std::ifstream vertexShaderFile(vertexSource);
        std::ifstream fragmentShaderFile(fragmentSource);
        std::stringstream vertexStream, fragmentStream;
        // Read file buffer contents into the streams
        vertexStream << vertexShaderFile.rdbuf();
        fragmentStream << fragmentShaderFile.rdbuf();
        // Close file handlers
        vertexShaderFile.close();
        fragmentShaderFile.close();
        // Convert stream into string
        vertexCode = vertexStream.str();
        fragmentCode = fragmentStream.str();
        // If geometry shader is present, load it
        if(geometrySource != nullptr)
        {
            std::ifstream geometryShaderFile(geometrySource);
            std::stringstream geometryStream;
            geometryStream << geometryShaderFile.rdbuf();
            geometryShaderFile.close();
            geometryCode = geometryStream.str();
        }
    }
    catch(std::exception e)
    {
        SGL_Log("ERROR::SHADER: Failed to read shader files:\n", LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_DEFAULT);
        SGL_Log("Vertex: " + std::string(vertexSource), LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_DEFAULT);
        SGL_Log("Fragment: " + std::string(fragmentSource), LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_DEFAULT);
        if (geometrySource != nullptr)
            SGL_Log("Geometry: " + std::string(geometrySource), LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_DEFAULT);
    }
    const GLchar *vShaderSource = vertexCode.c_str();
    const GLchar *fShaderSource = fragmentCode.c_str();
    const GLchar *gShaderSource = geometryCode.c_str();
    // Create shader object from source code
    SGL_Shader shader;
    SGL_Log("Compiling vertex shader: " + std::string(vertexSource), LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_DEFAULT);
    SGL_Log("Compiling fragment shader: " + std::string(fragmentSource), LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_DEFAULT);

    try
    {
        if (geometrySource != nullptr)
        {
            SGL_Log("Compiling geometry shader: " + std::string(geometrySource), LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_DEFAULT);
            shader.compileShaders(*WMOGLM, shaderType, vShaderSource, fShaderSource, gShaderSource);
        }
        else
        {
            shader.compileShaders(*WMOGLM, shaderType, vShaderSource, fShaderSource, nullptr);
        }
    }
    catch (SGL_Exception &e)
    {
        SGL_Log(e.what());
        throw SGL_Exception("SGL_AssetManager::loadShaderFromFile | Error processing shaders");
    }
    // Shaders successfully loaded and compiled
    return shader;
}

/**
 * @brief Loads an image file and generates an OpenGL texture
 *
 * @param file C string to the image file
 * @param alpha Does the image have an alpha channel?
 * @return SGL_Texture The requested texture or the default one
 */
SGL_Texture SGL_AssetManager::loadTextureFromFile(const GLchar *file, GLboolean alpha)
{
    //create a texture object
    SGL_Texture texture;
    // texture.imageFormat = GL_RGBA; // NOTE: causes segfaults when trying to load certain PNG files with no alpha
    if(alpha)
    {
        texture.internalFormat = GL_RGBA8;
        texture.imageFormat = GL_RGBA;
    }
    else
    {
        texture.internalFormat = GL_RGBA8;
        texture.imageFormat = GL_RGB;
    }
    //load image
    int width, height, channels;
    stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
    unsigned char *image = stbi_load(file, &width, &height, &channels, 0);
    if (image == NULL)
    {
        SGL_Log("Texture not found: " + std::string(file));
        return this->getTexture(SGL::DEFAULT_TEXTURE);
    }
    else
    {
        int GPUMemory = ((width * height)*4);
        //generate the texture
        texture.generate(*WMOGLM, width, height, image);
        std::string textureData = "| W: " + std::to_string(width) +
            " H: " + std::to_string(height) +
            " | A: " + ((alpha) ? "YES" : "NO") +
            " | ID: " + std::to_string(texture.ID) +
            " | MEM: " + std::to_string(GPUMemory/1024) + "(bytes) |";
        SGL_Log(textureData, LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_DEFAULT);
        //add its size to the total GPU memory counter
        //texture internal format must be set to GL_RGBA8
        //so each pixel takes exactly 4 bytes of memory
        pTextureGPUMemory += GPUMemory;
        SGL_Log("Done.", LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_DEFAULT);
    }
    stbi_image_free(image);
    return texture;
}

/**
 * @brief Return the total GPU memory allocated by SGL
 * @return int Total memory in bytes
 */
int SGL_AssetManager::getTextureMemoryGPU() const
{
    //return pTextureGPUMemory;
    return static_cast<int>(pTextureGPUMemory + SGL_OGL_CONSTANTS::MAX_LINE_BATCH_INSTANCES + SGL_OGL_CONSTANTS::MAX_SPRITE_BATCH_INSTANCES + SGL_OGL_CONSTANTS::MAX_PIXEL_BATCH_INSTANCES);
}
