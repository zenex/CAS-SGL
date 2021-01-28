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
 * @file    src/skeletonGL/window/SGL_Window.cpp
 * @author  SENEX @ XENOBYTE.XYZ
 * @date    26/01/2021
 * @version 2.1
 *
 * @brief Main window manager, acts as the SGL framework's interface
 *
 * @section DESCRIPTION
 *
 * SGL_Window wraps around most of the library's functionality, it provides the programmer
 * with the user's input, sets up the main window, controls the simulation's time, initializes and
 * configures the rendering engine, sets up renders. etc. This is the primary access to SkeletonGL.
 *
 */

#include "SGL_Window.hpp"


/**
 * @brief Main and only constructor
 *
 * @param
 * @return nothing
 */
SGL_Window::SGL_Window()
{
    // Enable logging during initialization, the user should modify this
    // value after
    setLogLevel(LOG_LEVEL::SGL_DEBUG);
    pAlreadyInitialized = false;
}

/**
 * @brief Destructor, must orderly free resources
 * @return nothing
 */
SGL_Window::~SGL_Window()
{
    SGL_Log("Exiting SkeletonGL.", LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_GREEN);
    SGL_Log("( ˘ ³˘)♥ Cheers M8.", LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_GREEN);
    this->destroyEverything();
    // Destroy SDL
    SDL_GL_DeleteContext(pGLContext);
    SDL_DestroyWindow(pWindow);
    SDL_Quit();
}


/**
 * @brief Destroys local resources
 * @return nothing
 */
void SGL_Window::destroyEverything()
{
    // If the gamepad is set, destroy it
    if (pGamepad_1 != NULL)
        SDL_JoystickClose(pGamepad_1);

    delete pIniParser;
    delete pCamera;
    delete pPostProcessorFBO;
}

/**
 * @brief Configure SkeletonGL, initializes all internal objects and subsystems
 *
 * @param x Window x position
 * @param y Window y position
 * @param w Window width (size, not internal resolution)
 * @param h Window height (size, not internal resolution)
 * @param internalW Internal rendering resolution width
 * @param internalH Internal rendering resolution height
 * @param windowTitle Limited to 50 characters, will truncate otherwise
 * @param fullscreen Should the window ne launched in fullscreen mode?
 * @param FPS Max FPS limit
 * @param iniFile .ini file to process, WILL OVERRIDE THESE VALUES
 * @return nothing
 */
void SGL_Window::initializeWindow(int x, int y, int w, int h, int internalW, int internalH,
                                  const std::string &windowTitle, bool fullscreen, std::string iniFile)
{
    pWindowCreationSpecs.currentX = x;
    pWindowCreationSpecs.currentY = y;
    pWindowCreationSpecs.currentW = w;
    pWindowCreationSpecs.currentH = h;
    pWindowCreationSpecs.internalW = internalW;
    pWindowCreationSpecs.internalH = internalH;
    pWindowCreationSpecs.windowTitle = windowTitle;
    pWindowCreationSpecs.fullScreen = fullscreen;
    pWindowCreationSpecs.enableVSYNC = true;
    // Process the ini file
    if (iniFile != "")
    {
        SGL_Log("Parsing configuration file: " + iniFile, LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_BLUE);
        pWindowCreationSpecs.iniFile = iniFile;
        processIniFile(iniFile);
    }
    this->start();
}

/**
 * @brief Parses and processes the .ini file
 *
 * @param path std::string, absolute path to the .ini file
 * @return nothing
 * @section DESCRIPTION
 *
 * Custom ini settings should be added here, the following are reserved for obvious internal use.
 * resolution, vsync, fullscreen, screen_id, gamepad_id
 */
void SGL_Window::processIniFile(std::string path)
{
    // --- INI LOADING ---
    // Load the ini configuration file
    pIniParser = new SGL_IniParser(pWindowCreationSpecs.iniFile);

    // [video]
    // resolution
    std::string resolutionIni = pIniParser->getRawValue("[VIDEO]", "resolution");
    SGL_Log("INI FILE resolution = " + resolutionIni);
    // Resoultion format must be WIDTHxHEIGHT (ex. 1920x1080)
    std::size_t posX = resolutionIni.find("x");
    if (resolutionIni != "" && posX != std::string::npos)
    {
        try
        {
            pWindowCreationSpecs.currentW = std::stoi(resolutionIni.substr(0, posX));
            pWindowCreationSpecs.currentH = std::stoi(resolutionIni.substr(posX + 1));
            //pWindowCreationSpecs.currentH = std::stoi(std::string(resolutionIni.substr(resolutionIni.find_last_of("x") + 1)));
        }
        catch (std::out_of_range &e)
        {
            SGL_Log(e.what());
        }
        SGL_Log("INI FILE resolution W = " + std::to_string(pWindowCreationSpecs.currentW));
        SGL_Log("INI FILE resolution H = " + std::to_string(pWindowCreationSpecs.currentH));
    }

    // Vsync
    std::string vsyncIni = pIniParser->getRawValue("[VIDEO]", "vsync");
    SGL_Log("INI FILE vsync = " + vsyncIni, LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_BLUE);
    if (!vsyncIni.empty())
    {
        if (vsyncIni == "0")
            pWindowCreationSpecs.enableVSYNC = false;
        else
            pWindowCreationSpecs.enableVSYNC = true;
    }
    // Fullscreen
    std::string fullscreenIni = pIniParser->getRawValue("[VIDEO]", "fullscreen");
    SGL_Log("INI FILE fullscreen = " + fullscreenIni, LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_BLUE);
    if (fullscreenIni == "0")
        pWindowCreationSpecs.fullScreen = false;
    else
        pWindowCreationSpecs.fullScreen = true;
    // Display ID
    int display_idIni = pIniParser->getIntValue("[VIDEO]", "display_id");
    SGL_Log("INI FILE display_id = " + std::to_string(display_idIni));
    pWindowCreationSpecs.displayID = display_idIni;
    // Show FPS
    std::string showFPSIni = pIniParser->getRawValue("[VIDEO]", "show_fps");
    if (!showFPSIni.empty())
    {
        if (showFPSIni == "0")
            pWindowCreationSpecs.showFPS = false;
        else
            pWindowCreationSpecs.showFPS = true;
    }
    SGL_Log("INI FILE show_fps = " + showFPSIni, LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_BLUE);
    // [INPUT]
    // Gamepad ID shouldn't be higher than 4
    int main_gamepad_id = pIniParser->getIntValue("[INPUT]", "main_gamepad_id");
    if (main_gamepad_id > 4)
    {
        SGL_Log("INI FILE main_gamepad_id: " + std::to_string(main_gamepad_id) + " INVALID ID. Setting to 0.", LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_BLUE);
        pWindowCreationSpecs.gamepad1 = 0;
    }
    else
    {
        SGL_Log("INI FILE main_gamepad_id = " + std::to_string(main_gamepad_id), LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_BLUE);
        pWindowCreationSpecs.gamepad1 = main_gamepad_id;
    }
}

/**
 * @brief Toggles the visibility of the default system cursor
 * @param bool enables / disables the system cursor
 * @return nothing
 * @section DESCRIPTION
 *
 *
*/
void SGL_Window::toggleCursor(bool enable)
{
    if (!enable)
    {
        int i = SDL_ShowCursor(SDL_FALSE);
        if (SDL_ShowCursor(SDL_QUERY) != SDL_FALSE)
            throw SGL_Exception("int SGL_Window::toggleCursor | Error disabling the system cursor \n ");

        pWindowCreationSpecs.cursorVisibility = false;
        SGL_Log("System cursor disabled.", LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_DEFAULT);
    }
    else
    {
        int i = SDL_ShowCursor(SDL_ENABLE);
        if (SDL_ShowCursor(SDL_ENABLE) != SDL_ENABLE)
            throw SGL_Exception("int SGL_Window::toggleCursor | Error disablinenabling the system cursor \n ");

        pWindowCreationSpecs.cursorVisibility = true;
        SGL_Log("System cursor enabled.", LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_DEFAULT);
    }
}

