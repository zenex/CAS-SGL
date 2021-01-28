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
 * @file    src/skeletonGL/utility/SGL_Renderer.cpp
 * @author  SENEX @ XENOBYTE.XYZ
 * @date    26/01/2021
 * @version 2.1
 *
 * @brief Renders to the currently bound post processor (FBO)
 *
 * @section DESCRIPTION
 *
 * This file defines all of the framework's rendering entities
 */

#include "SGL_Renderer.hpp"

/**
 * @brief Constructor
 *
 * @param oglm The window's OpenGL manager
 * @param texture Default texture to fallback on if a requested texture is not found
 * @param inivisibleTexture Completely transparent png image to be used as a canvas for the circle renderer
 * @param bmpTexture Default bmp font texture
 * @param pixelShader The default pixel shader
 * @param pixelBatchShader The default pixel batch shader
 * @param lineShader The default line shader
 * @param lineBatchShader The default line batch shader
 * @param circleShader The default circle shader
 * @param circleBatchShader The default circle batch shader
 * @param spriteShader The default sprite shader
 * @param spriteBatchShader The default sprite batch shader
 * @param textShader The default shader for the TTF text renderer
 * @return nothing
 */
SGL_Renderer::SGL_Renderer(std::shared_ptr<SGL_OpenGLManager> oglm,
                           const SGL_Texture texture, const SGL_Texture invisibleTexture,
                           const SGL_Texture bmpTexture,
                           const SGL_Shader &pixelShader, const SGL_Shader &pixelBatchShader,
                           const SGL_Shader &lineShader, const SGL_Shader &lineBatchShader,
                           const SGL_Shader &circleShader, const SGL_Shader &circleBatchShader,
                           const SGL_Shader &spriteShader, const SGL_Shader &spriteBatchShader,
                           const SGL_Shader &textShader)
{
    SGL_Log("Configuring renderer...", LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_DEFAULT);
    // Geometry shader configuration
    this->WMOGLM = oglm;
    this->pDefaultColor = {1.0f, 1.0f, 1.0f, 1.0f};
    this->pLineShader = lineShader;
    this->pPixelShader = pixelShader;
    this->pTextShader = textShader;
    this->pCircleShader = circleShader;
    this->pSpriteShader = spriteShader;
    this->pSpriteBatchShader = spriteBatchShader;
    this->pPixelBatchShader = pixelBatchShader;
    this->pLineBatchShader = lineBatchShader;
    this->pCircleBatchShader = circleBatchShader;

    // Default texture, this will be used in place of the sprtie's texture if this one is
    // invalid / empty, the AssetManager returns this same texture when queried for
    // a texture it can't find
    this->pDefaultTexture = texture;
    // Invisible texture, this fully transparent (alpha = 0.0f) texture is used by the circle renderers as
    // a blank canvas on which to draw the circle
    this->pInvisibleTexture = invisibleTexture;
    // Default texture to be used by the BMP font renderer
    this->pDefaultBMPFontTexture = bmpTexture;

    // OpenGL vertex arrays and vertex buffers objects
    // VAOs
    WMOGLM->createVAO(SGL_OGL_CONSTANTS::SGL_RENDERER_LINE_VAO);
    WMOGLM->createVAO(SGL_OGL_CONSTANTS::SGL_RENDERER_PIXEL_VAO);
    WMOGLM->createVAO(SGL_OGL_CONSTANTS::SGL_RENDERER_TEXT_VAO);
    WMOGLM->createVAO(SGL_OGL_CONSTANTS::SGL_RENDERER_SPRITE_VAO);
    WMOGLM->createVAO(SGL_OGL_CONSTANTS::SGL_RENDERER_SPRITE_BATCH_VAO);
    WMOGLM->createVAO(SGL_OGL_CONSTANTS::SGL_RENDERER_PIXEL_BATCH_VAO);
    WMOGLM->createVAO(SGL_OGL_CONSTANTS::SGL_RENDERER_LINE_BATCH_VAO);
    // VBO
    WMOGLM->createVBO(SGL_OGL_CONSTANTS::SGL_RENDERER_LINE_VBO);
    WMOGLM->createVBO(SGL_OGL_CONSTANTS::SGL_RENDERER_PIXEL_VBO);
    WMOGLM->createVBO(SGL_OGL_CONSTANTS::SGL_RENDERER_TEXT_VBO);
    WMOGLM->createVBO(SGL_OGL_CONSTANTS::SGL_RENDERER_SPRITE_VBO);
    WMOGLM->createVBO(SGL_OGL_CONSTANTS::SGL_RENDERER_SPRITE_BATCH_VBO);
    WMOGLM->createVBO(SGL_OGL_CONSTANTS::SGL_RENDERER_PIXEL_BATCH_VBO);
    WMOGLM->createVBO(SGL_OGL_CONSTANTS::SGL_RENDERER_LINE_BATCH_VBO);
    // Texture buffers: Simple buffer to hold UV data
    WMOGLM->createVBO(SGL_OGL_CONSTANTS::SGL_RENDERER_TEXTURE_UV_VBO);

    // Instancing buffers: These VBO are used to allocate the memory required to
    // fit the batch / instances to be rendered, this same buffer is then recycled
    // with the actual data to be stored in the previously allocated GPU memory.
    WMOGLM->createVBO(SGL_OGL_CONSTANTS::SGL_RENDERER_PIXEL_BATCH_INSTANCES_VBO);
    WMOGLM->createVBO(SGL_OGL_CONSTANTS::SGL_RENDERER_LINE_BATCH_INSTANCES_VBO);
    WMOGLM->createVBO(SGL_OGL_CONSTANTS::SGL_RENDERER_SPRITE_BATCH_INSTANCES_VBO);

    // Load the object data
    this->loadLineBuffers(pLineShader);
    this->loadPointBuffers(pPixelShader);
    this->loadSpriteBuffers(pSpriteShader);
    this->loadPixelBatchBuffers(pPixelBatchShader);
    this->loadSpriteBatchBuffers(pSpriteBatchShader);
    this->loadLineBatchBuffers(pLineBatchShader);

    this->generateBitmapFont();
    // Generate the charset from the provided TTF font
    this->generateTTFFont(FOLDER_STRUCTURE::DEFAULT_TTF_FONT_FILE);
}

/**
 * @brief Destructor
 *
 * @return nothing
 */
SGL_Renderer::~SGL_Renderer()
{
    // VAOs
    WMOGLM->deleteVAO(SGL_OGL_CONSTANTS::SGL_RENDERER_LINE_VAO);
    WMOGLM->deleteVAO(SGL_OGL_CONSTANTS::SGL_RENDERER_PIXEL_VAO);
    WMOGLM->deleteVAO(SGL_OGL_CONSTANTS::SGL_RENDERER_TEXT_VAO);
    WMOGLM->deleteVAO(SGL_OGL_CONSTANTS::SGL_RENDERER_SPRITE_VAO);
    WMOGLM->deleteVAO(SGL_OGL_CONSTANTS::SGL_RENDERER_SPRITE_BATCH_VAO);
    WMOGLM->deleteVAO(SGL_OGL_CONSTANTS::SGL_RENDERER_PIXEL_BATCH_VAO);
    WMOGLM->deleteVAO(SGL_OGL_CONSTANTS::SGL_RENDERER_LINE_BATCH_VAO);

    // VBOs
    WMOGLM->deleteVBO(SGL_OGL_CONSTANTS::SGL_RENDERER_LINE_VBO);
    WMOGLM->deleteVBO(SGL_OGL_CONSTANTS::SGL_RENDERER_PIXEL_VBO);
    WMOGLM->deleteVBO(SGL_OGL_CONSTANTS::SGL_RENDERER_TEXT_VBO);
    WMOGLM->deleteVBO(SGL_OGL_CONSTANTS::SGL_RENDERER_TEXTURE_UV_VBO);
    WMOGLM->deleteVBO(SGL_OGL_CONSTANTS::SGL_RENDERER_SPRITE_VBO);
    WMOGLM->deleteVBO(SGL_OGL_CONSTANTS::SGL_RENDERER_SPRITE_BATCH_VBO);
    WMOGLM->deleteVBO(SGL_OGL_CONSTANTS::SGL_RENDERER_PIXEL_BATCH_VBO);
    WMOGLM->deleteVBO(SGL_OGL_CONSTANTS::SGL_RENDERER_LINE_BATCH_VBO);
    WMOGLM->deleteVBO(SGL_OGL_CONSTANTS::SGL_RENDERER_PIXEL_BATCH_INSTANCES_VBO);
    WMOGLM->deleteVBO(SGL_OGL_CONSTANTS::SGL_RENDERER_LINE_BATCH_INSTANCES_VBO);
    WMOGLM->deleteVBO(SGL_OGL_CONSTANTS::SGL_RENDERER_SPRITE_BATCH_INSTANCES_VBO);
}

