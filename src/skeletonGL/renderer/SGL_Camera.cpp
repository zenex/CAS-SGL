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

#include "SGL_Camera.hpp"

/**
 * @brief Main and only constructor
 *
 * @param oglm Main window SGL_OpenGLManager
 * @return nothing
 */
SGL_Camera::SGL_Camera(std::shared_ptr<SGL_OpenGLManager> oglm) : pScale(1.0f), pCameraMatrix(1.0f), pUpdateMatrix(true),
                       pOrthographicProjection(1.0f), pScreenW(0), pScreenH(0),
                       pCameraPos(0.0f), pDeltaCameraPos(0.0f), pDeltaScale(0.0f),
                       pOverlayMatrix(0.0f)
{
    // Share the main OGLM instance
    WMOGLM = oglm;
}

/**
 * @brief Destructor
 *
 * @return nothing
 */
SGL_Camera::~SGL_Camera()
{

}


/**
 * @brief Starts the camera
 *
 * @param windoW Camera width
 * @param windoH Camera height
 * @return nothing
 */
void SGL_Camera::initialize(int windowW, int windowH)
{
    this->pScreenW = windowW;
    this->pScreenH = windowH;

    // These functions set scale and alignment for the orthographic projection that will be used in all the shaders
    // Y axis: UP = -1 DOWN = 1
    this->pOrthographicProjection = glm::ortho(0.0f, static_cast<float>(pScreenW), static_cast<float>(pScreenH), 0.0f, -1.0f, 1.0f);
    // Flipped Y axis: UP = 1 DOWN = -1
    //this->pOrthographicProjection = glm::ortho(0.0f, static_cast<float>(pScreenW), 0.0f, static_cast<float>(pScreenH), -1.0f, 1.0f);
    this->pOverlayMatrix = pOrthographicProjection;

    this->pCameraPos = glm::vec2(static_cast<float>(pScreenW / 2), static_cast<float>(pScreenH / 2));
    this->pDeltaCameraPos = pCameraPos;
}


/**
 * @brief Scales the camera's 'Z' value
 *
 * @param newScale Float with the new value (-1.0 <-> 1.0)
 * @return nothing
 */
void SGL_Camera::setScale(float newScale) noexcept
{
    if (newScale < -1.0 )
        newScale = -1.0;
    if (newScale > 1.0 )
        newScale = 1.0;

    this->pScale = newScale;
}


/**
 * @brief Updates the camera's matrices (if needed or forced)
 *
 * @param forceUpdate Bool to force the new values on the matrices
 * @return nothing
 */
void SGL_Camera::updateCamera(bool forceUpdate) noexcept
{
    if (pDeltaCameraPos != pCameraPos || pDeltaScale != pScale)
        pUpdateMatrix = true;
    if (pUpdateMatrix || forceUpdate)
    {
        // Update the camera
        // Move the camera
        glm::vec3 translate(-pCameraPos.x + static_cast<float>(pScreenW / 2), -pCameraPos.y + static_cast<float>(pScreenH / 2), 0.0f);
        pCameraMatrix = glm::translate(pOrthographicProjection, translate);
        // Scale the camera
        glm::vec3 scale(pScale, pScale, 0.0f);
        pCameraMatrix = glm::scale(glm::mat4(1.0f), scale) * pCameraMatrix;
        // Note that the pActiveCamera must be set to true and cameraMode must be called before every render for the update to have effect
        pDeltaCameraPos = pCameraPos;
        pDeltaScale = pScale;
        pUpdateMatrix = false;
    }
}

/**
 * @brief Applies the adequate projecton matrix to the specified shader, window manager wrapper updates them all.
 * Do NOT add the framebuffer shaders to the function
 *
 * @param shader The shader to be updated
 * @param mode Camera mode to use (default or overlay)
 * @return nothing
 */
void SGL_Camera::cameraMode(SGL_Shader shader, CAMERA_MODE mode) noexcept
{
    if (mode == DEFAULT)
    {
        // Camera is active, apply the offset to the projection matrix
        shader.use(*WMOGLM).setMatrix4(*WMOGLM, SGL_OGL_CONSTANTS::SHADER_UNIFORM_M4F_PROJECTION.c_str(), pCameraMatrix);
    }
    else if (mode == OVERLAY)
    {
        // Camera is disabled, apply the original overlay  matrix
        shader.use(*WMOGLM).setMatrix4(*WMOGLM, SGL_OGL_CONSTANTS::SHADER_UNIFORM_M4F_PROJECTION.c_str(), pOverlayMatrix);
    }

}

/**
 * @brief Position the camera
 *
 * @param pos GLM::vec2 (x, y)
 * @return nothing
 */
void SGL_Camera::setPosition(glm::vec2 pos) noexcept
{
    this->pCameraPos = pos;
}

/**
 * @brief Returns the camera matrix
 *
 * @return glm::mat4 The camera matrix
 */
glm::mat4 SGL_Camera::getCameraMatrix() const noexcept
{
    return this->pCameraMatrix;
}


/**
 * @brief Returns the overlay matrix
 *
 * @return glm::mat4 The overlay mode matrix
 */
glm::mat4 SGL_Camera::getOverlayMatrix() const noexcept
{
    return this->pOverlayMatrix;
}


/**
 * @brief Returns the overlay matrix
 *
 * @return glm::mat4 The overlay mode matrix
 */
glm::vec4 SGL_Camera::getPosition() const noexcept
{
    return glm::vec4(this->pCameraPos.x, this->pCameraPos.y, pScreenW, pScreenH);
}
