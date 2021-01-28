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
 * @file    src/skeletonGL/utility/SGL_PostProcessor.cpp
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

#include "SGL_PostProcessor.hpp"

/**
 * @brief Destructor
 *
 * @param
 * @return nothing
 */
SGL_PostProcessor::~SGL_PostProcessor()
{

}

/**
 * @brief Destroys all instanced OpenGL buffers
 *
 * @return nothing
 */
void SGL_PostProcessor::deleteBuffers()
{
    WMOGLM->deleteVAO(SGL_OGL_CONSTANTS::SGL_POSTPROCESSOR_VAO);
    WMOGLM->deleteFBO(SGL_OGL_CONSTANTS::SGL_POSTPROCESSOR_PRIMARY_FBO);
    WMOGLM->deleteFBO(SGL_OGL_CONSTANTS::SGL_POSTPROCESSOR_SECONDARY_FBO);
    WMOGLM->deleteVBO(SGL_OGL_CONSTANTS::SGL_POSTPROCESSOR_VBO);
}

/**
 * @brief Main and only constructor
 *
 * @param oglm The window's OpenGL context
 * @param defaultShader The default (no effects, just blit the FBO texture) shader
 * @return nothing
 */
SGL_PostProcessor::SGL_PostProcessor(std::shared_ptr<SGL_OpenGLManager> oglm, const SGL_Shader &defaultShader) :
    confuse(GL_FALSE), shake(GL_FALSE), chaos(GL_FALSE)
{
    if (defaultShader.shaderType == SHADER_TYPE::POST_PROCESSOR)
        pShader = defaultShader;
    else
        throw SGL_Exception("SGL_PostProcessor::SGL_PostProcessor | Default shader isn't of type POST_PROCESSOR");

    this->WMOGLM = oglm;
}


/**
 * @brief Changes the clear screen color
 *
 * @param color The clear screen color
 * @return nothing
 */
void SGL_PostProcessor::setClearColor(SGL_Color color) noexcept
{
    this->pClearColor = color;
}


/**
 * @brief Reloades the FBO
 *
 * @param newWidth New FBO width
 * @param newHeight New FBO height
 * @return nothing
 */
void SGL_PostProcessor::reload(GLuint newWidth, GLuint newHeight)
{
    pHeight = newHeight;
    pWidth = newWidth;
    // Init frame buffer and render buffer objects
    WMOGLM->checkForGLErrors();
    WMOGLM->createFBO(SGL_OGL_CONSTANTS::SGL_POSTPROCESSOR_PRIMARY_FBO);
    WMOGLM->createFBO(SGL_OGL_CONSTANTS::SGL_POSTPROCESSOR_SECONDARY_FBO);

    glDeleteRenderbuffers(1, &this->pRBO);
    glGenRenderbuffers(1, &this->pRBO);

    // Initialize renderbuffer storage with a multisampled color buffer (don't need a depth/stencil buffer)
    WMOGLM->bindFBO(SGL_OGL_CONSTANTS::SGL_POSTPROCESSOR_PRIMARY_FBO);

    glBindRenderbuffer(GL_RENDERBUFFER, this->pRBO);
    // Allocate storage for render buffer object
    glRenderbufferStorage(GL_RENDERBUFFER, GL_RGB, pWidth, pHeight);
    //glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, pWidth, pHeight);
    WMOGLM->checkForGLErrors();

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, pRBO);
    //glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, pRBO);
    WMOGLM->checkForGLErrors();

    // Check FBO status
    if (WMOGLM->checkFrameBufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        WMOGLM->checkForGLErrors();
        SGL_Log("ERROR::POST_PROCESSOR: Failed to initialize MSFBO", LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_DEFAULT);
    }

    // Initialize the FBO/texture to blit MS color buffer to; used for shader operations
    WMOGLM->bindFBO(SGL_OGL_CONSTANTS::SGL_POSTPROCESSOR_SECONDARY_FBO);
    this->pTexture.generate(*WMOGLM, pWidth, pHeight, NULL);
    // Atach the texture to th FBO
    WMOGLM->frameBufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->pTexture.ID, 0);
    // Check status
    if (WMOGLM->checkFrameBufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        WMOGLM->checkForGLErrors();
        SGL_Log("ERROR::POST_PROCESSOR: Failed to initialize FBO", LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_DEFAULT);
    }
    WMOGLM->unbindFBO();

    // Load buffers and init data
    this->loadBuffers();
    this->pShader.setInteger(*WMOGLM, SGL_OGL_CONSTANTS::SHADER_UNIFORM_I_SCENE.c_str(), 0, GL_TRUE);

    // GLfloat offset = 1.0f / 300.0f;
    // GLfloat offsets[9][2] = {
    //     { -offset,  offset  },  // top-left
    //     {  0.0f,    offset  },  // top-center
    //     {  offset,  offset  },  // top-right
    //     { -offset,  0.0f    },  // center-left
    //     {  0.0f,    0.0f    },  // center-center
    //     {  offset,  0.0f    },  // center - right
    //     { -offset, -offset  },  // bottom-left
    //     {  0.0f,   -offset  },  // bottom-center
    //     {  offset, -offset  }   // bottom-right
    // };
    // WMOGLM->uniform2fv(glGetUniformLocation(this->pShader.ID, "offsets"), 9, (GLfloat*)offsets);
    // GLint edge_kernel[9] = {
    //     -1, -1, -1,
    //     -1,  8, -1,
    //     -1, -1, -1
    // };
    // WMOGLM->uniform1iv(glGetUniformLocation(this->pShader.ID, "edge_kernel"), 9, edge_kernel);
    // GLfloat blur_kernel[9] = {
    //     1.0 / 16, 2.0 / 16, 1.0 / 16,
    //     2.0 / 16, 4.0 / 16, 2.0 / 16,
    //     1.0 / 16, 2.0 / 16, 1.0 / 16
    // };
    // WMOGLM->uniform1fv(glGetUniformLocation(this->pShader.ID, "blur_kernel"), 9, blur_kernel);
}