/**
 * @brief Renders a pixel
 * @param pixel The Pixel object to render
 *
 * @return nothing
 */
void SGL_Renderer::renderPixel(const SGL_Pixel &pixel) const
{
    SGL_Shader activeShader;
    if (pixel.shader.shaderType != SHADER_TYPE::PIXEL)
        activeShader = pPixelShader;
    else
        activeShader = pixel.shader;

    // NOTE, coords are normalized, gotta deal with that
    GLfloat vertices[] = {
        pixel.position.x, pixel.position.y
    };
    WMOGLM->bindVAO(SGL_OGL_CONSTANTS::SGL_RENDERER_PIXEL_VAO);
    WMOGLM->bindVBO(SGL_OGL_CONSTANTS::SGL_RENDERER_PIXEL_VBO);
    activeShader.use(*WMOGLM);

    WMOGLM->bufferSubData(GL_ARRAY_BUFFER, NULL, sizeof(vertices), &vertices[0]);
    glm::vec4 assignedColor = {pixel.color.r, pixel.color.g, pixel.color.b, pixel.color.a};
    activeShader.setVector4f(*WMOGLM, SGL_OGL_CONSTANTS::SHADER_UNIFORM_V4F_COLOR.c_str(), assignedColor);
    activeShader.setFloat(*WMOGLM, SGL_OGL_CONSTANTS::SHADER_UNIFORM_F_DELTA_TIME.c_str(), activeShader.renderDetails.deltaTime);
    activeShader.setFloat(*WMOGLM, SGL_OGL_CONSTANTS::SHADER_UNIFORM_F_TIME_ELAPSED.c_str(), activeShader.renderDetails.timeElapsed);
    activeShader.setVector2f(*WMOGLM, SGL_OGL_CONSTANTS::SHADER_UNIFORM_V2F_WINDOW_DIMENSIONS.c_str(), activeShader.renderDetails.canvasWidth, activeShader.renderDetails.canvasHeight);

    // Pixel width, if AA is enabled it must be set to 1.0f!
    if ( pixel.size > SGL_OGL_CONSTANTS::MIN_PIXEL_SIZE && pixel.size < SGL_OGL_CONSTANTS::MAX_PIXEL_SIZE )
        WMOGLM->pixelSize(pixel.size);
    else
        WMOGLM->pixelSize(SGL_OGL_CONSTANTS::MIN_PIXEL_SIZE);

    // Set the blending mode
    if (pixel.blending == 0)
        WMOGLM->blending(true, BLENDING_TYPE::DEFAULT_RENDERING);
    else
        WMOGLM->blending(true, pixel.blending);


    WMOGLM->drawArrays(GL_POINTS, 0, 2);

    // Cleanup
    WMOGLM->pixelSize(SGL_OGL_CONSTANTS::MIN_PIXEL_SIZE);
    WMOGLM->blending(true, BLENDING_TYPE::DEFAULT_RENDERING);
    WMOGLM->unbindVAO();
    WMOGLM->unbindVBO();
    activeShader.unbind(*WMOGLM);

#ifdef SGL_CHECK_FOR_ERRORS_ON_RENDER
    WMOGLM->checkForGLErrors();
#endif
}


/**
 * @brief Renders a pixel
 * @param x1 X position of the pixel
 * @param y1 Y position of the pixel
 * @param scale The pixel's size
 * @param color The pixel's color
 *
 * @return nothing
 */
void SGL_Renderer::renderPixel(float x1, float y1, float scale, SGL_Color color)
{
    SGL_Pixel p;
    p.position.x = x1;
    p.position.y = y1;
    p.size = scale;
    p.color = color;
    p.blending = BLENDING_TYPE::DEFAULT_RENDERING;
    p.shader = pPixelShader;
    renderPixel(p);
}

/**
 * @brief Renders a line
 * @param line SGL_Line to render
 *
 * @return nothing
 */
void SGL_Renderer::renderLine(const SGL_Line &line) const
{
    SGL_Shader activeShader;
    if (line.shader.shaderType != SHADER_TYPE::LINE)
        activeShader = pLineShader;
    else
        activeShader = line.shader;

    //NOTE, coords are normalized, gotta deal with that
    GLfloat vertices[] = {
        line.positionA.x, line.positionA.y, line.positionB.x, line.positionB.y
    };
    WMOGLM->bindVAO(SGL_OGL_CONSTANTS::SGL_RENDERER_LINE_VAO);
    WMOGLM->bindVBO(SGL_OGL_CONSTANTS::SGL_RENDERER_LINE_VBO);
    activeShader.use(*WMOGLM);

    WMOGLM->bufferSubData(GL_ARRAY_BUFFER, NULL, sizeof(vertices), &vertices[0]);
    glm::vec4 assignedColor = {line.color.r, line.color.g, line.color.b, line.color.a};
    activeShader.setVector4f(*WMOGLM, SGL_OGL_CONSTANTS::SHADER_UNIFORM_V4F_COLOR.c_str(), assignedColor);
    activeShader.setFloat(*WMOGLM, SGL_OGL_CONSTANTS::SHADER_UNIFORM_F_DELTA_TIME.c_str(), activeShader.renderDetails.deltaTime);
    activeShader.setFloat(*WMOGLM, SGL_OGL_CONSTANTS::SHADER_UNIFORM_F_TIME_ELAPSED.c_str(), activeShader.renderDetails.timeElapsed);
    activeShader.setVector2f(*WMOGLM, SGL_OGL_CONSTANTS::SHADER_UNIFORM_V2F_WINDOW_DIMENSIONS.c_str(), activeShader.renderDetails.canvasWidth, activeShader.renderDetails.canvasHeight);

    // Line width, if AA is enabled it must be set to 1.0f!
    if ( line.width > SGL_OGL_CONSTANTS::MIN_LINE_WIDTH && line.width < SGL_OGL_CONSTANTS::MAX_LINE_WIDTH )
        glLineWidth(line.width);
    else
        glLineWidth(SGL_OGL_CONSTANTS::MIN_LINE_WIDTH);

    // Set the blending mode
    if (line.blending == 0)
        WMOGLM->blending(true, BLENDING_TYPE::DEFAULT_RENDERING);
    else
        WMOGLM->blending(true, line.blending);


    WMOGLM->drawArrays(GL_LINES, 0, 2);

    // Reset line width
    glLineWidth(SGL_OGL_CONSTANTS::MIN_LINE_WIDTH);
    WMOGLM->blending(true, BLENDING_TYPE::DEFAULT_RENDERING);
    WMOGLM->unbindVAO();
    WMOGLM->unbindVBO();
    activeShader.unbind(*WMOGLM);

#ifdef SGL_CHECK_FOR_ERRORS_ON_RENDER
    WMOGLM->checkForGLErrors();
#endif

}

/**
 * @brief Renders a pixel
 * @param x1 First vector X position
 * @param y1 First vector Y position
 * @param x2 Second vector X position
 * @param y2 Second vector Y position
 * @param color The line's color
 *
 * @return nothing
 */
void SGL_Renderer::renderLine(float x1, float y1, float x2, float y2, float width, SGL_Color color)
{
    SGL_Line l;
    l.positionA.x = x1;
    l.positionA.y = y1;
    l.positionB.x = x2;
    l.positionB.y = y2;
    l.shader = pLineShader;
    l.blending = BLENDING_TYPE::DEFAULT_RENDERING;
    l.width = width;
    l.color = color;
    renderLine(l);
}

/**
 * @brief Renders a string of text
 * @param line SGL_Text object to render
 *
 * @return nothing
 */