/**
 * @brief Turns VSYNC on and off
 * @param bool enables / disables VSYNC
 * @return nothing
 * @section DESCRIPTION
 *
 *
*/
void SGL_Window::toggleVSYNC(bool enable)
{
    // VSYNC support
    if (!enable)
    {
        if (SDL_GL_SetSwapInterval(0) < 0)
            throw SGL_Exception("int SGL_Window::toggleVSYNC | Error disabling VSync \n ");

        pWindowCreationSpecs.activeVSYNC = false;
        SGL_Log("VSync disabled.", LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_DEFAULT);
    }
    else
    {
        if (SDL_GL_SetSwapInterval(1) < 0)
            throw SGL_Exception("int SGL_Window::toggleVSYNC | Error enabling VSync \n ");

        pWindowCreationSpecs.activeVSYNC = true;
        SGL_Log("VSync enabled.", LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_DEFAULT);
    }
}

/**
 * @brief Initializes dependencies, creates window and configures the renderer and asset manager modules
 * @return nothing
 * @section DESCRIPTION
 *
 * This functions loads their SkeletonGL library, it reads the .ini file (which will overwrite any settings
 * provided by initializeWindow), starts all dependencies, configures the orthographic matrix, loads and
 * generates all necessary assets, configures the renderer and postprocessor and creates a window.
*/
void SGL_Window::start()
{
    if (pAlreadyInitialized)
        return;

    SGL_Log("(•̀ᴗ•́)و< \Initializing SkeletonGL ver. " + std::to_string(pSGLVERSION), LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_GREEN);
    SGL_Log("      <--o-- By SENEX @ XENOBYTE.XYZ --o-->", LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_GREEN);

    pHasMouseFocus = false;
    pHasKeyboardFocus = false;
    pIsMinimized = false;

    // --- SDL2 & WINDOW CREATION ---
    SDL_SetHint(SDL_HINT_NO_SIGNAL_HANDLERS, "1");
    // SGL_Log("Initializing SDL2....", LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_DEFAULT);
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER |
                SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER |
                SDL_INIT_HAPTIC | SDL_INIT_NOPARACHUTE) < 0)
    {
        SGL_Log(SDL_GetError(), LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_DEFAULT);
        throw SGL_Exception("int SGL_Window::start | Error starting SDL2 \n ");
    }
    // Testing other SDL2 modules, not needed atm
    // if(TTF_Init() < 0)
    // {
    //     SGL_Log(TTF_GetError());
    //     throw SGL_Exception("EXCEPTION IN: int SGL_Window::start | Error starting SDL_TTF \n ");
    // }
    // if(IMG_Init(IMG_INIT_PNG|IMG_INIT_JPG) < 0)
    // {
    //     SGL_Log(IMG_GetError());
    //     throw SGL_Exception("EXCEPTION IN: int SGL_Window::start | Error starting SDL_Image \n ");
    // }
    // if(Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0)
    // {
    //     SGL_Log(Mix_GetError());
    //     throw SGL_Exception("EXCEPTION IN: int SGL_Window::start | Error starting SDL_Mixer \n ");
    // }

    // Create the window
    this->pWindow = SDL_CreateWindow(pWindowCreationSpecs.windowTitle.c_str(),
                                     pWindowCreationSpecs.windowPosX,
                                     pWindowCreationSpecs.windowPosY,
                                     pWindowCreationSpecs.currentW,
                                     pWindowCreationSpecs.currentH,
                                     SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL |
                                     SDL_INIT_JOYSTICK);

    if ( pWindowCreationSpecs.fullScreen != 0 )
        this->toggleFullScreen(true);
    if (pWindow == NULL)
    {
        SGL_Log("ERROR initializing an SDL2 window: " +  std::string(SDL_GetError()), LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_DEFAULT);
        throw SGL_Exception("int SGL_Window::start | Error creating window \n ");
    }
    SGL_Log("SDL2 initialized.", LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_DEFAULT);

    // --- OPENGL & GLEW  ---
    // SGL_Log("Initializing OpenGL...", LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_DEFAULT);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    // hardware accel: set to 1 to require hardware acceleration set
    // to 0 to force software rendering; defaults to allow either
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
    this->pGLContext = SDL_GL_CreateContext(pWindow);
    if(this->pGLContext == NULL)
    {
        SGL_Log("ERROR initializing OpenGL context.", LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_DEFAULT);
        throw SGL_Exception("int SGL_Window::start | Error creating opengl context \n ");
    }
    const unsigned char *version = glGetString(GL_VERSION);
    if(version == NULL)
    {
        SGL_Log("ERROR retrieving opengl version", LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_DEFAULT);
        throw SGL_Exception("int SGL_Window::start | Error retrieving opengl version \n ");
    }
    // Must create a context and make it current before calling glewinit
    glewExperimental = GL_TRUE;
    GLenum glew_status = glewInit();
    if(glew_status != GLEW_OK)
    {
        std::stringstream ss;
        const unsigned char *errStr = glewGetErrorString(glew_status);
        ss << errStr;
        SGL_Log("GLEW ERROR: " + ss.str(), LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_DEFAULT);
        throw SGL_Exception("int SGL_Window::start | Error starting GLEW \n ");
    }
    // Make the created context current
    SDL_GL_MakeCurrent(pWindow, pGLContext);
    SDL_SetWindowMinimumSize(pWindow, pWindowCreationSpecs.minW, pWindowCreationSpecs.minH);
    SDL_SetWindowMaximumSize(pWindow, pWindowCreationSpecs.maxW, pWindowCreationSpecs.maxH);
    SGL_Log("Window created. W: " + std::to_string(pWindowCreationSpecs.currentW) + " H: " + std::to_string(pWindowCreationSpecs.currentH), LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_DEFAULT);

    // VSYNC support
    toggleVSYNC(pWindowCreationSpecs.enableVSYNC);

    pOGLM = std::make_shared<SGL_OpenGLManager>();

    // Invalid enumerant error as there is yet details to configure, its normal
    pOGLM->checkForGLErrors();
    SGL_Log("OpenGL initialized.", LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_DEFAULT);

    // --- HARDWARE PROBE ---
    // Load host system info
    this->hostData = std::make_unique<SGL_DeviceInfo>();
    SDL_version compiled;
    SDL_version linked;
    SDL_VERSION(&compiled);
    SDL_GetVersion(&linked);

#ifdef __linux__
    hostData->hostOS = "Linux";
#endif

#ifdef __WIN32__
    hostData->hostOS = "Windows";
#endif

#ifdef __APPLE__
    hostData->hostOS = "Apple";
#endif

#ifdef __FreeBSD__
    hostData->hostOS = "FreeBSD";
