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
 * @file    src/skeletonGL/utility/SGL_DataStructures.hpp
 * @author  SENEX @ XENOBYTE.XYZ
 * @date    26/01/2021
 * @version 2.1
 *
 * @brief Contains most of the data structures used by SkeletonGL
 *
 * @section DESCRIPTION
 *
 * This file houses almost all the used data structurs, for easy access and clarity since
 * most of them are related
 */

#ifndef SGL_DATA_STRUCTURES
#define SGL_DATA_STRUCTURES

// C++
#include <string>
// SDL2
#include <SDL2/SDL.h>
// GLEW
#include <GL/glew.h>
// GLM
#include "../deps/glm/glm.hpp"
#include "../deps/glm/gtc/matrix_transform.hpp"


// PRE-PROCESSOR COMPILATION OPTIONS
// #define SGL_OUTPUT_OPENGL_DETAILS true ///< Enables (extra) OpenGLManager output. Useful for debugging.
// #define SGL_CHECK_FOR_ERRORS_ON_RENDER false

/**
 * @brief General purpose
 */
namespace SGL
{
    // Math constants
    const float PI = 3.14159265;
    const float PIx2 = 6.2831853;
    const float E = 2.7182818284;
    // Default texture name, used by both the AssetManager and the SGL_Renderer to ID internal resources
    const std::string DEFAULT_TEXTURE             = "default_texture";
    const std::string INVISIBLE_TEXTURE           = "invisible_texture"; // Use for the circle renderer
    const std::string SQUARE_TEXTURE              = "blank_square"; // White square, yeah
    const std::string DEFAULT_BMP_FONT_TEXTURE    = "default_bmp_font_texture"; // Used by the SGL_Renderer to render bitmap fonts
    const std::string DEFAULT_SPRITE_SHADER       = "default_sprite_shader";
    const std::string DEFAULT_SPRITE_BATCH_SHADER = "default_sprite_batch_shader";
    const std::string DEFAULT_PIXEL_SHADER        = "default_pixel_shader";
    const std::string DEFAULT_PIXEL_BATCH_SHADER  = "default_pixel_batch_shader";
    const std::string DEFAULT_TEXT_SHADER         = "default_text_shader";
    const std::string DEFAULT_LINE_SHADER         = "default_line_shader";
    const std::string DEFAULT_LINE_BATCH_SHADER   = "default_line_batch_shader";
    const std::string DEFAULT_FRAMEBUFFER_SHADER  = "default_framebuffer_shader";
    const std::string DEFAULT_CIRCLE_SHADER       = "default_circle_shader";
    const std::string DEFAULT_CIRCLE_BATCH_SHADER = "default_circle_batch_shader";
}

/**
 * @brief Suble OpenGL API constants, to avoid feeding wrong data to the GPU
 */
namespace SGL_OGL_CONSTANTS
{
    // GL_LINES uses this value top set the line's width, note that if AA is enabled it limits the linw width
    // support to 1.0f
    const float MAX_LINE_WIDTH = 20.0f;
    const float MIN_LINE_WIDTH = 1.0f;

    const float MAX_PIXEL_SIZE = 20.0f;
    const float MIN_PIXEL_SIZE = 1.0f;

    const float MAX_CIRCLE_WIDTH = 1.0f;
    const float MIN_CIRCLE_WIDTH = 0.01f;

    // These rendering consatants are the maximum amount of simultaneous instances to be rendered in a batch
    // and MUST NOT BE EXCEEDED
    const std::uint32_t MAX_SPRITE_BATCH_INSTANCES = 10000;
    const std::uint32_t MAX_PIXEL_BATCH_INSTANCES = 10000;
    const std::uint32_t MAX_LINE_BATCH_INSTANCES = 10000;