void SGL_Renderer::renderText(SGL_Text &text)
{
    SGL_Shader activeShader;
    if (text.shader.shaderType != SHADER_TYPE::TEXT)
        activeShader = pTextShader;
    else
        activeShader = text.shader;

    WMOGLM->faceCulling(true);
    // RESUME render state
    activeShader.use(*WMOGLM);
    glm::vec4 assignedColor = {text.color.r, text.color.g, text.color.b, text.color.a};
    activeShader.setVector4f(*WMOGLM, "textColor", assignedColor);
    activeShader.setFloat(*WMOGLM, "deltaTime", activeShader.renderDetails.deltaTime);
    WMOGLM->activeTexture(GL_TEXTURE0);
    WMOGLM->bindVAO(SGL_OGL_CONSTANTS::SGL_RENDERER_TEXT_VAO);

    // Iterate through all the characters in the map
    std::string::const_iterator c;
    for(c = text.message.begin(); c != text.message.end(); ++c)
    {
        Character ch = characters[*c];

        GLfloat xpos = text.position.x + ch.bearing.x * text.scale;
        // This y pos works when you do NOT flip the y axis in the ortho projection matrix
        // GLfloat ypos = y - (ch.size.y - ch.bearing.y) * scale;
        // this one does
        GLfloat ypos = text.position.y + (this->characters['H'].bearing.y - ch.bearing.y) * text.scale;

        GLfloat w = ch.size.x * text.scale;
        GLfloat h = ch.size.y * text.scale;

        // Update VBO for each character
        GLfloat vertices[6][4] = {
            { xpos,     ypos + h,   0.0, 1.0 },
            { xpos + w, ypos,       1.0, 0.0 },
            { xpos,     ypos,       0.0, 0.0 },

            { xpos,     ypos + h,   0.0, 1.0 },
            { xpos + w, ypos + h,   1.0, 1.0 },
            { xpos + w, ypos,       1.0, 0.0 }
        };
        // Render glyph texture over the quad
        WMOGLM->bindTexture(GL_TEXTURE_2D, ch.textureID);
        // Update content of VBO memory
        WMOGLM->bindVBO(SGL_OGL_CONSTANTS::SGL_RENDERER_TEXT_VBO);
        WMOGLM->bufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        WMOGLM->unbindVBO();
        // Render the quad
        WMOGLM->drawArrays(GL_TRIANGLES, 0, 6);
        // Advance cursor for the next glyph (note that advance is number of 1/64 pixels)
        text.position.x += (ch.advance >> 6) * text.scale;
    }

    WMOGLM->unbindVAO();
    WMOGLM->unbindVBO();
    activeShader.unbind(*WMOGLM);
    WMOGLM->bindTexture(GL_TEXTURE_2D, 0);
    WMOGLM->faceCulling(false);

#ifdef SGL_CHECK_FOR_ERRORS_ON_RENDER
    WMOGLM->checkForGLErrors();
#endif
}

/**
 * @brief Renders a string of TTF-generated text
 * @param text String to render
 * @param x Vvector Y position
 * @param y Vector Y position
 * @param scale Float that sets the size
 * @param color The text's color
 *
 * @return nothing
 */
void SGL_Renderer::renderText(std::string text, GLfloat x, GLfloat y, GLfloat scale, SGL_Color color)
{
    WMOGLM->faceCulling(true);
    // RESUME render state
    this->pTextShader.use(*WMOGLM);
    this->pTextShader.setVector4f(*WMOGLM, SGL_OGL_CONSTANTS::SHADER_UNIFORM_V4F_COLOR.c_str(), color.r, color.g, color.b, color.a);
    this->pTextShader.setFloat(*WMOGLM, SGL_OGL_CONSTANTS::SHADER_UNIFORM_F_DELTA_TIME.c_str(), this->pTextShader.renderDetails.deltaTime);
    this->pTextShader.setFloat(*WMOGLM, SGL_OGL_CONSTANTS::SHADER_UNIFORM_F_TIME_ELAPSED.c_str(), this->pTextShader.renderDetails.timeElapsed);
    this->pTextShader.setVector2f(*WMOGLM, SGL_OGL_CONSTANTS::SHADER_UNIFORM_V2F_WINDOW_DIMENSIONS.c_str(), this->pTextShader.renderDetails.canvasWidth, this->pTextShader.renderDetails.canvasHeight);

    WMOGLM->activeTexture(GL_TEXTURE0);
    WMOGLM->bindVAO(SGL_OGL_CONSTANTS::SGL_RENDERER_TEXT_VAO);

    // Iterate through all the characters in the map
    std::string::const_iterator c;
    for(c = text.begin(); c != text.end(); ++c)
    {
        Character ch = characters[*c];

        GLfloat xpos = x + ch.bearing.x * scale;
        // This y pos works when you do NOT flip the y axis in the ortho projection matrix
        // GLfloat ypos = y - (ch.size.y - ch.bearing.y) * scale;
        // this one does
        GLfloat ypos = y + (this->characters['H'].bearing.y - ch.bearing.y) * scale;

        GLfloat w = ch.size.x * scale;
        GLfloat h = ch.size.y * scale;

        // Update VBO for each character
        GLfloat vertices[6][4] = {
            { xpos,     ypos + h,   0.0, 1.0 },
            { xpos + w, ypos,       1.0, 0.0 },
            { xpos,     ypos,       0.0, 0.0 },

            { xpos,     ypos + h,   0.0, 1.0 },
            { xpos + w, ypos + h,   1.0, 1.0 },
            { xpos + w, ypos,       1.0, 0.0 }
        };
        // Render glyph texture over the quad
        WMOGLM->bindTexture(GL_TEXTURE_2D, ch.textureID);
        // Update content of VBO memory
        WMOGLM->bindVBO(SGL_OGL_CONSTANTS::SGL_RENDERER_TEXT_VBO);
        WMOGLM->bufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        WMOGLM->unbindVBO();
        // Render the quad
        WMOGLM->drawArrays(GL_TRIANGLES, 0, 6);
        // Advance cursor for the next glyph (note that advance is number of 1/64 pixels)
        x += (ch.advance >> 6) * scale;
    }

    WMOGLM->unbindVAO();
    WMOGLM->unbindVBO();
    this->pTextShader.unbind(*WMOGLM);
    WMOGLM->bindTexture(GL_TEXTURE_2D, 0);
    WMOGLM->faceCulling(false);

#ifdef SGL_CHECK_FOR_ERRORS_ON_RENDER
    WMOGLM->checkForGLErrors();
#endif

}

/**
 * @brief Renders a string of BMP-generated text
 * @param text String to render
 * @param x Vvector Y position
 * @param y Vector Y position
 * @param scale Float that sets the size
 * @param color The text's color
 *
 * @return nothing
 */
void SGL_Renderer::renderBitmapText(std::string text, GLfloat x, GLfloat y, GLfloat scale, SGL_Color color) const
{
    SGL_Bitmap_Text t;
    t.text = text;
    t.position.x = x;
    t.position.y = y;
    t.scale = scale;
    t.color = color;
    t.texture = pDefaultBMPFontTexture;
    t.blending = BLENDING_TYPE::DEFAULT_RENDERING;
    renderBitmapText(t);
}

/**
 * @brief Renders a string of text as a bitmap font
 * @param line SGL_Text object to render
 *
 * @return nothing
 */
void SGL_Renderer::renderBitmapText(SGL_Bitmap_Text &text) const
{
    // Get the UV positioning for the character to rener
    SGL_Sprite s;
    s.position.x = text.position.x;
    s.position.y = text.position.y;
    s.size.x = text.scale;
    s.size.y = text.scale;
    s.texture = text.texture;
    s.color = text.color;
    s.blending = text.blending;
    // s.rotationOrigin.x = 5;
    // s.rotationOrigin.y = 5;
    // s.rotation = 2.0;

    // SGL_Log("Total chars in text to render: " + std::to_string(text.text.size()));
    std::string str = "";
    // Iterate through all the characters in the string to render
    for (const char &c : text.text)
    {
        // Get UV coords
        glm::vec4 uvC = pBitmapCharacters.find(c)->second;
        s.changeUVCoords(uvC.x, uvC.y, uvC.w, uvC.z);
        // SGL_Log("X: " + std::to_string(uvC.x) + " Y: " + std::to_string(uvC.y) + " W: " + std::to_string(uvC.w) + " H: " + std::to_string(uvC.z));
        this->renderSprite(s);

        str.push_back(c);
        // SGL_Log("CHAR: " + str);

        // Move the X position by the width of the last rendered character
        s.position.x += s.size.x;
    }
}