#endif

    hostData->majorSDLVer = static_cast<int>(compiled.major);
    hostData->minorSDLVer = static_cast<int>(compiled.minor);
    hostData->majorSDLVer = static_cast<int>(compiled.patch);
    hostData->linkedMajorSDLVer = static_cast<int>(linked.major);
    hostData->linkedMinorSDLVer = static_cast<int>(linked.minor);
    hostData->linkedPatchSDLVer = static_cast<int>(linked.patch);
    hostData->glewVer = glewGetString(GLEW_VERSION);
    hostData->glVer = glGetString(GL_VERSION);
    hostData->glVendor = glGetString(GL_VENDOR);
    hostData->glRenderer = glGetString(GL_RENDERER);
    hostData->glslVer = glGetString(GL_SHADING_LANGUAGE_VERSION);
    hostData->glMaxVer = glGetString(GL_VERSION);

    hostData->printDeviceInfo();

    SGL_Log("<--- Display data --->", LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_GREEN);
    // Display mode data
    int currentDisplay = pWindowCreationSpecs.displayID, displayModeCount;
    u_int32_t format;
    SDL_DisplayMode dMode;
    SGL_Log("Display(s) available: " + std::to_string(SDL_GetNumVideoDisplays()), LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_DEFAULT);
    displayModeCount = SDL_GetNumDisplayModes(currentDisplay);
    if (displayModeCount < 1)
        throw SGL_Exception("int SGL_Window::start | Failed to retrieve display mode data. \n ");

    SGL_Log("Total display modes: " + std::to_string(displayModeCount), LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_DEFAULT);
    for (int i = 0; i < displayModeCount; ++i)
    {
        if (SDL_GetDisplayMode(currentDisplay, i,  &dMode) != 0)
            throw SGL_Exception("int SGL_Window::start | Failed to get display. \n ");
        format = dMode.format;
        SGL_Log("Display mode: " + std::to_string(i) + "bpp: " + std::to_string(SDL_BITSPERPIXEL(format)) + " format name: " + SDL_GetPixelFormatName(format) +
                    " mode width: " + std::to_string(dMode.w) + " mode height: " +
                    std::to_string(dMode.h), LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_DEFAULT);
    }
    // Current video display
    for (int i = 0; i < SDL_GetNumVideoDisplays(); ++i)
    {
        if (SDL_GetCurrentDisplayMode(i, &dMode) != 0)
            throw SGL_Exception("int SGL_Window::start | Failed to get video display mode. \n ");
        else
        {
            pWindowCreationSpecs.maxW = dMode.w;
            pWindowCreationSpecs.maxH = dMode.h;
            if (i == currentDisplay)
            {
                pWindowCreationSpecs.fullscreenW = dMode.w;
                pWindowCreationSpecs.fullscreenH = dMode.h;
            }
            SGL_Log("Display #" + std::to_string(i) +
                        " current display mode is " + std::to_string(dMode.w) +
                        " x " + std::to_string(dMode.h) +
                        "@ " + std::to_string(dMode.refresh_rate) + "hz", LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_DEFAULT);
        }
    }
    SGL_Log("Using display #" + std::to_string(currentDisplay), LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_DEFAULT);
    pWindowCreationSpecs.displayID = currentDisplay;

    // --- DEFAULT ASSET LOADING ---
    this->checkGamepads(pWindowCreationSpecs.gamepad1);
    this->pLoadDefaultAssets();

    // --- RENDERER & CAMERA CONFIGURATION ----
    SGL_Log("<--- Engine configuration --->", LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_GREEN);
    // Initialize the FBO
    // NOTE: if this function is skipped, it may still work since the resizeGL function reloads the FBO
    // however this only works under certain compilers that correctly load the FBO pointer, if run in gdb
    // it will always crash
    this->startFBO(assetManager->getShader(SGL::DEFAULT_FRAMEBUFFER_SHADER));
    // Set internal opengl viewport size and reconfigure the framebuffer shader
    this->resizeGL(pWindowCreationSpecs.currentW, pWindowCreationSpecs.currentH);
    // Initialize the camera module (required for the projection matrix that be will parsed to the shaders)
    this->pCamera = new SGL_Camera(pOGLM);
    this->pCamera->initialize(getWindowCreationSpecs().internalW, getWindowCreationSpecs().internalH);
    // Update orthographic shaders
    this->setCameraMode(CAMERA_MODE::DEFAULT);
    SGL_Log("Orthographic shader width: " + std::to_string(this->getWindowCreationSpecs().internalW), LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_DEFAULT);
    SGL_Log("Orthographic shader height: " + std::to_string(this->getWindowCreationSpecs().internalH), LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_DEFAULT);
    SGL_Log("Camera and orthographic shader configured.", LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_DEFAULT);
    // Configure renderer
    //this->pClearScreen = {1.0f, 1.0f, 1.0f, 1.0f};
    renderer = std::make_unique<SGL_Renderer>(pOGLM, assetManager->getTexture(SGL::DEFAULT_TEXTURE),
                                              assetManager->getTexture(SGL::INVISIBLE_TEXTURE),
                                              assetManager->getTexture(SGL::DEFAULT_BMP_FONT_TEXTURE),
                                              assetManager->getShader(SGL::DEFAULT_PIXEL_SHADER),
                                              assetManager->getShader(SGL::DEFAULT_PIXEL_BATCH_SHADER),
                                              assetManager->getShader(SGL::DEFAULT_LINE_SHADER),
                                              assetManager->getShader(SGL::DEFAULT_LINE_BATCH_SHADER),
                                              assetManager->getShader(SGL::DEFAULT_CIRCLE_SHADER),
                                              assetManager->getShader(SGL::DEFAULT_CIRCLE_BATCH_SHADER),
                                              assetManager->getShader(SGL::DEFAULT_SPRITE_SHADER),
                                              assetManager->getShader(SGL::DEFAULT_SPRITE_BATCH_SHADER),
                                              assetManager->getShader(SGL::DEFAULT_TEXT_SHADER));

    SGL_Log("Renderer configured.", LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_DEFAULT);
    // Setup the post processor
    this->startFBO(pDefaultPPShader);
    pPostProcessorFBO->setClearColor(pClearScreen);
    SGL_Log("Post-processor initialized.", LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_DEFAULT);

    // Set the default window icon
    Uint16 pixels[16*16] = { // Will later upgrado to use images too
        0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff,
        0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff,
        0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff,
        0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff,
        0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff,
        0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff,
        0x0fff, 0x0aab, 0x0789, 0x0bcc, 0x0eee, 0x09aa, 0x099a, 0x0ddd,
        0x0fff, 0x0eee, 0x0899, 0x0fff, 0x0fff, 0x1fff, 0x0dde, 0x0dee,
        0x0fff, 0xabbc, 0xf779, 0x8cdd, 0x3fff, 0x9bbc, 0xaaab, 0x6fff,
        0x0fff, 0x3fff, 0xbaab, 0x0fff, 0x0fff, 0x6689, 0x6fff, 0x0dee,
        0xe678, 0xf134, 0x8abb, 0xf235, 0xf678, 0xf013, 0xf568, 0xf001,
        0xd889, 0x7abc, 0xf001, 0x0fff, 0x0fff, 0x0bcc, 0x9124, 0x5fff,
        0xf124, 0xf356, 0x3eee, 0x0fff, 0x7bbc, 0xf124, 0x0789, 0x2fff,
        0xf002, 0xd789, 0xf024, 0x0fff, 0x0fff, 0x0002, 0x0134, 0xd79a,
        0x1fff, 0xf023, 0xf000, 0xf124, 0xc99a, 0xf024, 0x0567, 0x0fff,
        0xf002, 0xe678, 0xf013, 0x0fff, 0x0ddd, 0x0fff, 0x0fff, 0xb689,
        0x8abb, 0x0fff, 0x0fff, 0xf001, 0xf235, 0xf013, 0x0fff, 0xd789,
        0xf002, 0x9899, 0xf001, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0xe789,
        0xf023, 0xf000, 0xf001, 0xe456, 0x8bcc, 0xf013, 0xf002, 0xf012,
        0x1767, 0x5aaa, 0xf013, 0xf001, 0xf000, 0x0fff, 0x7fff, 0xf124,
        0x0fff, 0x089a, 0x0578, 0x0fff, 0x089a, 0x0013, 0x0245, 0x0eff,
        0x0223, 0x0dde, 0x0135, 0x0789, 0x0ddd, 0xbbbc, 0xf346, 0x0467,
        0x0fff, 0x4eee, 0x3ddd, 0x0edd, 0x0dee, 0x0fff, 0x0fff, 0x0dee,
        0x0def, 0x08ab, 0x0fff, 0x7fff, 0xfabc, 0xf356, 0x0457, 0x0467,
        0x0fff, 0x0bcd, 0x4bde, 0x9bcc, 0x8dee, 0x8eff, 0x8fff, 0x9fff,
        0xadee, 0xeccd, 0xf689, 0xc357, 0x2356, 0x0356, 0x0467, 0x0467,
        0x0fff, 0x0ccd, 0x0bdd, 0x0cdd, 0x0aaa, 0x2234, 0x4135, 0x4346,
        0x5356, 0x2246, 0x0346, 0x0356, 0x0467, 0x0356, 0x0467, 0x0467,
        0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff,
        0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff,
        0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff,
        0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff
    };
    this->setWindowIcon(pixels);

    pDeltaTimeMS = 0.0f;

    SGL_Log("<--- SkeletonGL rendering engine successfully loaded and configured --->", LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_GREEN);
    pAlreadyInitialized = true;
}