/**
 * @brief Must be called before rendering
 *
 * @return nothing
 */
void SGL_PostProcessor::beginRender() noexcept
{
    WMOGLM->bindFBO(SGL_OGL_CONSTANTS::SGL_POSTPROCESSOR_PRIMARY_FBO);
    //SGL_Color color{1.0f, 1.0f, 1.0f, 1.0f};
    WMOGLM->setClearColor(pClearColor);
    WMOGLM->clearColorBuffer();
    //WMOGLM->clearDepthBuffer();
}


/**
 * @brief Must be called after rendering
 *
 * @return nothing
 */
void SGL_PostProcessor::endRender() noexcept
{
    // Resolve multisampled color-buffer into intermediate FBO to store to texture
    WMOGLM->bindFBO(SGL_OGL_CONSTANTS::SGL_POSTPROCESSOR_PRIMARY_FBO, GLCONSTANTS::R);
    WMOGLM->bindFBO(SGL_OGL_CONSTANTS::SGL_POSTPROCESSOR_SECONDARY_FBO, GLCONSTANTS::W);
    WMOGLM->blitFrameBuffer(0, 0, this->pWidth, this->pHeight, 0, 0, this->pWidth, this->pHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST);
    WMOGLM->unbindFBO();
}



/**
 * @brief Render the final texture to the screen
 *
 * @param shader The shader (effect) to use
 * @param UVdata The texture coordinates for the final geometry render
 * @return nothing
 */