void SGL_Renderer::renderCircle(float x, float y, float radius, float width, SGL_Color color)
{
    SGL_Circle c;
    c.shader = pCircleShader;
    c.shader.renderDetails.circleBorder = width;
    c.position.x = x;
    c.position.y = y;
    c.radius = radius;
    c.blending = BLENDING_TYPE::DEFAULT_RENDERING;
    c.color = color;
    renderCircle(c);
}


void SGL_Renderer::renderCircle(const SGL_Circle &circle) const
{
    SGL_Sprite c;
    // Do this check here before passing the sprite to the sprite renderer, otherwise if the incorrect shader type is passed
    // i.e. SHADER_TYPE::SPRITE_BATCH it will fallback to the default circle shader instead of the default sprite shader
    if (circle.shader.shaderType != SHADER_TYPE::SPRITE)
        c.shader = pCircleShader;
    // Normalize the circle line width between 0.010f and 1.0 (1.0 being a filled circle)
    if (circle.shader.renderDetails.circleBorder < SGL_OGL_CONSTANTS::MIN_CIRCLE_WIDTH)
        c.shader.renderDetails.circleBorder = SGL_OGL_CONSTANTS::MIN_CIRCLE_WIDTH;
    if (circle.shader.renderDetails.circleBorder > SGL_OGL_CONSTANTS::MAX_CIRCLE_WIDTH)
        c.shader.renderDetails.circleBorder = SGL_OGL_CONSTANTS::MAX_CIRCLE_WIDTH;

    c.shader = circle.shader;
    c.texture = pInvisibleTexture;
    c.resetUVCoords();
    c.position = circle.position;
    c.color = circle.color;
    c.blending = circle.blending;
    c.size.x = circle.radius;
    c.size.y = circle.radius;
    renderSprite(c);
}

/**
 * @brief Renders a sprite
 * @param sprite SGL_Sprite to be rendered
 *
 * @return nothing
 */
void SGL_Renderer::renderSprite(const SGL_Sprite &sprite) const
{
    SGL_Shader activeShader;
    if (sprite.shader.shaderType != SHADER_TYPE::SPRITE)
        activeShader = pSpriteShader;
    else
        activeShader = sprite.shader;

    SGL_Texture activeTexture;
    if (sprite.texture.width == 0) // Uninitialized texture
        activeTexture = pDefaultTexture;
    else
        activeTexture = sprite.texture;

    GLfloat UV[] = {
        sprite.uvCoords.UV_topLeft.x / sprite.texture.width, sprite.uvCoords.UV_topLeft.y / sprite.texture.height,
        sprite.uvCoords.UV_botRight.x / sprite.texture.width, sprite.uvCoords.UV_botRight.y / sprite.texture.height,
        sprite.uvCoords.UV_botLeft.x / sprite.texture.width, sprite.uvCoords.UV_botLeft.y / sprite.texture.height,

        sprite.uvCoords.UV_topLeft.x / sprite.texture.width, sprite.uvCoords.UV_topLeft.y / sprite.texture.height,
        sprite.uvCoords.UV_topRight.x / sprite.texture.width, sprite.uvCoords.UV_topRight.y / sprite.texture.height,
        sprite.uvCoords.UV_botRight.x / sprite.texture.width, sprite.uvCoords.UV_botRight.y / sprite.texture.height,
    };

    WMOGLM->bindVAO(SGL_OGL_CONSTANTS::SGL_RENDERER_SPRITE_VAO);
    activeShader.use(*WMOGLM);

    // Replace current texture coordinates
    WMOGLM->bindVBO(SGL_OGL_CONSTANTS::SGL_RENDERER_TEXTURE_UV_VBO);
    WMOGLM->bufferSubData(GL_ARRAY_BUFFER, NULL, sizeof(UV), &UV[0]);

    // If the user didn't specify a blending mode use the renderers default
    if (sprite.blending == 0)
        WMOGLM->blending(true, BLENDING_TYPE::DEFAULT_RENDERING);
    else
        WMOGLM->blending(true, sprite.blending);

    // Prepare transformations
    glm::mat4 model(1.0f);
    model = glm::translate(model, glm::vec3(sprite.position, 0.0f)); //move
    // Move origin of rotation specified in sprite's data
    model = glm::translate(model, glm::vec3(sprite.rotationOrigin.x, sprite.rotationOrigin.y, 0.0f));
    model = glm::rotate(model, sprite.rotation, glm::vec3(0.0f, 0.0f, 1.0f));//rotate
    model = glm::translate(model, glm::vec3(-sprite.rotationOrigin.x, -sprite.rotationOrigin.y, 0.0f)); //set the rotation origin back to origin
    model = glm::scale(model, glm::vec3(sprite.size, 1.0f)); //scale

    // Parse uniforms
    activeShader.setMatrix4(*WMOGLM, SGL_OGL_CONSTANTS::SHADER_UNIFORM_M4F_MODEL.c_str(), model);
    activeShader.setVector4f(*WMOGLM, SGL_OGL_CONSTANTS::SHADER_UNIFORM_V4F_COLOR.c_str(), sprite.color.r, sprite.color.g, sprite.color.b, sprite.color.a);
    activeShader.setFloat(*WMOGLM, SGL_OGL_CONSTANTS::SHADER_UNIFORM_F_DELTA_TIME.c_str(), activeShader.renderDetails.deltaTime);
    activeShader.setFloat(*WMOGLM, SGL_OGL_CONSTANTS::SHADER_UNIFORM_F_TIME_ELAPSED.c_str(), activeShader.renderDetails.timeElapsed);
    activeShader.setVector2f(*WMOGLM, SGL_OGL_CONSTANTS::SHADER_UNIFORM_V2F_WINDOW_DIMENSIONS.c_str(), activeShader.renderDetails.canvasWidth, activeShader.renderDetails.canvasHeight);
    // Circles are just transparent textures used as canvases, just parse it fuck it
    activeShader.setFloat(*WMOGLM, SGL_OGL_CONSTANTS::SHADER_UNIFORM_F_CIRCLE_BORDER_WIDTH.c_str(), activeShader.renderDetails.circleBorder);

    // Activate the texture
    WMOGLM->activeTexture(GL_TEXTURE0);
    activeTexture.bind(*WMOGLM);

    WMOGLM->drawArrays(GL_TRIANGLES, 0, 6);

    // Cleanup
    activeShader.unbind(*WMOGLM);
    WMOGLM->unbindVAO();
    WMOGLM->unbindVBO();
    WMOGLM->blending(true, BLENDING_TYPE::DEFAULT_RENDERING);

#ifdef SGL_CHECK_FOR_ERRORS_ON_RENDER
    WMOGLM->checkForGLErrors();
#endif
}

/**
 * @brief Renders a sprite batch using instanced rendering
 * @param sprite SGL_Sprite to be rendered
 * @param sprite SGL_Sprite to be rendered
 *
 * @return nothing
 */
