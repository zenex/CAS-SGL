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
 * @file    src/skeletonGL/utility/SGL_PostProcessor.hpp
 * @author  SENEX @ XENOBYTE.XYZ
 * @date    26/01/2021
 * @version 2.1
 *
 * @brief Processes the frame's final render
 *
 * @section DESCRIPTION
 *
 * See the post processor shader for more filter and effects examples
 */

#ifndef SRC_SKELETONGL_RENDERER_POST_PROCESSOR_HPP
#define SRC_SKELETONGL_RENDERER_POST_PROCESSOR_HPP

// C++
#include <iostream>
#include <string>
#include <memory>
// Dependencies
#include <GL/glew.h>
// SkeletonGL
#include "../utility/SGL_DataStructures.hpp"
#include "../utility/SGL_Utility.hpp"
#include "SGL_OpenGLManager.hpp"
#include "SGL_Shader.hpp"
#include "SGL_Texture.hpp"

/**
 * @brief Post processor for the final texture to be rendered, adds effects and filters
 * @section DESCRIPTION
 *
 * This class encapsulates all final effects.
 */
class SGL_PostProcessor
{
private:
    std::shared_ptr<SGL_OpenGLManager> WMOGLM;                                 ///< Primary OpenGL context
    // std::string pMainFBO, pSecondaryFBO, pMainVBO, pMainVAO, pTextureCoordVBO; ///< OpenGL object containers
    GLuint pRBO;                                                               ///< Render buffer object
    // NOT the same as the window's default shader, this one simply blits the
    // final FBO as it is, the default window shader provides the program's
    // default effect, which may not necessarily be the final vanilla FBO
    SGL_Shader pShader, pActiveShader;                                         ///< Shader to be used
    GLuint pWidth, pHeight;                                                    ///< FBO texture dimensions
    //NOTE: until this leaves testing, keep teh texture here and not in the manager
    SGL_Texture pTexture;                                                      ///< SGL texture for the FBO
    SGL_Color pClearColor;                                                     ///< Clear screen color

    // Load the buffer data
    void loadBuffers();

    // Disable all copying and moving to protect BO
    SGL_PostProcessor(const SGL_PostProcessor&) = delete;
    SGL_PostProcessor *operator = (const SGL_PostProcessor&) = delete;
    SGL_PostProcessor(SGL_PostProcessor &&) = delete;
    SGL_PostProcessor &operator = (SGL_PostProcessor &&) = delete;

public:
    GLboolean confuse, chaos, shake; ///< Toggle the corresponding effect
    // Start the PP with the window's opengl context and the default shader (rander the FBO without any effects)
    SGL_PostProcessor(std::shared_ptr<SGL_OpenGLManager> oglm, const SGL_Shader &defaultShader);
    // Destructor
    ~SGL_PostProcessor();
    // Recreates the FBO
    void reload(GLuint newWidth, GLuint newHeight);
    // The clear color to use for the render
    void setClearColor(SGL_Color color) noexcept;
    // Destroy the instanced OpenGL objects
    void deleteBuffers();
    // Prepare the postpocessor framebuffer operations (before any rendering)
    void beginRender() noexcept;
    // Call it once the rendering is done so it stores everything into a texture object
    void endRender() noexcept;

    // Render the final texture (a screen sized sprite)
    // Render with specific UV coords (camera zoom for example)
    // Note that this shader must be the same as the one used to reload the PP
    // this frame, passing it again allows the renderDetails struct to be safely
    // updated (the window class is responsible for this same shader pass)
    void render(const SGL_Shader &shader, UV_Wrapper *UVdata = nullptr);
};



#endif // SRC_SKELETONGL_RENDERER_POST_PROCESSOR_HPP
