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
 * @file    src/skeletonGL/utility/SGL_Camera.cpp
 * @author  SENEX @ XENOBYTE.XYZ
 * @date    26/01/2021
 * @version 2.1
 *
 * @brief Applies the final MVP matrix to the relevant shaders
 *
 * @section DESCRIPTION
 *
 * The camera applies the requested changes to the render scenery
 */

#ifndef SRC_SKELETONGL_RENDERER_CAMERA_HPP
#define SRC_SKELETONGL_RENDERER_CAMERA_HPP

// C++
#include <memory>
// Dependencies
#include "../deps/glm/glm.hpp"
#include "../utility/SGL_DataStructures.hpp"
// SkeletonGL
#include "SGL_OpenGLManager.hpp"
#include "SGL_Shader.hpp"

/**
 * @brief Camera module
 * @section DESCRIPTION
 *
 * This class encapsulates an internal orthographic projection matrix that
 * is applied to every render.
 */
class SGL_Camera
{
private:
    std::shared_ptr<SGL_OpenGLManager> WMOGLM; ///< The primary OpenGL context
    glm::vec2 pCameraPos, pDeltaCameraPos;     ///< Camera positions
    glm::mat4 pOrthographicProjection;         ///< The main perspective projection matrix
    glm::mat4 pCameraMatrix, pOverlayMatrix;   ///< The two main rendering modes, camera and overlay modes
    float pScale, pDeltaScale;                 ///< Zoom in/out (-1.0 <-> 1.0)
    bool pUpdateMatrix;                        ///< Update the camera?
    int pScreenW, pScreenH;                    ///< Screen properties to define camera boundaries

public:
    // Constructor
    SGL_Camera(std::shared_ptr<SGL_OpenGLManager> oglm);
    // Destructor
    ~SGL_Camera();
    // Configure the orthographic shader and initial camera position
    void initialize(int windowW, int windowH);
    // Updates the MVP matrix
    void updateCamera(bool forceUpdate = false) noexcept;
    // Switches between the available camera modes, check the data structures header file for more info
    void cameraMode(SGL_Shader shader, CAMERA_MODE mode = CAMERA_MODE::DEFAULT) noexcept;
    // Move the camera
    void setPosition(glm::vec2 pos) noexcept;
    // Scale the camera (-1.0 <-> 1.0)
    void setScale(float newScale) noexcept;
    // Returns the final matrix
    glm::mat4 getCameraMatrix() const noexcept;
    // Returns the overlay matrix
    glm::mat4 getOverlayMatrix() const noexcept;
    // Returns the camera position
    glm::vec4 getPosition() const noexcept;
};

#endif // SRC_SKELETONGL_RENDERER_CAMERA_HPP