void SGL_Renderer::renderSpriteBatch(const SGL_Sprite &sprite, const std::vector<glm::mat4> *matrices)
{
    SGL_Shader activeShader;
    if (sprite.shader.shaderType != SHADER_TYPE::SPRITE_BATCH)
        activeShader = pSpriteBatchShader;
    else
        activeShader = sprite.shader;

    // activeShader = pSpriteBatchShader;

    GLfloat UV[] = {
        sprite.uvCoords.UV_topLeft.x / sprite.texture.width, sprite.uvCoords.UV_topLeft.y / sprite.texture.height,
        sprite.uvCoords.UV_botRight.x / sprite.texture.width, sprite.uvCoords.UV_botRight.y / sprite.texture.height,
        sprite.uvCoords.UV_botLeft.x / sprite.texture.width, sprite.uvCoords.UV_botLeft.y / sprite.texture.height,

        sprite.uvCoords.UV_topLeft.x / sprite.texture.width, sprite.uvCoords.UV_topLeft.y / sprite.texture.height,
        sprite.uvCoords.UV_topRight.x / sprite.texture.width, sprite.uvCoords.UV_topRight.y / sprite.texture.height,
        sprite.uvCoords.UV_botRight.x / sprite.texture.width, sprite.uvCoords.UV_botRight.y / sprite.texture.height,
    };

    // Activate VAO & load shader
    WMOGLM->bindVAO(SGL_OGL_CONSTANTS::SGL_RENDERER_SPRITE_BATCH_VAO);
    activeShader.use(*WMOGLM);

    // Replace current texture coordinates
    WMOGLM->bindVBO(SGL_OGL_CONSTANTS::SGL_RENDERER_TEXTURE_UV_VBO);
    WMOGLM->bufferSubData(GL_ARRAY_BUFFER, NULL, sizeof(UV), &UV[0]);

    // If the user didn't specify a blending mode use the renderers default
    if (sprite.blending == 0)
        WMOGLM->blending(true, BLENDING_TYPE::DEFAULT_RENDERING);
    else
        WMOGLM->blending(true, sprite.blending);

    // Update the batch data buffer
    WMOGLM->bindVBO(SGL_OGL_CONSTANTS::SGL_RENDERER_SPRITE_BATCH_INSTANCES_VBO);
    WMOGLM->bufferSubData(GL_ARRAY_BUFFER, NULL, sizeof(glm::mat4) * matrices->size(), matrices->data());
    WMOGLM->unbindVBO();

    // Parse uniforms
    activeShader.setVector4f(*WMOGLM, SGL_OGL_CONSTANTS::SHADER_UNIFORM_V4F_COLOR.c_str(), sprite.color.r, sprite.color.g, sprite.color.b, sprite.color.a);
    activeShader.setFloat(*WMOGLM, SGL_OGL_CONSTANTS::SHADER_UNIFORM_F_DELTA_TIME.c_str(), activeShader.renderDetails.deltaTime);
    activeShader.setFloat(*WMOGLM, SGL_OGL_CONSTANTS::SHADER_UNIFORM_F_TIME_ELAPSED.c_str(), activeShader.renderDetails.timeElapsed);
    activeShader.setVector2f(*WMOGLM, SGL_OGL_CONSTANTS::SHADER_UNIFORM_V2F_WINDOW_DIMENSIONS.c_str(), activeShader.renderDetails.canvasWidth, activeShader.renderDetails.canvasHeight);
    // Circles are just transparent textures used as canvases, just parse it fuck it
    activeShader.setFloat(*WMOGLM, SGL_OGL_CONSTANTS::SHADER_UNIFORM_F_CIRCLE_BORDER_WIDTH.c_str(), activeShader.renderDetails.circleBorder);

    // Activate textures
    WMOGLM->activeTexture(GL_TEXTURE0);
    sprite.texture.bind(*WMOGLM);

    WMOGLM->drawArraysInstanced(GL_TRIANGLES, 0, 6, matrices->size());

    // Cleanup
    activeShader.unbind(*WMOGLM);
    WMOGLM->unbindVAO();
    WMOGLM->unbindVBO();
    WMOGLM->blending(true, BLENDING_TYPE::DEFAULT_RENDERING);

#ifdef SGL_CHECK_FOR_ERRORS_ON_RENDER
    WMOGLM->checkForGLErrors();
#endif

}



void SGL_Renderer::renderLineBatch(const SGL_Line &line, const std::vector<glm::vec2> *vectors)
{
    SGL_Shader activeShader;
    if (line.shader.shaderType != SHADER_TYPE::LINE_BATCH)
        activeShader = pLineBatchShader;
    else
        activeShader = line.shader;

    WMOGLM->bindVAO(SGL_OGL_CONSTANTS::SGL_RENDERER_LINE_BATCH_VAO);
    activeShader.use(*WMOGLM);

    // Update the VBO position data
    GLfloat vertices[] = { line.positionA.x, line.positionA.y, line.positionB.x, line.positionB.y };
    WMOGLM->bindVBO(SGL_OGL_CONSTANTS::SGL_RENDERER_LINE_BATCH_VBO);
    WMOGLM->bufferSubData(GL_ARRAY_BUFFER, NULL, sizeof(vertices), &vertices[0]);

    // Update the batch list
    WMOGLM->bindVBO(SGL_OGL_CONSTANTS::SGL_RENDERER_LINE_BATCH_INSTANCES_VBO);
    WMOGLM->bufferSubData(GL_ARRAY_BUFFER, NULL, sizeof(glm::vec4) * vectors->size(), vectors->data());
    WMOGLM->unbindVBO();

    // Set shader uniforms
    glm::vec4 assignedColor = {line.color.r, line.color.g, line.color.b, line.color.a};
    activeShader.setVector4f(*WMOGLM, SGL_OGL_CONSTANTS::SHADER_UNIFORM_V4F_COLOR.c_str(), assignedColor);
    activeShader.setFloat(*WMOGLM, SGL_OGL_CONSTANTS::SHADER_UNIFORM_F_DELTA_TIME.c_str(), activeShader.renderDetails.deltaTime);
    activeShader.setFloat(*WMOGLM, SGL_OGL_CONSTANTS::SHADER_UNIFORM_F_TIME_ELAPSED.c_str(), activeShader.renderDetails.timeElapsed);
    activeShader.setVector2f(*WMOGLM, SGL_OGL_CONSTANTS::SHADER_UNIFORM_V2F_WINDOW_DIMENSIONS.c_str(), activeShader.renderDetails.canvasWidth, activeShader.renderDetails.canvasHeight);

    // Line width, if AA is enabled it must be set to 1.0f!
    if ( line.width > SGL_OGL_CONSTANTS::MIN_LINE_WIDTH && line.width < SGL_OGL_CONSTANTS::MAX_LINE_WIDTH )
        WMOGLM->lineWidth(line.width);
    else
        WMOGLM->lineWidth(SGL_OGL_CONSTANTS::MIN_LINE_WIDTH);

    // Set the blending mode
    if (line.blending == 0)
        WMOGLM->blending(true, BLENDING_TYPE::DEFAULT_RENDERING);
    else
        WMOGLM->blending(true, line.blending);

    // Render instanced data
    WMOGLM->drawArraysInstanced(GL_LINES, 0, 2, vectors->size());

    // Cleanup
    WMOGLM->lineWidth(SGL_OGL_CONSTANTS::MIN_LINE_WIDTH);
    WMOGLM->blending(true, BLENDING_TYPE::DEFAULT_RENDERING);
    WMOGLM->unbindVAO();
    WMOGLM->unbindVBO();
    activeShader.unbind(*WMOGLM);

#ifdef SGL_CHECK_FOR_ERRORS_ON_RENDER
    WMOGLM->checkForGLErrors();
#endif
}