    // Names assigned to the OpenGL objects used by the SGL_Renderer
    const std::string SGL_RENDERER_PIXEL_VAO                  = "SGL_Renderer_pixel_VAO";
    const std::string SGL_RENDERER_PIXEL_VBO                  = "SGL_Renderer_pixel_VBO";
    const std::string SGL_RENDERER_PIXEL_BATCH_INSTANCES_VBO  = "SGL_Renderer_pixel_batch_instances_VBO";
    const std::string SGL_RENDERER_PIXEL_BATCH_VAO            = "SGL_Renderer_pixel_batch_VAO";
    const std::string SGL_RENDERER_PIXEL_BATCH_VBO            = "SGL_Renderer_pixel_batch_VBO";
    const std::string SGL_RENDERER_LINE_VAO                   = "SGL_Renderer_line_VAO";
    const std::string SGL_RENDERER_LINE_VBO                   = "SGL_Renderer_line_VBO";
    const std::string SGL_RENDERER_LINE_BATCH_INSTANCES_VBO   = "SGL_Renderer_line_batch_instances_VBO";
    const std::string SGL_RENDERER_LINE_BATCH_VAO             = "SGL_Renderer_line_batch_VAO";
    const std::string SGL_RENDERER_LINE_BATCH_VBO             = "SGL_Renderer_line_batch_VBO";
    const std::string SGL_RENDERER_SPRITE_VAO                 = "SGL_Renderer_sprite_VAO";
    const std::string SGL_RENDERER_SPRITE_VBO                 = "SGL_Renderer_sprite_VBO";
    const std::string SGL_RENDERER_SPRITE_BATCH_INSTANCES_VBO = "SGL_Renderer_sprite_batch_instances_VBO";
    const std::string SGL_RENDERER_SPRITE_BATCH_VAO           = "SGL_Renderer_sprite_batch_VAO";
    const std::string SGL_RENDERER_SPRITE_BATCH_VBO           = "SGL_Renderer_sprite_batch_VBO";
    const std::string SGL_RENDERER_TEXT_VAO                   = "SGL_Renderer_text_VAO";
    const std::string SGL_RENDERER_TEXT_VBO                   = "SGL_Renderer_text_VBO";
    const std::string SGL_RENDERER_TEXTURE_UV_VBO             = "SGL_Renderer_texture_uv_VBO";

    // POST PROCESSOR EXCLUSIVE
    const std::string SGL_POSTPROCESSOR_PRIMARY_FBO    = "SGL_PostProcessor_primary_FBO";
    const std::string SGL_POSTPROCESSOR_SECONDARY_FBO  = "SGL_PostProcessor_secondary_FBO";
    const std::string SGL_POSTPROCESSOR_TEXTURE_UV_VBO = "SGL_PostProcessor_UV_VBO";
    const std::string SGL_POSTPROCESSOR_VAO            = "SGL_PostProcessor_VAO";
    const std::string SGL_POSTPROCESSOR_VBO            = "SGL_PostProcessor_VBO";

    // Default shader uniform names, make sure they match your custom shaders.
    const std::string SHADER_UNIFORM_V4F_COLOR                  = "color";
    const std::string SHADER_UNIFORM_F_DELTA_TIME               = "deltaTime";
    const std::string SHADER_UNIFORM_F_TIME_ELAPSED             = "timeElapsed";
    const std::string SHADER_UNIFORM_V2F_WINDOW_DIMENSIONS      = "windowDimensions";
    const std::string SHADER_UNIFORM_M4F_MODEL                  = "model";
    const std::string SHADER_UNIFORM_M4F_PROJECTION             = "projection";
    const std::string SHADER_UNIFORM_F_CIRCLE_BORDER_WIDTH      = "circleBorder";

    // POST PROCESSOR EXCLUSIVE
    const std::string SHADER_UNIFORM_I_SCENE                    = "scene";
    const std::string SHADER_UNIFORM_V2F_FBO_TEXTURE_DIMENSIONS = "fboTextureDimensions";
    const std::string SHADER_UNIFORM_V2F_MOUSE_POSITION         = "mousePosition";

};



/**
 * @brief Project's folder structure
 */
namespace FOLDER_STRUCTURE
{
    const std::string ROOT_DIR   = "./";
    const std::string ASSETS_DIR = "/assets/skeletonGL/";

