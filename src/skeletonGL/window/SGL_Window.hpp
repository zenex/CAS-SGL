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
 * @file    src/skeletonGL/window/SGL_Window.hpp
 * @author  SENEX @ XENOBYTE.XYZ
 * @date    26/01/2021
 * @version 2.1
 * @brief Main window manager, acts as the SGL framework's interface
 * @section DESCRIPTION
 *
 * SGL_Window wraps around most of the library's functionality, it provides the programmer
 * with the user's input, sets up the main window, controls the simulation's time, initializes and
 * configures the rendering engine, sets up renders. etc. This is the primary access to SkeletonGL.
 */

#ifndef SRC_SKELETONGL_WINDOW_SGLWINDOW_H
#define SRC_SKELETONGL_WINDOW_SGLWINDOW_H

// C++
#include <iostream>
#include <array>
#include <sstream>
#include <ctime>
#include <vector>
#include <memory>
#include <stdio.h>
#include <thread>
#include <chrono>

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// SDL2
#include <SDL2/SDL.h>
#include <SDL2/SDL_version.h>
// GLM
#include "../deps/glm/glm.hpp"
#include "../deps/glm/gtc/type_ptr.hpp"
#include "../deps/glm/gtc/matrix_transform.hpp"
// FREETYPE
#include <ft2build.h>
#include FT_FREETYPE_H

// SGL - UTILITY
#include "../utility/SGL_AssetManager.hpp"
#include "../utility/SGL_DataStructures.hpp"
#include "../utility/SGL_Utility.hpp"
//SGL - RENDERER
#include "../renderer/SGL_OpenGLManager.hpp"
#include "../renderer/SGL_Camera.hpp"
#include "../renderer/SGL_Renderer.hpp"
#include "../renderer/SGL_PostProcessor.hpp"


/**
 *  @brief Encapsulates the SkeletonGL library
 */
class SGL_Window
{
private:
    // SKELETONGL
    const float pSGLVERSION = 2.1f;                           ///< Current SGL version number
    // WINDOW & OPENGL
    SDL_Window *pWindow;                                      ///< Main window pointer
    SDL_GLContext pGLContext;                                 ///< Internal OpenGL context
    const std::string pGLVersion;                             ///< OpenGL version used
    WindowCreationSpecs pWindowCreationSpecs;                 ///< Window creation details, overwritten by the INI file
    SGL_Color pClearScreen;                                   ///< Clear screen color (will be passed to the PP-FBO)
    std::shared_ptr<SGL_OpenGLManager> pOGLM;                 ///< Main OpenGL context manager, shared with assetManager and renderer
    bool pAlreadyInitialized;                                 ///< Prevent re-initialization
    bool pHasMouseFocus, pHasKeyboardFocus, pIsMinimized;     ///< Is the window minimized or focused?
    // INI PARSER
    SGL_IniParser *pIniParser;                                ///< Processes the local .ini file
    // INPUT PROCESSING
    SDL_Event pEvent;                                         ///< Internal SDL input polling
    SGL_InputFrame pDeltaInput;                               ///< The frame's  entire input record, used by the post-processor
    // TIME PROFILING
    std::chrono::steady_clock::time_point pChrono0, pChrono1; ///< C++11 chrono based counter
    double pDeltaTimeMS;                                      ///< Time the past frame took to finish
    // float pT0, pT1;                                           ///< Used to calculate the frame's processing time
    // std::uint32_t pTick0, pTick1, pEpochTick;                 ///< Tick based
    // double pChronoDeltaTime;                                  ///< C++11 chrono based counter
    // std::uint16_t pTotalFrames;
    // // FIXED TIME STEP
    // float pDTAccumulator;
    // std::uint16_t pUpdateCycles;
    // std::vector<SGL_FrameData> pRenderedFrames;

    // // FINAL
    // float pFrameCompletionTime;

    // CAMERA AND POST-PROCESSOR
    SGL_Camera *pCamera;                                      ///< 2D camera module (contains the orthographic projection data)
    //FBO and shader to reload the FBO if necessary
    SGL_PostProcessor *pPostProcessorFBO;                     ///< Main framebuffer
    SGL_Shader pDefaultPPShader;                              ///< Main post-processor shader

    // TESTING
    SDL_Joystick *pGamepad_1;