void SGL_Renderer::renderPixelBatch(const SGL_Pixel &pixel, const std::vector<glm::vec2> *vectors)
{
    SGL_Shader activeShader;
    if (pixel.shader.shaderType != SHADER_TYPE::PIXEL_BATCH)
        activeShader = pPixelBatchShader;
    else
        activeShader = pixel.shader;

    WMOGLM->bindVAO(SGL_OGL_CONSTANTS::SGL_RENDERER_PIXEL_BATCH_VAO);
    // WMOGLM->bindVBO(this->pPixelBatchVBO);
    activeShader.use(*WMOGLM);

    // Update batch data
    WMOGLM->bindVBO(SGL_OGL_CONSTANTS::SGL_RENDERER_PIXEL_BATCH_INSTANCES_VBO);
    WMOGLM->bufferSubData(GL_ARRAY_BUFFER, NULL, sizeof(glm::vec2) * vectors->size(), vectors->data());
    WMOGLM->unbindVBO();

    // Set shader uniforms
    glm::vec4 assignedColor = {pixel.color.r, pixel.color.g, pixel.color.b, pixel.color.a};
    activeShader.setVector4f(*WMOGLM, SGL_OGL_CONSTANTS::SHADER_UNIFORM_V4F_COLOR.c_str(), assignedColor);
    activeShader.setFloat(*WMOGLM, SGL_OGL_CONSTANTS::SHADER_UNIFORM_F_DELTA_TIME.c_str(), activeShader.renderDetails.deltaTime);
    activeShader.setFloat(*WMOGLM, SGL_OGL_CONSTANTS::SHADER_UNIFORM_F_TIME_ELAPSED.c_str(), activeShader.renderDetails.timeElapsed);
    activeShader.setVector2f(*WMOGLM, SGL_OGL_CONSTANTS::SHADER_UNIFORM_V2F_WINDOW_DIMENSIONS.c_str(), activeShader.renderDetails.canvasWidth, activeShader.renderDetails.canvasHeight);


    // Pixel width, if AA is enabled it must be set to 1.0f!
    if ( pixel.size > SGL_OGL_CONSTANTS::MIN_PIXEL_SIZE && pixel.size < SGL_OGL_CONSTANTS::MAX_PIXEL_SIZE )
        WMOGLM->pixelSize(pixel.size);
    else
        WMOGLM->pixelSize(SGL_OGL_CONSTANTS::MIN_PIXEL_SIZE);

    // Set the blending mode
    if (pixel.blending == 0)
        WMOGLM->blending(true, BLENDING_TYPE::DEFAULT_RENDERING);
    else
        WMOGLM->blending(true, pixel.blending);


    // Render instanced data
    WMOGLM->drawArraysInstanced(GL_POINTS, 0, 2, vectors->size());

    // Cleanup
    WMOGLM->unbindVAO();
    WMOGLM->unbindVBO();
    WMOGLM->pixelSize(SGL_OGL_CONSTANTS::MIN_PIXEL_SIZE);
    WMOGLM->blending(true, BLENDING_TYPE::DEFAULT_RENDERING);
    activeShader.unbind(*WMOGLM);

#ifdef SGL_CHECK_FOR_ERRORS_ON_RENDER
    WMOGLM->checkForGLErrors();
#endif
}

/**
 * @brief Load the line buffers
 * @param shader SGL_Shader to set as the line renderer default
 *
 * @return nothing
 */
void SGL_Renderer::loadLineBuffers(SGL_Shader shader) noexcept
{
    SGL_Log("Configuring the line renderer...", LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_DEFAULT);
    shader.use(*WMOGLM);

    GLfloat vertices[] = {
        0.0f, 0.0f, 0.0f, 0.0f
    };
    WMOGLM->bindVAO(SGL_OGL_CONSTANTS::SGL_RENDERER_LINE_VAO);
    WMOGLM->bindVBO(SGL_OGL_CONSTANTS::SGL_RENDERER_LINE_VBO);
    WMOGLM->bufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
    WMOGLM->enableVertexAttribArray(0);
    WMOGLM->vertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid *)0);

    WMOGLM->unbindVAO();
    WMOGLM->unbindVBO();
    shader.unbind(*WMOGLM);
}

/**
 * @brief Load the line batcher buffers
 * @param shader SGL_Shader to set as the line BATCH (NOT THE SAME AS THE INDIVIDUAL LINE SHADER) renderer default
 *
 * @return nothing
 */
void SGL_Renderer::loadLineBatchBuffers(SGL_Shader shader) noexcept
{
    SGL_Log("Configuring the line batch renderer...", LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_DEFAULT);
    shader.use(*WMOGLM);

    GLfloat vertices[] = {
        0.0f, 0.0f, 0.0f, 0.0f
    };
    WMOGLM->bindVAO(SGL_OGL_CONSTANTS::SGL_RENDERER_LINE_BATCH_VAO);
    WMOGLM->bindVBO(SGL_OGL_CONSTANTS::SGL_RENDERER_LINE_BATCH_VBO);
    WMOGLM->bufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
    WMOGLM->enableVertexAttribArray(0);
    WMOGLM->vertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid *)0);

    WMOGLM->checkForGLErrors();
    // Reserve the line batch memory
    glm::vec4 memRes[SGL_OGL_CONSTANTS::MAX_LINE_BATCH_INSTANCES];

    // Line batching
    WMOGLM->bindVBO(SGL_OGL_CONSTANTS::SGL_RENDERER_LINE_BATCH_INSTANCES_VBO);
    WMOGLM->bufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * SGL_OGL_CONSTANTS::MAX_LINE_BATCH_INSTANCES, &memRes[0], GL_STATIC_DRAW);

    // Line batching
    SGL_Log("Configuring line batch shader attrib pointer 3.");
    WMOGLM->enableVertexAttribArray(3);
    WMOGLM->checkForGLErrors();
    WMOGLM->vertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, (void*)0);
    WMOGLM->checkForGLErrors();
    WMOGLM->vertexAttribDivisor(3, 1);

    WMOGLM->unbindVAO();
    WMOGLM->unbindVBO();
    shader.unbind(*WMOGLM);
}

/**
 * @brief Load the pixel buffers
 * @param shader SGL_Shader to set as the pixel renderer default
 *
 * @return nothing
 */
void SGL_Renderer::loadPointBuffers(SGL_Shader shader) noexcept
{
    SGL_Log("Configuring the pixel renderer...", LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_DEFAULT);
    shader.use(*WMOGLM);

    GLfloat vertices[] = {
        0.0f, 0.0f
    };
    WMOGLM->bindVAO(SGL_OGL_CONSTANTS::SGL_RENDERER_PIXEL_VAO);
    WMOGLM->bindVBO(SGL_OGL_CONSTANTS::SGL_RENDERER_PIXEL_VBO);
    WMOGLM->bufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
    WMOGLM->enableVertexAttribArray(0);
    WMOGLM->vertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid *)0);

    WMOGLM->unbindVAO();
    WMOGLM->unbindVBO();
    shader.unbind(*WMOGLM);
}

/**
 * @brief Load the pixel batch buffers
 * @param shader SGL_Shader to set as the pixel batch renderer default
 *
 * @return nothing
 */
void SGL_Renderer::loadPixelBatchBuffers(SGL_Shader shader) noexcept
{
    SGL_Log("Configuring the pixel batch renderer...", LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_DEFAULT);
    shader.use(*WMOGLM);

    GLfloat vertices[] = {
        0.0f, 0.0f
    };

    // Reserve the pixel batch memory
    glm::vec2 memRes[SGL_OGL_CONSTANTS::MAX_PIXEL_BATCH_INSTANCES];

    WMOGLM->bindVAO(SGL_OGL_CONSTANTS::SGL_RENDERER_PIXEL_BATCH_VAO);

    // Pixel batching
    WMOGLM->bindVBO(SGL_OGL_CONSTANTS::SGL_RENDERER_PIXEL_BATCH_INSTANCES_VBO);
    WMOGLM->bufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * SGL_OGL_CONSTANTS::MAX_PIXEL_BATCH_INSTANCES, &memRes[0], GL_STATIC_DRAW);
    WMOGLM->unbindVBO();

    // Model coords
    WMOGLM->bindVBO(SGL_OGL_CONSTANTS::SGL_RENDERER_PIXEL_BATCH_VBO);
    WMOGLM->bufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
    WMOGLM->enableVertexAttribArray(0);
    WMOGLM->vertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid *)0);

    // Pixel batching
    SGL_Log("Configuring pixel batch shader attrib pointer 3.");
    WMOGLM->bindVBO(SGL_OGL_CONSTANTS::SGL_RENDERER_PIXEL_BATCH_INSTANCES_VBO);
    WMOGLM->enableVertexAttribArray(3);
    WMOGLM->checkForGLErrors();
    WMOGLM->vertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, (void*)0);
    WMOGLM->checkForGLErrors();
    WMOGLM->vertexAttribDivisor(3, 1);

    WMOGLM->unbindVAO();
    WMOGLM->unbindVBO();
    shader.unbind(*WMOGLM);
}

/**
 * @brief Load the sprite buffers
 * @param shader SGL_Shader to set as the pixesprite renderer default
 *
 * @return nothing
 */