    const std::string SHADERS_DIR  = ROOT_DIR + ASSETS_DIR + "shaders/";
    const std::string TEXTURES_DIR = ROOT_DIR + ASSETS_DIR + "textures/";
    const std::string FONTS_DIR    = ROOT_DIR + ASSETS_DIR + "fonts/";

    const std::string DEFAULT_DEBUG_LOG_FILE         = ROOT_DIR + ".debugLog.txt";
    const std::string DEFAULT_TTF_FONT_FILE          = FONTS_DIR + "defaultFont.ttf";
    const std::string DEFAULT_TEXTURE_FILE           = TEXTURES_DIR + "defaultTexture.png";
    const std::string DEFAULT_SQUARE_TEXTURE_FILE    = TEXTURES_DIR + "blank_square.png";
    const std::string DEFAULT_BMP_FONT_TEXTURE_FILE  = TEXTURES_DIR + "default_bitmap_font.png";
    const std::string DEFAULT_INVISIBLE_TEXTURE_FILE = TEXTURES_DIR + "invisible_texture.png";

    // Default shader locations
    const std::string SPRITE_SHADER_V_FILE       = FOLDER_STRUCTURE::SHADERS_DIR + "spriteV.c";
    const std::string SPRITE_SHADER_F_FILE       = FOLDER_STRUCTURE::SHADERS_DIR + "spriteF.c";
    const std::string SPRITE_BATCH_SHADER_V_FILE = FOLDER_STRUCTURE::SHADERS_DIR + "spriteBatchV.c";
    const std::string SPRITE_BATCH_SHADER_F_FILE = FOLDER_STRUCTURE::SHADERS_DIR + "spriteBatchF.c";
    const std::string PIXEL_SHADER_V_FILE        = FOLDER_STRUCTURE::SHADERS_DIR + "pixelV.c";
    const std::string PIXEL_SHADER_F_FILE        = FOLDER_STRUCTURE::SHADERS_DIR + "pixelF.c";
    const std::string PIXEL_BATCH_SHADER_V_FILE  = FOLDER_STRUCTURE::SHADERS_DIR + "pixelBatchV.c";
    const std::string PIXEL_BATCH_SHADER_F_FILE  = FOLDER_STRUCTURE::SHADERS_DIR + "pixelBatchF.c";
    const std::string LINE_SHADER_V_FILE         = FOLDER_STRUCTURE::SHADERS_DIR + "lineV.c";
    const std::string LINE_SHADER_F_FILE         = FOLDER_STRUCTURE::SHADERS_DIR + "lineF.c";
    const std::string LINE_BATCH_SHADER_V_FILE   = FOLDER_STRUCTURE::SHADERS_DIR + "lineBatchV.c";
    const std::string LINE_BATCH_SHADER_F_FILE   = FOLDER_STRUCTURE::SHADERS_DIR + "lineBatchF.c";
    const std::string CIRCLE_SHADER_V_FILE       = FOLDER_STRUCTURE::SHADERS_DIR + "circleV.c";
    const std::string CIRCLE_SHADER_F_FILE       = FOLDER_STRUCTURE::SHADERS_DIR + "circleF.c";
    const std::string CIRCLE_BATCH_SHADER_V_FILE = FOLDER_STRUCTURE::SHADERS_DIR + "circleBatchV.c";
    const std::string CIRCLE_BATCH_SHADER_F_FILE = FOLDER_STRUCTURE::SHADERS_DIR + "circleBatchF.c";
    const std::string TEXT_SHADER_V_FILE         = FOLDER_STRUCTURE::SHADERS_DIR + "textV.c";
    const std::string TEXT_SHADER_F_FILE         = FOLDER_STRUCTURE::SHADERS_DIR + "textF.c";
    const std::string FRAMEBUFFER_SHADER_V_FILE  = FOLDER_STRUCTURE::SHADERS_DIR + "frameBufferV.c";
    const std::string FRAMEBUFFER_SHADER_F_FILE  = FOLDER_STRUCTURE::SHADERS_DIR + "frameBufferF.c";

}