/**
 * @brief Ask the camera to update, won't if unnecessary
 * @param forceUpdate Forces the update to take place
 * @return nothing
 */
void SGL_Window::updateCamera(bool forceUpdate)
{
    this->pCamera->updateCamera(forceUpdate);
}

/**
 * @brief Set the camera position
 * @param pos glm::vec2 position vector
 * @return nothing
 */
void SGL_Window::setCameraPosition(glm::vec2 pos)
{
    this->pCamera->setPosition(pos);
}


/**
 * @brief Scale the camera to a new size
 * @param newScale Values outside 1.0 and -1.0 will cause visual glitches
 * @return nothing
 */
void SGL_Window::setCameraScale(float newScale)
{
    this->pCamera->setScale(newScale);
}


/**
 * @brief Scale the console output filter level
 * @param LogLeveL [DEBUG, ERROR, SGL_DEBUG]
 * @return nothing
 */
void SGL_Window::setLogLevel(LOG_LEVEL logLevel)
{
    SGL_STDOUT_FILTER = logLevel;
}



/**
 * @brief Toggles the camera mode between DEFAULT and OVERLAY and updates the PP shaders
 * @param mode Mode to be implemented
 * @return nothing
 */
void SGL_Window::setCameraMode(CAMERA_MODE mode)
{
    /*  Camera is active, apply the offset to the projection matrix */
    // Attempt to iterate all the required shaders, the PP doesn't require any MVP transformations
    // since it simply blits the final FBO texture to the screen
    for (auto iter = assetManager->shaderTypes.begin(); iter != assetManager->shaderTypes.end(); ++iter)
    {
        if (assetManager->getShader((*iter).first).shaderType != SHADER_TYPE::POST_PROCESSOR)
        {
            this->pCamera->cameraMode(assetManager->getShader((*iter).first), mode);
            //SGL_Log("Updating shader: " + (*iter).first);
        }
    }
}


/**
 * @brief Returns the horizontal position of the camera
 * @param mode Mode to be implemented
 * @return int Horizontal camera position
 */
int SGL_Window::getCameraPosX() const
{
    return static_cast<int>(this->pCamera->getPosition().x);
}


/**
 * @brief Returns the vertical position of the camera
 * @return int Vertical camera position
 */
int SGL_Window::getCameraPosY() const
{
    return static_cast<int>(this->pCamera->getPosition().y);
}


/**
 * @brief Returns the width of the camera
 * @return int Camera's width
 */
int SGL_Window::getCameraPosW() const
{
    return static_cast<int>(this->pCamera->getPosition().z);
}

/**
 * @brief Returns the height of the camera
 * @return int Camera's height
 */
int SGL_Window::getCameraPosH() const
{
    return static_cast<int>(this->pCamera->getPosition().w);
}

/**
 * @brief Set the OpenGL clear screen color
 * @param color The new clear screen color (alpha is ignored)
 */
void SGL_Window::setClearColor(const SGL_Color &color)
{
    pClearScreen = {color.r, color.g, color.b, 1.0f};
    pPostProcessorFBO->setClearColor(pClearScreen);
}


/**
 * @brief Change the Window title
 * @param newTitle String to be used
 */
void SGL_Window::setWindowTitle(const std::string &newTitle)
{
    SDL_SetWindowTitle(pWindow, newTitle.c_str());
}


/**
 * @brief Get the current WCS (note that these may be different to the one originally submitted)
 * @return WindowCreationSpecs Note the .ini file settings have priority
 */
WindowCreationSpecs SGL_Window::getWindowCreationSpecs()
{
    return this->pWindowCreationSpecs;
}

/**
 * @brief Change the program's icon (displayed in taskbars and window menu bars)
 * @param pixelMatrix Must be exactly 255 bytes long
 * @return nothing
 */
void SGL_Window::setWindowIcon(Uint16 pixelMatrix[16*16])
{
    SDL_Surface *surface;
    surface = SDL_CreateRGBSurfaceFrom(pixelMatrix,16,16,16,16*2,0x0f00,0x00f0,0x000f,0xf000);
    SDL_SetWindowIcon(this->pWindow, surface);
    SDL_FreeSurface(surface);
}

//NOTE: if the the new window size isn't internally rescaled the camera must
//be adjusted

/**
 * @brief Scales the main window, unavailable in fullscreen, scaling internal resolution resizes the glViewPort
 * @param w New window width
 * @param h New window height
 * @param scaleInternalResolution Resize the glViewPort?
 * @return nothing
 */
void SGL_Window::resizeWindow(int w, int h, bool scaleInternalResolution)
{
    if (w >= pWindowCreationSpecs.minW && h >= pWindowCreationSpecs.minH)
    {
        if (pWindowCreationSpecs.fullScreen)
        {
            SGL_Log("Can't resize window on fullscreen mode.", LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_DEFAULT);
            return;
        }
        SGL_Log("Resized window to w: " + std::to_string(w) + " h: " + std::to_string(h), LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_DEFAULT);
        pWindowCreationSpecs.currentW = w;
        pWindowCreationSpecs.currentH = h;
        SDL_SetWindowSize(this->pWindow, w, h);
        if (scaleInternalResolution)
            this->resizeGL(w, h);
    }
    else
    {
        SGL_Log("Window can't be smaller than w: " + std::to_string(pWindowCreationSpecs.minW) +
                    " h: " + std::to_string(pWindowCreationSpecs.minH), LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_DEFAULT);
    }
}

/**
 * @brief Does the window have the user's keyboard focus?
 * @return bool
 */
bool SGL_Window::hasKeyboardFocus() const
{
    return pHasKeyboardFocus;
}


/**
 * @brief Does the window have the user's mouse focus?
 * @return bool
 */
bool SGL_Window::hasMouseFocus() const
{
    return pHasMouseFocus;
}

// Check if there are any gamepads available

/**
 * @brief Check if there are any gamepads available
 * @param gamepadID ID of the gamepad to use as default
 * @return nothing
 */