    // Create a window and configure the entire framework, called by initializeWindow()
    void start();
    // Loads the internal assets
    void pLoadDefaultAssets();
    // Initialize the PP-FBO
    void startFBO(const SGL_Shader &shader);
    // Resize the OGL viewport and resize the PP-FBO texture to match.
    void resizeGL(int width, int heigth);
    // Processes the .ini file (must be located in the same folder as the executable)
    void processIniFile(std::string path);

    // Disable both move and copy constructors
    SGL_Window(const SGL_Window &) = delete;
    SGL_Window &operator = (const SGL_Window &) = delete;
    SGL_Window(SGL_Window &&) = delete;
    SGL_Window &operator = (SGL_Window &&) = delete;

public:

    // Constructor
    SGL_Window();
    // Destructor
    ~SGL_Window();

    // Destroys everything
    void destroyEverything();

    // Public renderers and assetmanager, both configured with the
    // window manager's internal OGLM instance, this to keep tabs on
    // all gl related events and context
    std::unique_ptr<SGL_AssetManager> assetManager; ///< Loads textures, fonts and shaders
    std::unique_ptr<SGL_Renderer> renderer;         ///< Renders to the screen
    std::unique_ptr<SGL_DeviceInfo> hostData;       ///< Contains all of the host device's relevant data

    // Initialize the window, setups the camera as well
    // This function does all initialization checkups, the aim is to keep all potential
    // problems related to opengl contexts and window loading here, this is the first function
    // the programmer must call to initialize SkeletonGL
    void initializeWindow(int x, int y, int w, int h, int internalW, int internalH,
                          const std::string &windowTitle, bool fullscreen, std::string iniFile = "");

    // -- WINDOW HANDLERS --
    // Sets the window's title
    void setWindowTitle(const std::string &newTitle);
    // Returns the window's creation data
    WindowCreationSpecs getWindowCreationSpecs();
    // Sets the window icon
    void setWindowIcon(Uint16 pixelMatrix[16*16]);
    // Resizes the window
    void resizeWindow(int w, int h, bool scaleInternalResolution = false);
    // Turns fullscreen on / off
    void toggleFullScreen(bool toggle = true);
    // Does the window has the keyboard focus?
    bool hasKeyboardFocus() const;
    // Does the window has the mouse focus?
    bool hasMouseFocus() const;
    // Enable / disable VSYNC
    void toggleVSYNC(bool enable);
    // Toggles the system cursor visibility
    void toggleCursor(bool enable);

    // -- CAMERA --
    // Sets the camera position
    void setCameraPosition(glm::vec2 pos);
    // Scales (zooms) the camera in or out
    void setCameraScale(float newScale);
    // Change between overaly and default camera modes
    void setCameraMode(CAMERA_MODE mode = CAMERA_MODE::DEFAULT);
    // Set the console output filter level
    void setLogLevel(LOG_LEVEL logLevel = LOG_LEVEL::SGL_DEBUG);
    // Update the camera
    void updateCamera(bool forceUpdate = false);
    // Get the camera's x position
    int getCameraPosX() const;
    // Get the camera's y position
    int getCameraPosY() const;
    // Get the camera's width
    int getCameraPosW() const;
    // Get the camera's height
    int getCameraPosH() const;

    // -- INPUT --
    // Returns a COPY of the this frame's total input
    // sequence, meaning a struct with the values af all
    // the supported input states with their respective
    // state (bool)
    SGL_InputFrame getFrameInput();
    // Selects a gamepad ID to bind to the current controller port (currently only one gamepad is supported)
    void checkGamepads(int gamepadID = 0);

    // -- POST PROCESSOR --
    // Controls the post processor (FBO) behavior
    // startFrame and end Frame MUST be called at
    // their respective times to properly render
    void setClearColor(const SGL_Color &color);
    // Sets the default shader the final FBO texture will use
    void setPostProcessorShader(SGL_Shader shader);
    // Enable shake effect?
    void setShakeScreen(bool shakeit);
    // Enable chaos effect?
    void setChaosEffect(bool chaos);
    // Enable confuse effect?
    void setConfuseEffect(bool confuse);
    void toggleLineAA(bool toggle);

    // FPS
    // Begin counting frame time
    void startFrame();
    // End counting frame time
    void endFrame();


    // Get the delta time as a double
    double getRenderDeltaTime() const;

    // Manually check for OpenGL errors
    void checkForErrors() const;
};


#endif // SRC_SKELETONGL_WINDOW_SGLWINDOW_H