/**
 * @brief Details for the renderer's custom UV values
 * @section DESCRIPTION
 *
 * Example format:
 * GLfloat UV[] = {
 *     0.0f, 1.0f, //top left
 *     1.0f, 0.0f, //bot right
 *     0.0f, 0.0f, //bot left

 *     0.0f, 1.0f, //top left
 *     1.0f, 1.0f, //top right
 *     1.0f, 0.0f, //bot right
 * };
 */
struct UV_Wrapper
{
    glm::vec2 UV_topLeft, UV_botRight, UV_botLeft, UV_topRight;
    UV_Wrapper() : UV_topRight(0,0), UV_botLeft(0,0), UV_botRight(0,0), UV_topLeft(0,0) {}
};

/**
 * @brief Houses all the initialization options for window creation
 * @section DESCRIPTION
 *
 * Dimension values will be used to calculate orthographic projection metrices
 * Note that .ini values will override the ones supplied by the programmer during window creation
 */
struct WindowCreationSpecs
{
    bool fullScreen, enableVSYNC;               ///< Enable fullscreen, VSYNC at startup
    bool activeVSYNC;                           ///< Actual VSYNC status
    bool showFPS;                               ///< Honored by the user, stored here for ez .ini file access
    bool cursorVisibility;                      ///< Toggles the system cursor on and off
    int displayID;                              ///< Helps when debugging on multiple displays
    std::string windowTitle;                    ///< The title of the window
    int windowPosX;                             ///< Initial window position for non fullscreen applications
    int windowPosY;                             ///< Initial window position for non fullscreen applications
    int minW, minH;                             ///< Smallest supported window size
    int maxW, maxH;                             ///< Maximum supported window size
    int currentX, currentY, currentW, currentH; ///< Window dimensions
    int internalW, internalH;                   ///< Internal orthographic projection dimensions
    int fullscreenW, fullscreenH;               ///< Default full screen resolution
    int gamepad1;                               ///< Available gamepad slots
    std::string iniFile;                        ///< Path to the config ini file (might want to rename the file like the executable)
    WindowCreationSpecs() : currentX(0), currentY(0), currentW(0), currentH(0), internalW(0), internalH(0),
                            windowPosX(SDL_WINDOWPOS_UNDEFINED), windowPosY(SDL_WINDOWPOS_UNDEFINED),
                            enableVSYNC(true), fullScreen(false), minW(640), minH(360), maxW(1920),
                            maxH(1080), fullscreenW(1920), fullscreenH(1080), windowTitle(""), showFPS(false),
                            cursorVisibility(true), activeVSYNC(false), displayID(-1), iniFile("config.ini") {}
};

/**
 * @brief Represents a single character to be processed by the TTF renderer
 */
struct Character
{
    GLuint textureID;
    glm::ivec2 size;
    glm::ivec2 bearing;
    GLuint advance;
};

/**
 * @brief Normalized color used by SkeletonGL
 */
struct SGL_Color
{
    GLfloat r, g, b, a; ///< min 0.0 to max 1.0
    SGL_Color() : r(1.0f), g(1.0f), b(1.0f), a(1.0f) {}
    SGL_Color(float r, float g, float b, float a) : r(r), g(g), b(b), a(a) {}
    // Helper function to change color, may be used later for further range validation
    void setColor(float r, float g, float b, float a) noexcept
        {
            this->r = r; this->b = b;
            this->g = g; this->a = a;
        }
};

/**
 * @brief Represents a single keystate
 */
struct SGL_InputKey
{
    GLboolean pressed, released;
    SGL_InputKey() : pressed(false), released(false) {}
};

/**
 * @brief Encapsulates an internal event (window events, UNIX system signals etc.)
 */
struct SGL_InternalEvent
{
    GLboolean active;

    SGL_InternalEvent() : active(false) {}
};

/**
 * @brief Represents a mouse
 */
struct SGL_InputMouse
{
    // Mouse scroll wheel is treated like a button, pressed = rolling in that direction
    SGL_InputKey rightBtn, leftBtn, middleBtn, scrollDown, scrollUp;
    std::int32_t cursorX, cursorY, cursorXNormalized, cursorYNormalized;

    SGL_InputMouse() : cursorX(0), cursorY(0), cursorXNormalized(0), cursorYNormalized(0) {}
};