void SGL_Window::checkGamepads(int gamepadID)
{
    pGamepad_1 = NULL;
    SGL_Log("Checking for available gamepads...");
    int totalGamepadsFound = SDL_NumJoysticks();
    if( totalGamepadsFound == 0 )
    {
        SGL_Log("No gamepads detected. " + std::string(SDL_GetError()));
        return;
    }
    else if (gamepadID != 0)
    {
        for (int i = 0; i < totalGamepadsFound; ++i)
        {
            SDL_Joystick *tmpGamepad = SDL_JoystickOpen(i);
            if (tmpGamepad)
            {
                SDL_JoystickGUID guid = SDL_JoystickGetGUID(tmpGamepad);
                char guidStr[1024];
                SDL_JoystickGetGUIDString(guid, guidStr, sizeof(guidStr));
                const char *gamepadName = SDL_JoystickName(tmpGamepad);
                std::string guidString {guidStr};
                std::string nameString {gamepadName};

                int axes = SDL_JoystickNumAxes(tmpGamepad);
                int buttons = SDL_JoystickNumButtons(tmpGamepad);
                int hats = SDL_JoystickNumHats(tmpGamepad);
                int balls = SDL_JoystickNumBalls(tmpGamepad);

                SGL_Log("Gamepad #" + std::to_string(i+1) + " name: " + nameString + " ID:" + guidString + "  axes: " + std::to_string(axes) +
                        " buttons: " + std::to_string(buttons) +
                        " hats: " + std::to_string(hats) +
                        " balls: " + std::to_string(balls));
                SDL_JoystickClose(tmpGamepad);
                // We know gamepadID isn't 0, however SDL starts counting found gamepads from 0
                if ((gamepadID - 1) == i)
                {
                    this->pGamepad_1 = SDL_JoystickOpen(i);
                    if (pGamepad_1 == NULL)
                        SGL_Log("Failed to load gamepad.");
                    else
                        SGL_Log("Gamepad #" + std::to_string(gamepadID) + " loaded.");

                    return;
                }
            }
        }
    }
}

/**
 * @brief Get the input polled by this particular frame
 * @return SDL_InputFrame Contains all the collective user input
 */
SGL_InputFrame SGL_Window::getFrameInput()
{
    // Feel free to remove all the input your program won't need from the checklist
    SGL_InputFrame input;

    // -- MOUSE --
    // Mouse position relative to the window dimensions (RAW)
    SDL_GetMouseState(&input.mouse.cursorX, &input.mouse.cursorY);
    // Mouse position relative to the internal rendering resolution
    // Get the position in percent relative to the current resolution
    float xPos = input.mouse.cursorX / static_cast<float>(pWindowCreationSpecs.currentW);
    float yPos = input.mouse.cursorY / static_cast<float>(pWindowCreationSpecs.currentH);
    // Get the position relative to the internal resoultion using the percent
    input.mouse.cursorXNormalized = std::floor(xPos * static_cast<float>(pWindowCreationSpecs.internalW));
    input.mouse.cursorYNormalized = std::floor(yPos * static_cast<float>(pWindowCreationSpecs.internalH));

    while(SDL_PollEvent(&pEvent) != 0)
    {
        // Note about SDL_QUIT:
        // This event can raise from a variety of different sources and is OS dependant, in UNIX systems
        // all signals (SIGINT, SIGTERM etc.) that aren't directly addressed will also throw an SDL_QUIT
        // signal. SDL_WINDOWEVENT_CLOSE is a reliable alternative to handling user requested program
        // termination.

        if( pEvent.type == SDL_QUIT ) //{ input.sdlInternalQuit = true; }
            SGL_Log("SDL_QUIT event received.", LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_DEFAULT);
        if( pEvent.type == SDL_WINDOWEVENT )
        {
            switch(pEvent.window.event) // --- WINDOW EVENTS ---
            {
            case SDL_WINDOWEVENT_RESIZED: // Resize the specs and the glViewPort
                pWindowCreationSpecs.currentW = pEvent.window.data1;
                pWindowCreationSpecs.currentH = pEvent.window.data2;
                this->resizeGL(pEvent.window.data1, pEvent.window.data2);
                // this->calculateProjections();
                break;
            case SDL_WINDOWEVENT_MINIMIZED:
                SGL_Log("Window minimized: " + std::to_string(static_cast<int>(pEvent.window.windowID)), LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_DEFAULT);
                input.sdlInternalQuit.active = true;
                pIsMinimized = true;
                break;
            case SDL_WINDOWEVENT_RESTORED:
                SGL_Log("Window restored: " + std::to_string(static_cast<int>(pEvent.window.windowID)), LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_DEFAULT);
                input.windowMinimized.active = false;
                input.windowRestored.active = true;
                pIsMinimized = false;
                break;
            case SDL_WINDOWEVENT_ENTER:
                SGL_Log("Window gained mouse focus: " + std::to_string(static_cast<int>(pEvent.window.windowID)), LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_DEFAULT);
                input.mouseFocus.active = true;
                pHasMouseFocus = true;
                break;
            case SDL_WINDOWEVENT_LEAVE:
                SGL_Log("Window lost mouse focus: " + std::to_string(static_cast<int>(pEvent.window.windowID)), LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_DEFAULT);
                input.mouseFocus.active = false;
                pHasMouseFocus = false;
                break;
            case SDL_WINDOWEVENT_FOCUS_GAINED:
                SGL_Log("Window gained keyboard focus: " + std::to_string(static_cast<int>(pEvent.window.windowID)), LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_DEFAULT);
                input.keyboardFocus.active = true;
                pHasKeyboardFocus = true;
                break;
            case SDL_WINDOWEVENT_FOCUS_LOST:
                SGL_Log("Window lost keyboard focus: " + std::to_string(static_cast<int>(pEvent.window.windowID)), LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_DEFAULT);
                input.keyboardFocus.active = true;
                pHasKeyboardFocus = false;
                break;
            case SDL_WINDOWEVENT_CLOSE:
                SGL_Log("Window has requested to exit: " + std::to_string(static_cast<int>(pEvent.window.windowID)), LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_DEFAULT);
                input.sdlInternalQuit.active = true;
                break;
            }
        }
        // --- MOUSE --

        if (pEvent.type == SDL_MOUSEBUTTONDOWN)
        {
            if (pEvent.button.button == SDL_BUTTON_MIDDLE)
            { input.mouse.middleBtn.pressed = true; }
            if (pEvent.button.button == SDL_BUTTON_LEFT)
            { input.mouse.leftBtn.pressed = true; }
            if (pEvent.button.button == SDL_BUTTON_RIGHT)
            { input.mouse.rightBtn.pressed = true; }
        }
        if (pEvent.type == SDL_MOUSEBUTTONUP)
        {
            if (pEvent.button.button == SDL_BUTTON_MIDDLE)
            { input.mouse.middleBtn.released = true; }
            if (pEvent.button.button == SDL_BUTTON_LEFT)
            { input.mouse.leftBtn.released = true; }
            if (pEvent.button.button == SDL_BUTTON_RIGHT)
            { input.mouse.rightBtn.released = true; }
        }
        if (pEvent.type == SDL_MOUSEWHEEL)
        {
            if (pEvent.wheel.y > 0) // Scroll up
                input.mouse.scrollUp.pressed = true;
            if (pEvent.wheel.y < 0) // Scroll down
                input.mouse.scrollDown.pressed = true;
        }

        // --- KEYBOARD ---
        //NOTE: Its best to place keys that shouldn't be held down like a toggle button
        if(pEvent.type == SDL_KEYDOWN && pEvent.key.repeat == 0)
        {
            switch(pEvent.key.keysym.sym)
            {
            case SDLK_ESCAPE:
                input.esc.pressed = true;
                break;
            case SDLK_SPACE:
                input.space.pressed = true;
                break;
            case SDLK_RETURN: // Scancode for the enter key don't seem to work
                input.enter.pressed = true;
                break;
            case SDLK_RETURN2:
                input.enter.pressed = true;
                break;
            case SDLK_F1:
                SDL_ShowCursor(1);
                break;
            case SDLK_F2:
                SDL_ShowCursor(0);
                break;
            }
        }
    }

    // Keyboard key states
    const Uint8 *state = SDL_GetKeyboardState(NULL);
    // Alphabet (qwerty)
    // First row
    if (state[SDL_SCANCODE_Q]) { input.q.pressed = true; }
    if (state[SDL_SCANCODE_W]) { input.w.pressed = true; }
    if (state[SDL_SCANCODE_E]) { input.e.pressed = true; }
    if (state[SDL_SCANCODE_R]) { input.r.pressed = true; }
    if (state[SDL_SCANCODE_T]) { input.t.pressed = true; }
    if (state[SDL_SCANCODE_Y]) { input.y.pressed = true; }
    if (state[SDL_SCANCODE_U]) { input.u.pressed = true; }
    if (state[SDL_SCANCODE_I]) { input.i.pressed = true; }
    if (state[SDL_SCANCODE_O]) { input.o.pressed = true; }
    if (state[SDL_SCANCODE_P]) { input.p.pressed = true; }
    // Second row
    if (state[SDL_SCANCODE_A]) { input.a.pressed = true; }
    if (state[SDL_SCANCODE_S]) { input.s.pressed = true; }
    if (state[SDL_SCANCODE_D]) { input.d.pressed = true; }
    if (state[SDL_SCANCODE_F]) { input.f.pressed = true; }
    if (state[SDL_SCANCODE_G]) { input.g.pressed = true; }
    if (state[SDL_SCANCODE_H]) { input.h.pressed = true; }
    if (state[SDL_SCANCODE_J]) { input.j.pressed = true; }
    if (state[SDL_SCANCODE_K]) { input.k.pressed = true; }
    if (state[SDL_SCANCODE_L]) { input.l.pressed = true; }
    // Third row
    if (state[SDL_SCANCODE_Z]) { input.z.pressed = true; }
    if (state[SDL_SCANCODE_X]) { input.x.pressed = true; }
    if (state[SDL_SCANCODE_C]) { input.c.pressed = true; }
    if (state[SDL_SCANCODE_V]) { input.v.pressed = true; }
    if (state[SDL_SCANCODE_B]) { input.b.pressed = true; }
    if (state[SDL_SCANCODE_N]) { input.n.pressed = true; }
    if (state[SDL_SCANCODE_M]) { input.m.pressed = true; }
    // Arrow keys
    if (state[SDL_SCANCODE_UP]) { input.up.pressed = true; }
    if (state[SDL_SCANCODE_DOWN]) { input.down.pressed = true; }
    if (state[SDL_SCANCODE_LEFT]) { input.left.pressed = true; }
    if (state[SDL_SCANCODE_RIGHT]) { input.right.pressed = true; }
    // Numbers
    if (state[SDL_SCANCODE_1]) { input.num1.pressed = true; }
    if (state[SDL_SCANCODE_2]) { input.num2.pressed = true; }
    if (state[SDL_SCANCODE_3]) { input.num3.pressed = true; }
    if (state[SDL_SCANCODE_4]) { input.num4.pressed = true; }
    if (state[SDL_SCANCODE_5]) { input.num5.pressed = true; }
    if (state[SDL_SCANCODE_6]) { input.num6.pressed = true; }
    if (state[SDL_SCANCODE_7]) { input.num7.pressed = true; }
    if (state[SDL_SCANCODE_8]) { input.num8.pressed = true; }
    if (state[SDL_SCANCODE_9]) { input.num9.pressed = true; }
    // Modifiers
    if (state[SDL_SCANCODE_BACKSPACE]) { input.backspace.pressed = true; }
    // if (state[SDL_SCANCODE_ESCAPE]) { input.esc = true; }
    if (state[SDL_SCANCODE_LCTRL]) { input.ctrl.pressed = true; }
    if (state[SDL_SCANCODE_LALT]) { input.alt.pressed = true; }
    if (state[SDL_SCANCODE_LSHIFT]) { input.shift.pressed = true; }
    if (state[SDL_SCANCODE_KP_ENTER]) { input.enter.pressed = true; }
    if (state[SDL_SCANCODE_KP_SPACE]) { input.space.pressed = true; }
    // if (state[SDL_SCANCODE_ENTER || SDL_SCANCODE_ENTER2]) { input.enter = true; }
    if (state[SDL_SCANCODE_RETURN || SDL_SCANCODE_RETURN2]) { input.enter.pressed = true; }

    // Save the current input as delta to compare
    pDeltaInput = input;
    return input;
}