void SGL_Renderer::loadSpriteBuffers(SGL_Shader shader) noexcept
{
    SGL_Log("Configuring the sprite renderer...", LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_DEFAULT);
    GLfloat vertices[] = {
        // pos   tex
        0.0f, 1.0f,
        1.0f, 0.0f,
        0.0f, 0.0f,

        0.0f, 1.0f,
        1.0f, 1.0f,
        1.0f, 0.0f,
    };
    GLfloat UVCoords[] = {
        0.0f, 1.0f, // top left
        1.0f, 0.0f, // bot right
        0.0f, 0.0f, // bot left

        0.0f, 1.0f, // top left
        1.0f, 1.0f, // top right
        1.0f, 0.0f, // bot right
    };

    WMOGLM->bindVAO(SGL_OGL_CONSTANTS::SGL_RENDERER_SPRITE_VAO);
    shader.use(*WMOGLM);
    // Parse buffers to GPU
    WMOGLM->bindVBO(SGL_OGL_CONSTANTS::SGL_RENDERER_TEXTURE_UV_VBO);
    WMOGLM->bufferData(GL_ARRAY_BUFFER, sizeof(UVCoords), UVCoords, GL_DYNAMIC_DRAW);
    WMOGLM->bindVBO(SGL_OGL_CONSTANTS::SGL_RENDERER_SPRITE_VBO);
    WMOGLM->bufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

    // Bind vertex data
    WMOGLM->bindVBO(SGL_OGL_CONSTANTS::SGL_RENDERER_SPRITE_VBO);
    WMOGLM->enableVertexAttribArray(0);
    WMOGLM->vertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);

    WMOGLM->bindVBO(SGL_OGL_CONSTANTS::SGL_RENDERER_TEXTURE_UV_VBO);
    WMOGLM->enableVertexAttribArray(1);
    WMOGLM->vertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);

    WMOGLM->unbindVAO();
    WMOGLM->unbindVBO();
    shader.unbind(*WMOGLM);
}

/**
 * @brief Load the sprite batch buffers
 * @param shader SGL_Shader to set as the pixesprite renderer default
 *               MUST COINCIDE WITH THE SAME ATTRIB DIVISOR USED IN SHADER
 * @return nothing
 */
void SGL_Renderer::loadSpriteBatchBuffers(SGL_Shader shader) noexcept
{
    SGL_Log("Configuring the sprite batch renderer...", LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_DEFAULT);
    GLfloat vertices[] = {
        // pos   tex
        0.0f, 1.0f,
        1.0f, 0.0f,
        0.0f, 0.0f,

        0.0f, 1.0f,
        1.0f, 1.0f,
        1.0f, 0.0f,
    };
    GLfloat UVCoords[] = {
        0.0f, 1.0f, // top left
        1.0f, 0.0f, // bot right
        0.0f, 0.0f, // bot left

        0.0f, 1.0f, // top left
        1.0f, 1.0f, // top right
        1.0f, 0.0f, // bot right
    };

    // Reserve the memory required to store the sprite batch
    glm::mat4 *modelMatrices = new glm::mat4[SGL_OGL_CONSTANTS::MAX_SPRITE_BATCH_INSTANCES];

    WMOGLM->checkForGLErrors();

    WMOGLM->bindVAO(SGL_OGL_CONSTANTS::SGL_RENDERER_SPRITE_BATCH_VAO);
    shader.use(*WMOGLM);

    WMOGLM->checkForGLErrors();

    // Parse buffers to VBOs

    // Sprite batch
    WMOGLM->bindVBO(SGL_OGL_CONSTANTS::SGL_RENDERER_SPRITE_BATCH_INSTANCES_VBO);
    WMOGLM->bufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * SGL_OGL_CONSTANTS::MAX_SPRITE_BATCH_INSTANCES, &modelMatrices[0], GL_STATIC_DRAW);
    WMOGLM->unbindVBO();

    // UV Coords
    WMOGLM->bindVBO(SGL_OGL_CONSTANTS::SGL_RENDERER_TEXTURE_UV_VBO);
    WMOGLM->bufferData(GL_ARRAY_BUFFER, sizeof(UVCoords), UVCoords, GL_DYNAMIC_DRAW);

    // Base sprite model
    WMOGLM->bindVBO(SGL_OGL_CONSTANTS::SGL_RENDERER_SPRITE_BATCH_VBO);
    WMOGLM->bufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

    // Bind to shaders

    // Bind vertex data
    WMOGLM->bindVBO(SGL_OGL_CONSTANTS::SGL_RENDERER_SPRITE_BATCH_VBO);
    WMOGLM->enableVertexAttribArray(0);
    WMOGLM->vertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);

    // Bind texture data
    WMOGLM->bindVBO(SGL_OGL_CONSTANTS::SGL_RENDERER_TEXTURE_UV_VBO);
    WMOGLM->enableVertexAttribArray(1);
    WMOGLM->vertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);

    // Set instancing data
    GLsizei vec4Size = sizeof(glm::vec4);
    // SGL_Log("Configuring shader attrib pointer 3.");
    WMOGLM->bindVBO(SGL_OGL_CONSTANTS::SGL_RENDERER_SPRITE_BATCH_INSTANCES_VBO);
    WMOGLM->enableVertexAttribArray(3);
    WMOGLM->checkForGLErrors();
    WMOGLM->vertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, vec4Size * 4, (void*)0);
    WMOGLM->checkForGLErrors();

    // SGL_Log("Configuring shader attrib pointer 4.");
    WMOGLM->enableVertexAttribArray(4);
    WMOGLM->vertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, vec4Size * 4, (void*)(vec4Size));
    WMOGLM->checkForGLErrors();

    WMOGLM->enableVertexAttribArray(5);
    WMOGLM->vertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, vec4Size * 4, (void*)(2 * vec4Size));
    WMOGLM->checkForGLErrors();

    WMOGLM->enableVertexAttribArray(6);
    WMOGLM->vertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, vec4Size * 4, (void*)(3 * vec4Size));

    WMOGLM->vertexAttribDivisor(3, 1);
    WMOGLM->vertexAttribDivisor(4, 1);
    WMOGLM->vertexAttribDivisor(5, 1);
    WMOGLM->vertexAttribDivisor(6, 1);
    WMOGLM->unbindVBO();

    WMOGLM->unbindVAO();
    WMOGLM->unbindVBO();
    shader.unbind(*WMOGLM);

    delete []modelMatrices;
}




/**
 * @brief Generates a character texture based on the font to load
 * @param fontPath String to the font
 *
 * @return nothing
 */
void SGL_Renderer::generateTTFFont(const std::string fontPath)
{
    this->pTextShader.use(*WMOGLM);
    FT_Library ft;
    if(FT_Init_FreeType(&ft))
    {
        SGL_Log("ERROR::FREETYPE: Could not init FreeType Library", LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_RED);
        throw SGL_Exception("SGL_Renderer::generateFont | Could not open the FreeType library.");
    }

    FT_Face face;
    if(FT_New_Face(ft, fontPath.c_str(), 0, &face))
    {
        SGL_Log("ERROR::FREETYPE: Failed to load font" + fontPath, LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_RED);
        throw SGL_Exception("SGL_Renderer::generateFont | Error generating font: ");
    }

    FT_Set_Pixel_Sizes(face, 0, 48);
    // Set OpenGL options
    WMOGLM->faceCulling(true);
    WMOGLM->blending(true, BLENDING_TYPE::TEXT_RENDERING);
    WMOGLM->pixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // Load the first 128 characters of the ASCII set
    for(GLubyte c = 0; c < 128; ++c)
    {
        // Load a glyph
        if(FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            SGL_Log("ERROR::FREETYPE: Failed to load glyph", LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_RED);
            continue;
        }
        // Generate texture, due to the nature of the textures, avoid the resource manager
        GLuint texture;
        WMOGLM->genTextures(1, &texture);
        WMOGLM->bindTexture(GL_TEXTURE_2D, texture);
        WMOGLM->texImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width, face->glyph->bitmap.rows,
                          0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);
        // Texture options
        WMOGLM->texParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        WMOGLM->texParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        WMOGLM->texParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        WMOGLM->texParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Store it in the map
        Character character = {
            texture,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            static_cast<GLuint>(face->glyph->advance.x)
        };
        characters.insert(std::pair<GLchar,Character>(c, character));
    }
    WMOGLM->bindTexture(GL_TEXTURE_2D, 0);
    // Destroy ft
    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    // Configure VBO and VAO
    // VBO will be filled with glSubData during render
    WMOGLM->bindVAO(SGL_OGL_CONSTANTS::SGL_RENDERER_TEXT_VAO);
    WMOGLM->bindVBO(SGL_OGL_CONSTANTS::SGL_RENDERER_TEXT_VBO);
    WMOGLM->bufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    WMOGLM->enableVertexAttribArray(0);
    WMOGLM->vertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);

    WMOGLM->unbindVAO();
    WMOGLM->unbindVBO();
    WMOGLM->faceCulling(false);
}