/**
 * @brief Represents the entire input state (and some helpful events) of a single frame
 */
struct SGL_InputFrame
{
    // Keyboard
    SGL_InputKey up, down, left, right;                                                   ///< Arrow keys
    SGL_InputKey q,w,e,r,t,y,u,i,o,p,a,s,d,f,g,h,j,k,l,z,x,c,v,b,n,m;                     ///< Letter keys
    SGL_InputKey num1, num2, num3, num4, num5, num6, num7, num8, num9;                    ///< Upper numbers
    SGL_InputKey esc, space, enter, backspace, ctrl, shift, alt;                          ///< Modifiers

    // Mouse
    SGL_InputMouse mouse;

    // Events
    SGL_InternalEvent sdlInternalQuit;                                                         ///< Internal SDL exit event
    SGL_InternalEvent mouseFocus, keyboardFocus, windowFocus, windowMinimized, windowRestored; ///< Window state
};

/**
 * @brief Represents a single value and key from the .ini file
 */
struct SGL_IniField
{
    std::string title, key, value;
};

/**
 * @brief Contains an individual frame's time data
 */

struct SGL_FrameData
{
    double time;
    std::uint16_t ticks;
    std::uint16_t updateCycles;

    SGL_FrameData() : time(0.0f), ticks(0), updateCycles(0) {}
    void reset() noexcept
        {
            time = 0.0f;
            ticks = 0;
        }
};


/**
 * @brief Represents an OpenGL buffer
 */
struct GLBuffer
{
    std::string name;
    GLuint ID;
    GLBuffer() : name(""), ID(0) {}
};

/**
 * @brief Represents an OpenGL setting
 */
struct GLSetting
{
    std::string name;
    GLenum value;
    bool active;
    GLSetting() : name(""), value(0), active(false) {}
};

/**
 * @brief Collection of the currently active buffers and other settings
 */
struct CurrentBuffers
{
    GLBuffer currentVBO, currentEBO, currentVAO, currentFBO_R, currentFBO_W;
};


/**
 * @brief Collection of the currently active buffers and other settings
 */
struct CurrentSettings
{
    GLSetting depth, faceCulling, blending, depthMask;
};


/**
 * @brief Helps with priority sorting
 */
enum LOG_LEVEL
{
    NO_LOG_LEVEL,
    DEBUG,    // Non critical output, for non SGL debugging
    ERROR,    // Outputs errors only, for production builds
    SGL_DEBUG // This one is exclusvely used by the internal SGL code!
};


/**
 * @brief Terminal colors, requires a compatible terminal! (URXVT works fine)
 */
enum LOG_COLOR
{
    TERM_DEFAULT,
    TERM_RED,
    TERM_GREEN,
    TERM_BLUE,
    TERM_BLACK,
    TERM_YELLOW,
    TERM_MAGENTA,
    TERM_CYAN,
    TERM_WHITE
};


/**
 * @brief What blending mode should the render use?
 */
enum BLENDING_TYPE
{
    NONE,               // DISABLES BLENDING MODE
    CUSTOM,             // USE THE PROVIDED sfactor & dfactor
    DEFAULT_RENDERING,  // USED BY DEFAULT WHEN BLENDING IS ENABLED BUT NOT SPECIFIED
    SPRITE_RENDERING,   // DEFAULT OF SGL_SPRITE
    TEXT_RENDERING,     // DEFAULT TO THE SGL_RENDERER TEXT RENDERER
    PARTICLE_RENDERING, // FOR PARTICLES
    INVISIBLE_RENDERING,
    TEST_RENDERING_1,
    TEST_RENDERING_2,
    TEST_RENDERING_3
};

enum OPENGL_BLENDING_MODES
{

};

/**
 * @brief Switch between the two avilable camera modes
 */
enum CAMERA_MODE
{
    DEFAULT, // Apply camera offset to all shaders
    OVERLAY // Render everything relative to the screen (UI mode)
};


/**
 * @brief Used by the post-processor's framebuffer
 */
namespace GLCONSTANTS
{
    enum FBO
    {
        R,
        W,
        RW
    };
}


#endif