/**
 * @brief Update the glViewPort and post-processor framebuffer dimensions
 * @param width New internal rendering width
 * @param height New internal rendering height
 * @return nothing
 */
void SGL_Window::resizeGL(int width, int heigth)
{
    pOGLM->resizeGL(0, 0, width, heigth);
    if (this->pPostProcessorFBO != nullptr)
    {
        SGL_Log("Reloading main frame buffer...", LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_DEFAULT);
        this->pPostProcessorFBO->reload(width, heigth);
        SGL_Log("Done.", LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_DEFAULT);
    }
}


/**
 * @brief Configure and instantiate the post-processor
 * @param SGL_Shader The framebuffer shader
 * @return nothing
 */
void SGL_Window::startFBO(const SGL_Shader &shader)
{
    // Destroy previous FBO if it exists
    SGL_Log("Starting the post-processor FBO.", LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_DEFAULT);
    this->checkForErrors();
    // Start the FBO
    this->pPostProcessorFBO = new SGL_PostProcessor(pOGLM, shader);
    this->pPostProcessorFBO->reload(pWindowCreationSpecs.currentW, pWindowCreationSpecs.currentH);
    this->checkForErrors();
    SGL_Log("Done", LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_DEFAULT);
}


/**
 * @brief Take an initial timestamp of the frame to be processed and reload the post processor
 * @return nothing
 */
void SGL_Window::startFrame()
{
    // C++11 (chrono) based timer
    // pChrono0 = std::chrono::high_resolution_clock::now();
    pChrono0 = std::chrono::steady_clock::now();


    // Begin FBO operations
    this->pPostProcessorFBO->beginRender();
}


/**
 * @brief Calculates delta times, renders the current state of the post processor and swaps the renderer's window
 * @return nothing
 */
void SGL_Window::endFrame()
{
    // End all rendering operations
    this->pPostProcessorFBO->endRender();

    // pChrono1 = std::chrono::high_resolution_clock::now();
    pChrono1 = std::chrono::steady_clock::now();

    auto timeDiff = std::chrono::duration_cast<std::chrono::nanoseconds>(pChrono1 - pChrono0);
    pDeltaTimeMS = timeDiff.count();
    // Nanoseconds to seconds
    pDeltaTimeMS /= 1000000000;

    pDefaultPPShader.renderDetails.deltaTime = pDeltaTimeMS;
    pDefaultPPShader.renderDetails.mousePosX = pDeltaInput.mouse.cursorXNormalized;
    pDefaultPPShader.renderDetails.mousePosY = pDeltaInput.mouse.cursorYNormalized;

    // Render final FBO texture
    this->pPostProcessorFBO->render(pDefaultPPShader);
    // Finally, swap the buffers
    SDL_GL_SwapWindow(this->pWindow);
}


/**
 * @brief Returns the time it took to render the frame
 * @return double Delta time
 */
double SGL_Window::getRenderDeltaTime() const
{
    return pDeltaTimeMS;
    //return pChronoDeltaTime;
}

/**
 * @brief Toggle the screen shake post processor effect
 * @param shakeit Bool to enable / disable
 * @return nothing
 */