void SGL_PostProcessor::render(const SGL_Shader &shader, UV_Wrapper *UVdata)
{
    SGL_Shader activeShader;
    activeShader = shader;


    if (UVdata != nullptr)
    {
        GLfloat UV[] = {
            UVdata->UV_topLeft.x / pWidth, UVdata->UV_topLeft.y / pHeight,
            UVdata->UV_botRight.x / pWidth, UVdata->UV_botRight.y / pHeight,
            UVdata->UV_botLeft.x / pWidth, UVdata->UV_botLeft.y / pHeight,

            UVdata->UV_topLeft.x / pWidth, UVdata->UV_topLeft.y / pHeight,
            UVdata->UV_topRight.x / pWidth, UVdata->UV_topRight.y / pHeight,
            UVdata->UV_botRight.x / pWidth, UVdata->UV_botRight.y / pHeight
        };
        WMOGLM->bindVAO(SGL_OGL_CONSTANTS::SGL_POSTPROCESSOR_VAO);
        WMOGLM->bindVBO(SGL_OGL_CONSTANTS::SGL_POSTPROCESSOR_TEXTURE_UV_VBO);
        WMOGLM->bufferSubData(GL_ARRAY_BUFFER, NULL, sizeof(UV), &UV[0]);
    }
    else
    {
        GLfloat UV[] = {
            0.0f, 0.0f, //top left
            1.0f, 1.0f, //bot right
            0.0f, 1.0f, //bot left

            0.0f, 0.0f, //top left
            1.0f, 0.0f, //top right
            1.0f, 1.0f //bot right
        };
        WMOGLM->bindVAO(SGL_OGL_CONSTANTS::SGL_POSTPROCESSOR_VAO);
        WMOGLM->bindVBO(SGL_OGL_CONSTANTS::SGL_POSTPROCESSOR_TEXTURE_UV_VBO);
        WMOGLM->bufferSubData(GL_ARRAY_BUFFER, NULL, sizeof(UV), &UV[0]);
    }

    WMOGLM->blending(true, BLENDING_TYPE::DEFAULT_RENDERING);
    activeShader.use(*WMOGLM);
    activeShader.setFloat(*WMOGLM, SGL_OGL_CONSTANTS::SHADER_UNIFORM_F_DELTA_TIME.c_str(), activeShader.renderDetails.deltaTime);
    activeShader.setFloat(*WMOGLM, SGL_OGL_CONSTANTS::SHADER_UNIFORM_F_TIME_ELAPSED.c_str(), activeShader.renderDetails.timeElapsed);
    activeShader.setVector2f(*WMOGLM, SGL_OGL_CONSTANTS::SHADER_UNIFORM_V2F_FBO_TEXTURE_DIMENSIONS.c_str(), pWidth, pHeight);
    activeShader.setVector2f(*WMOGLM, SGL_OGL_CONSTANTS::SHADER_UNIFORM_V2F_MOUSE_POSITION.c_str(), activeShader.renderDetails.mousePosX, activeShader.renderDetails.mousePosY);
    // activeShader.setInteger(*WMOGLM, "confuse", this->confuse);
    // activeShader.setInteger(*WMOGLM, "chaos", this->chaos);
    // activeShader.setInteger(*WMOGLM, "shake", this->shake);
    // Render textured quad
    WMOGLM->activeTexture(GL_TEXTURE0);
    this->pTexture.bind(*WMOGLM);
    WMOGLM->bindVAO(SGL_OGL_CONSTANTS::SGL_POSTPROCESSOR_VAO);
    WMOGLM->drawArrays(GL_TRIANGLES, 0, 6);
    WMOGLM->unbindVAO();
    activeShader.unbind(*WMOGLM);
}


/**
 * @brief Loads the OpenGl buffers, must be called before the FBO is functional
 *
 * @return nothing
 */
void SGL_PostProcessor::loadBuffers()
{
    WMOGLM->createVAO(SGL_OGL_CONSTANTS::SGL_POSTPROCESSOR_VAO);
    WMOGLM->createVBO(SGL_OGL_CONSTANTS::SGL_POSTPROCESSOR_VBO);
    WMOGLM->createVBO(SGL_OGL_CONSTANTS::SGL_POSTPROCESSOR_TEXTURE_UV_VBO);


    GLfloat vertices[] = {
        // Pos
        -1.0f, -1.0f,
        1.0f,  1.0f,
        -1.0f,  1.0f,

        -1.0f, -1.0f,
        1.0f, -1.0f,
        1.0f,  1.0f
    };


    GLfloat UVCoords[] = {
        0.0f, 0.0f, //top left
        1.0f, 1.0f, //bot right
        0.0f, 1.0f, //bot left

        0.0f, 0.0f, //top left
        1.0f, 0.0f, //top right
        1.0f, 1.0f //bot right
    };

    WMOGLM->bindVAO(SGL_OGL_CONSTANTS::SGL_POSTPROCESSOR_VAO);
    this->pShader.use(*WMOGLM);
    // UV coords buffer
    WMOGLM->bindVBO(SGL_OGL_CONSTANTS::SGL_POSTPROCESSOR_TEXTURE_UV_VBO);
    WMOGLM->bufferData(GL_ARRAY_BUFFER, sizeof(UVCoords), UVCoords, GL_STATIC_DRAW);
    // Vertex buffer
    WMOGLM->bindVBO(SGL_OGL_CONSTANTS::SGL_POSTPROCESSOR_VBO);
    WMOGLM->bufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // Link it to the shader
    // VBO
    WMOGLM->bindVBO(SGL_OGL_CONSTANTS::SGL_POSTPROCESSOR_VBO);
    WMOGLM->enableVertexAttribArray(0);
    WMOGLM->vertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GL_FLOAT), (GLvoid*)0);
    // UVCoords
    WMOGLM->bindVBO(SGL_OGL_CONSTANTS::SGL_POSTPROCESSOR_TEXTURE_UV_VBO);
    WMOGLM->enableVertexAttribArray(1);
    WMOGLM->vertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);

    WMOGLM->unbindVAO();
    WMOGLM->unbindVBO();
    this->pShader.unbind(*WMOGLM);
}