/**
 * @brief Populates the internal std::map with the character values in the texture
 *
 * @return nothing
 */
void SGL_Renderer::generateBitmapFont() noexcept
{
    // Note that the following values are to be processed by Sprite::changeUVCoords

    std::uint16_t charW = 10, charH = 10;

    // Letters
    pBitmapCharacters['a'] = glm::vec4(0, 30, charW, charH);
    pBitmapCharacters['A'] = glm::vec4(0, 30, charW, charH);
    pBitmapCharacters['b'] = glm::vec4(10, 30, charW, charH);
    pBitmapCharacters['B'] = glm::vec4(10, 30, charW, charH);
    pBitmapCharacters['c'] = glm::vec4(20, 30, charW, charH);
    pBitmapCharacters['C'] = glm::vec4(20, 30, charW, charH);
    pBitmapCharacters['d'] = glm::vec4(30, 30, charW, charH);
    pBitmapCharacters['D'] = glm::vec4(30, 30, charW, charH);
    pBitmapCharacters['e'] = glm::vec4(40, 30, charW, charH);
    pBitmapCharacters['E'] = glm::vec4(40, 30, charW, charH);
    pBitmapCharacters['f'] = glm::vec4(50, 30, charW, charH);
    pBitmapCharacters['F'] = glm::vec4(50, 30, charW, charH);
    pBitmapCharacters['g'] = glm::vec4(60, 30, charW, charH);
    pBitmapCharacters['G'] = glm::vec4(60, 30, charW, charH);
    pBitmapCharacters['h'] = glm::vec4(70, 30, charW, charH);
    pBitmapCharacters['H'] = glm::vec4(70, 30, charW, charH);
    pBitmapCharacters['i'] = glm::vec4(80, 30, charW, charH);
    pBitmapCharacters['I'] = glm::vec4(80, 30, charW, charH);
    pBitmapCharacters['j'] = glm::vec4(90, 30, charW, charH);
    pBitmapCharacters['J'] = glm::vec4(90, 30, charW, charH);
    pBitmapCharacters['k'] = glm::vec4(100, 30, charW, charH);
    pBitmapCharacters['K'] = glm::vec4(100, 30, charW, charH);
    pBitmapCharacters['l'] = glm::vec4(110, 30, charW, charH);
    pBitmapCharacters['L'] = glm::vec4(110, 30, charW, charH);
    pBitmapCharacters['m'] = glm::vec4(120, 30, charW, charH);
    pBitmapCharacters['M'] = glm::vec4(120, 30, charW, charH);

    pBitmapCharacters['n'] = glm::vec4(0, 40, charW, charH);
    pBitmapCharacters['N'] = glm::vec4(0, 40, charW, charH);
    pBitmapCharacters['o'] = glm::vec4(10, 40, charW, charH);
    pBitmapCharacters['O'] = glm::vec4(10, 40, charW, charH);
    pBitmapCharacters['p'] = glm::vec4(20, 40, charW, charH);
    pBitmapCharacters['P'] = glm::vec4(20, 40, charW, charH);
    pBitmapCharacters['q'] = glm::vec4(30, 40, charW, charH);
    pBitmapCharacters['Q'] = glm::vec4(30, 40, charW, charH);
    pBitmapCharacters['r'] = glm::vec4(40, 40, charW, charH);
    pBitmapCharacters['R'] = glm::vec4(40, 40, charW, charH);
    pBitmapCharacters['s'] = glm::vec4(50, 40, charW, charH);
    pBitmapCharacters['S'] = glm::vec4(50, 40, charW, charH);
    pBitmapCharacters['t'] = glm::vec4(60, 40, charW, charH);
    pBitmapCharacters['T'] = glm::vec4(60, 40, charW, charH);
    pBitmapCharacters['u'] = glm::vec4(70, 40, charW, charH);
    pBitmapCharacters['U'] = glm::vec4(70, 40, charW, charH);
    pBitmapCharacters['v'] = glm::vec4(80, 40, charW, charH);
    pBitmapCharacters['V'] = glm::vec4(80, 40, charW, charH);
    pBitmapCharacters['w'] = glm::vec4(90, 40, charW, charH);
    pBitmapCharacters['W'] = glm::vec4(90, 40, charW, charH);
    pBitmapCharacters['x'] = glm::vec4(100, 40, charW, charH);
    pBitmapCharacters['X'] = glm::vec4(100, 40, charW, charH);
    pBitmapCharacters['y'] = glm::vec4(110, 40, charW, charH);
    pBitmapCharacters['Y'] = glm::vec4(110, 40, charW, charH);
    pBitmapCharacters['z'] = glm::vec4(120, 40, charW, charH);
    pBitmapCharacters['Z'] = glm::vec4(120, 40, charW, charH);

    // Numbers
    pBitmapCharacters['0'] = glm::vec4(0, 10, charW, charH);
    pBitmapCharacters['1'] = glm::vec4(10, 10, charW, charH);
    pBitmapCharacters['2'] = glm::vec4(20, 10, charW, charH);
    pBitmapCharacters['3'] = glm::vec4(30, 10, charW, charH);
    pBitmapCharacters['4'] = glm::vec4(40, 10, charW, charH);
    pBitmapCharacters['5'] = glm::vec4(50, 10, charW, charH);
    pBitmapCharacters['6'] = glm::vec4(60, 10, charW, charH);
    pBitmapCharacters['7'] = glm::vec4(70, 10, charW, charH);
    pBitmapCharacters['8'] = glm::vec4(80, 10, charW, charH);
    pBitmapCharacters['9'] = glm::vec4(90, 10, charW, charH);
    pBitmapCharacters['['] = glm::vec4(100, 10, charW, charH);
    pBitmapCharacters[']'] = glm::vec4(110, 10, charW, charH);

    // More Symbols
    pBitmapCharacters['!'] = glm::vec4(0, 0, charW, charH);
    pBitmapCharacters['"'] = glm::vec4(10, 0, charW, charH);
    pBitmapCharacters['$'] = glm::vec4(20, 0, charW, charH);
    pBitmapCharacters['%'] = glm::vec4(30, 0, charW, charH);
    pBitmapCharacters['('] = glm::vec4(40, 0, charW, charH);
    pBitmapCharacters[')'] = glm::vec4(50, 0, charW, charH);
    pBitmapCharacters['*'] = glm::vec4(60, 0, charW, charH);
    pBitmapCharacters['+'] = glm::vec4(70, 0, charW, charH);
    pBitmapCharacters['-'] = glm::vec4(80, 0, charW, charH);
    pBitmapCharacters['/'] = glm::vec4(90, 0, charW, charH);
    pBitmapCharacters['.'] = glm::vec4(100, 0, charW, charH);
    pBitmapCharacters['\\'] = glm::vec4(110,0, charW, charH);

    pBitmapCharacters[':'] = glm::vec4(0, 20, charW, charH);
    pBitmapCharacters[';'] = glm::vec4(10, 20, charW, charH);
    pBitmapCharacters['{'] = glm::vec4(20, 20, charW, charH);
    pBitmapCharacters['='] = glm::vec4(30, 20, charW, charH);
    pBitmapCharacters['}'] = glm::vec4(40, 20, charW, charH);
    pBitmapCharacters['?'] = glm::vec4(50, 20, charW, charH);
    pBitmapCharacters['@'] = glm::vec4(60, 20, charW, charH);
    pBitmapCharacters['_'] = glm::vec4(70, 20, charW, charH);
    pBitmapCharacters[','] = glm::vec4(80, 20, charW, charH);
    pBitmapCharacters['|'] = glm::vec4(90, 20, charW, charH);

    // The last inserted entry will be the one returned by find()
    // should the search fail, making this the default "NOT OFUND"
    // character
    pBitmapCharacters['~'] = glm::vec4(0, 120, charW, charH);

    SGL_Log("Total characters  in bitmap font: " + std::to_string(pBitmapCharacters.size()));

}