void SGL_Window::setShakeScreen(bool shakeit)
{
    if (shakeit)
        this->pPostProcessorFBO->shake = GL_TRUE;
    else
        this->pPostProcessorFBO->shake = GL_FALSE;
}

/**
 * @brief Toggle the screen chaos post processor effect
 * @param chaos Bool to enable / disable
 * @return nothing
 */
void SGL_Window::setChaosEffect(bool chaos)
{
    if (chaos)
        this->pPostProcessorFBO->chaos = GL_TRUE;
    else
        this->pPostProcessorFBO->chaos = GL_FALSE;
}

/**
 * @brief Toggle the screen confuse post processor effect
 * @param confuse Bool to enable / disable
 * @return nothing
 */
void SGL_Window::setConfuseEffect(bool confuse)
{
    if (confuse)
        this->pPostProcessorFBO->confuse = GL_TRUE;
    else
        this->pPostProcessorFBO->confuse = GL_FALSE;
}

/**
 * @brief Check for any potential errors, useful to debug a particular render or gl process
 * @return nothing
 */
void SGL_Window::checkForErrors() const
{
    pOGLM->checkForGLErrors();
}

/**
 * @brief SGL internal media is to be loaded here, place your custom assets elsewhere to prevent mistakes
 * @return nothing
 */
void SGL_Window::pLoadDefaultAssets()
{
    SGL_Log("<--- Default assets --->", LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_GREEN);
    assetManager = std::make_unique<SGL_AssetManager>(pOGLM);

    assetManager->loadTexture(FOLDER_STRUCTURE::DEFAULT_TEXTURE_FILE.c_str(), GL_FALSE, SGL::DEFAULT_TEXTURE);
    assetManager->loadTexture(FOLDER_STRUCTURE::DEFAULT_INVISIBLE_TEXTURE_FILE.c_str(), GL_TRUE, SGL::INVISIBLE_TEXTURE);
    assetManager->loadTexture(FOLDER_STRUCTURE::DEFAULT_SQUARE_TEXTURE_FILE.c_str(), GL_TRUE, SGL::SQUARE_TEXTURE);
    assetManager->loadTexture(FOLDER_STRUCTURE::DEFAULT_BMP_FONT_TEXTURE_FILE.c_str(), GL_TRUE, SGL::DEFAULT_BMP_FONT_TEXTURE);

    SGL_Log("Default textures loaded.", LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_DEFAULT);

    pOGLM->checkForGLErrors();
    // SGL_Log("Loading default shaders...", LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_DEFAULT);
    // Load default shaders
    // std::string spriteUVV = FOLDER_STRUCTURE::shadersDir + "spriteUVV.c";
    // std::string spriteUVF = FOLDER_STRUCTURE::shadersDir + "spriteUVF.c";
    // std::string spriteBatchUVV = FOLDER_STRUCTURE::shadersDir + "spriteBatchUVV.c";
    // std::string spriteBatchUVF = FOLDER_STRUCTURE::shadersDir + "spriteBatchUVF.c";
    // std::string textV = FOLDER_STRUCTURE::shadersDir + "textV.c";
    // std::string textF = FOLDER_STRUCTURE::shadersDir + "textF.c";
    // std::string lineV = FOLDER_STRUCTURE::shadersDir + "lineV.c";
    // std::string lineF = FOLDER_STRUCTURE::shadersDir + "lineF.c";
    // std::string FBOshaderV = FOLDER_STRUCTURE::shadersDir + "frameBufferV.c";
    // std::string FBOshaderF = FOLDER_STRUCTURE::shadersDir + "frameBufferF.c";
    // std::string pixelV = FOLDER_STRUCTURE::shadersDir + "pixelV.c";
    // std::string pixelF = FOLDER_STRUCTURE::shadersDir + "pixelF.c";
    // std::string pixelBatchV = FOLDER_STRUCTURE::shadersDir + "pixelBatchV.c";
    // std::string pixelBatchF = FOLDER_STRUCTURE::shadersDir + "pixelBatchF.c";
    // std::string lineBatchV = FOLDER_STRUCTURE::shadersDir + "lineBatchV.c";
    // std::string lineBatchF = FOLDER_STRUCTURE::shadersDir + "lineBatchF.c";
    // std::string circleV = FOLDER_STRUCTURE::shadersDir + "circleV.c";
    // std::string circleF = FOLDER_STRUCTURE::shadersDir + "circleF.c";
    // std::string circleBatchV = FOLDER_STRUCTURE::shadersDir + "circleBatchV.c";
    // std::string circleBatchF = FOLDER_STRUCTURE::shadersDir + "circleBatchF.c";


    // assetManager->loadShaders(spriteUVV.c_str(), spriteUVF.c_str(), nullptr, "spriteUV", SHADER_TYPE::SPRITE);
    // assetManager->loadShaders(spriteBatchUVV.c_str(), spriteBatchUVF.c_str(), nullptr, "spriteBatchUV", SHADER_TYPE::SPRITE_BATCH);
    // assetManager->loadShaders(textV.c_str(), textF.c_str(), nullptr, "text", SHADER_TYPE::TEXT);
    // assetManager->loadShaders(lineV.c_str(), lineF.c_str(), nullptr, "line", SHADER_TYPE::LINE);
    // assetManager->loadShaders(pixelV.c_str(), pixelF.c_str(), nullptr, "pixel", SHADER_TYPE::PIXEL);
    // assetManager->loadShaders(pixelBatchV.c_str(), pixelBatchF.c_str(), nullptr, "pixelBatch", SHADER_TYPE::PIXEL_BATCH);
    // assetManager->loadShaders(lineBatchV.c_str(), lineBatchF.c_str(), nullptr, "lineBatch", SHADER_TYPE::LINE_BATCH);
    // assetManager->loadShaders(FBOshaderV.c_str(), FBOshaderF.c_str(), nullptr, "postProcessor", SHADER_TYPE::POST_PROCESSOR);


    // // Default sprite shaders
    // std::string spriteV = FOLDER_STRUCTURE::shadersDir + "spriteV.c";
    // std::string spriteF = FOLDER_STRUCTURE::shadersDir + "spriteF.c";
    // std::string spriteBatchV = FOLDER_STRUCTURE::shadersDir + "spriteBatchV.c";
    // std::string spriteBatchF = FOLDER_STRUCTURE::shadersDir + "spriteBatchF.c";
    // // Default pixel shaders
    // std::string pixelV = FOLDER_STRUCTURE::shadersDir + "pixelV.c";
    // std::string pixelF = FOLDER_STRUCTURE::shadersDir + "pixelF.c";
    // std::string pixelBatchV = FOLDER_STRUCTURE::shadersDir + "pixelBatchV.c";
    // std::string pixelBatchF = FOLDER_STRUCTURE::shadersDir + "pixelBatchF.c";
    // // Default line shaders
    // std::string lineV = FOLDER_STRUCTURE::shadersDir + "lineV.c";
    // std::string lineF = FOLDER_STRUCTURE::shadersDir + "lineF.c";
    // std::string lineBatchV = FOLDER_STRUCTURE::shadersDir + "lineBatchV.c";
    // std::string lineBatchF = FOLDER_STRUCTURE::shadersDir + "lineBatchF.c";
    // // Default circle shaders
    // std::string circleV = FOLDER_STRUCTURE::shadersDir + "circleV.c";
    // std::string circleF = FOLDER_STRUCTURE::shadersDir + "circleF.c";
    // std::string circleBatchV = FOLDER_STRUCTURE::shadersDir + "circleBatchV.c";
    // std::string circleBatchF = FOLDER_STRUCTURE::shadersDir + "circleBatchF.c";
    // // Default text shaders
    // std::string textV = FOLDER_STRUCTURE::shadersDir + "textV.c";
    // std::string textF = FOLDER_STRUCTURE::shadersDir + "textF.c";
    // // Default FBO shaders
    // std::string fboV = FOLDER_STRUCTURE::shadersDir + "frameBufferV.c";
    // std::string fboF = FOLDER_STRUCTURE::shadersDir + "frameBufferF.c";

    assetManager->loadShaders(FOLDER_STRUCTURE::SPRITE_SHADER_V_FILE.c_str(),
                              FOLDER_STRUCTURE::SPRITE_SHADER_F_FILE.c_str(),
                              nullptr, SGL::DEFAULT_SPRITE_SHADER, SHADER_TYPE::SPRITE);
    assetManager->loadShaders(FOLDER_STRUCTURE::SPRITE_BATCH_SHADER_V_FILE.c_str(),
                              FOLDER_STRUCTURE::SPRITE_BATCH_SHADER_F_FILE.c_str(),
                              nullptr, SGL::DEFAULT_SPRITE_BATCH_SHADER, SHADER_TYPE::SPRITE_BATCH);
    assetManager->loadShaders(FOLDER_STRUCTURE::TEXT_SHADER_V_FILE.c_str(),
                              FOLDER_STRUCTURE::TEXT_SHADER_F_FILE.c_str(),
                              nullptr, SGL::DEFAULT_TEXT_SHADER, SHADER_TYPE::TEXT);
    assetManager->loadShaders(FOLDER_STRUCTURE::LINE_SHADER_V_FILE.c_str(),
                              FOLDER_STRUCTURE::LINE_SHADER_F_FILE.c_str(),
                              nullptr, SGL::DEFAULT_LINE_SHADER, SHADER_TYPE::LINE);
    assetManager->loadShaders(FOLDER_STRUCTURE::PIXEL_SHADER_V_FILE.c_str(),
                              FOLDER_STRUCTURE::PIXEL_SHADER_F_FILE.c_str(),
                              nullptr, SGL::DEFAULT_PIXEL_SHADER, SHADER_TYPE::PIXEL);
    assetManager->loadShaders(FOLDER_STRUCTURE::PIXEL_BATCH_SHADER_V_FILE.c_str(),
                              FOLDER_STRUCTURE::PIXEL_BATCH_SHADER_F_FILE.c_str(),
                              nullptr, SGL::DEFAULT_PIXEL_BATCH_SHADER, SHADER_TYPE::PIXEL_BATCH);
    assetManager->loadShaders(FOLDER_STRUCTURE::LINE_BATCH_SHADER_V_FILE.c_str(),
                              FOLDER_STRUCTURE::LINE_BATCH_SHADER_F_FILE.c_str(),
                              nullptr, SGL::DEFAULT_LINE_BATCH_SHADER, SHADER_TYPE::LINE_BATCH);
    assetManager->loadShaders(FOLDER_STRUCTURE::FRAMEBUFFER_SHADER_V_FILE.c_str(),
                              FOLDER_STRUCTURE::FRAMEBUFFER_SHADER_F_FILE.c_str(),
                              nullptr, SGL::DEFAULT_FRAMEBUFFER_SHADER, SHADER_TYPE::POST_PROCESSOR);
    // Circle shaders are just custom sprite shaders that use a transparent texture as a canvas for the fragment shader to draw a circle on
    assetManager->loadShaders(FOLDER_STRUCTURE::CIRCLE_SHADER_V_FILE.c_str(),
                              FOLDER_STRUCTURE::CIRCLE_SHADER_F_FILE.c_str(),
                              nullptr, SGL::DEFAULT_CIRCLE_SHADER, SHADER_TYPE::SPRITE);
    assetManager->loadShaders(FOLDER_STRUCTURE::CIRCLE_BATCH_SHADER_V_FILE.c_str(),
                              FOLDER_STRUCTURE::CIRCLE_BATCH_SHADER_F_FILE.c_str(),
                              nullptr, SGL::DEFAULT_CIRCLE_BATCH_SHADER, SHADER_TYPE::SPRITE_BATCH);

    pDefaultPPShader = assetManager->getShader(SGL::DEFAULT_FRAMEBUFFER_SHADER);
    SGL_Log("Default shaders compiled and linked.", LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_DEFAULT);
    pOGLM->checkForGLErrors();
}

/**
 * @brief Toggle the window fullscreen feature
 * @param toggle Toggle fullscreen mode on or off
 * @return nothing
 * @section DESCRIPTION
 *
 * Enabling fullscreen requires the internal glViewPort to match the new size of the fullscreen window.
 * When applying the fullscreen transformation the resizeGL function must be called with the display's
 * max resolution to avoid blackbars
 */
void SGL_Window::toggleFullScreen(bool toggle)
{
    SGL_Log("TOGGLE FULLSCREEN.", LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_DEFAULT);

    if (toggle)
    {
        SDL_SetWindowFullscreen(this->pWindow, SDL_WINDOW_FULLSCREEN_DESKTOP);
        pWindowCreationSpecs.fullScreen = true;
        // If the internal resolution and the max display resolution aren't compatible, artifacts will form
        SDL_DisplayMode current;
        SDL_GetCurrentDisplayMode(0, &current);
        pWindowCreationSpecs.fullscreenW = current.w;
        pWindowCreationSpecs.fullscreenH = current.h;
        this->resizeGL(pWindowCreationSpecs.fullscreenW, pWindowCreationSpecs.fullscreenH);
        SGL_Log("Fullscreen enabled @ w: " + std::to_string(pWindowCreationSpecs.fullscreenW) + " h: " + std::to_string(pWindowCreationSpecs.fullscreenH), LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_DEFAULT);

    }
    else
    {
        SDL_SetWindowFullscreen(this->pWindow, 0);
        pWindowCreationSpecs.fullScreen = false;
        SGL_Log("Fullscreen disabled.", LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_DEFAULT);
    }


    // if (!toggle && pWindowCreationSpecs.fullScreen)
    // {
    //     SDL_SetWindowFullscreen(this->pWindow, 0);
    //     pWindowCreationSpecs.fullScreen = false;
    //     SGL_Log("Fullscreen disabled.", LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_DEFAULT);
    // }
    // if (toggle && !pWindowCreationSpecs.fullScreen)
    // {
    //     SDL_SetWindowFullscreen(this->pWindow, SDL_WINDOW_FULLSCREEN_DESKTOP);
    //     pWindowCreationSpecs.fullScreen = true;
    //     // If the internal resolution and the max display resolution aren't compatible, artifacts will form
    //     SDL_DisplayMode current;
    //     SDL_GetCurrentDisplayMode(0, &current);
    //     pWindowCreationSpecs.fullscreenW = current.w;
    //     pWindowCreationSpecs.fullscreenH = current.h;
    //     this->resizeGL(pWindowCreationSpecs.fullscreenW, pWindowCreationSpecs.fullscreenH);
    //     SGL_Log("Fullscreen enabled @ w: " + std::to_string(pWindowCreationSpecs.fullscreenW) + " h: " + std::to_string(pWindowCreationSpecs.fullscreenH), LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_DEFAULT);
    // }
}

/**
 * @brief Update the post-processor shader
 * @param SGL_Shader New FBO shader
 * @return nothing
 */
void SGL_Window::setPostProcessorShader(SGL_Shader shader)
{
    if (shader.shaderType == SHADER_TYPE::POST_PROCESSOR)
        pDefaultPPShader = shader;
}

void SGL_Window::toggleLineAA(bool toggle)
{
    if (toggle)
        pOGLM->enable(GL_LINE_SMOOTH);
    else
        pOGLM->disable(GL_LINE_SMOOTH);
}
