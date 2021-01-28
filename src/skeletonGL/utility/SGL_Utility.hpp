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
 * @file    src/skeletonGL/utility/SGL_Utility.hpp
 * @author  SENEX @ XENOBYTE.XYZ
 * @date    26/01/2021
 * @version 2.1
 *
 * @brief General purpose utility
 *
 * @section DESCRIPTION
 *
 * Utility that doesn't fit elsewhere.
 *
 */

#ifndef SRC_SKELETONGL_ASSETS_SGL_UTILITY_HPP
#define SRC_SKELETONGL_ASSETS_SGL_UTILITY_HPP

// C++
#include <map>
#include <string>
#include <algorithm>
#include <vector>
#include <time.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <chrono>
// SkeletonGL
#include "SGL_DataStructures.hpp"

extern std::vector<std::string> SGL_LOG_HISTORY; ///< The entire log history
extern LOG_LEVEL SGL_STDOUT_FILTER;


void SGL_Log(const std::string &msg, LOG_LEVEL logLevel = LOG_LEVEL::NO_LOG_LEVEL, LOG_COLOR logColor = LOG_COLOR::TERM_DEFAULT);


/**
 * @brief Encapsulates all SkeletonGL related exceptions, these exceptions will eventually be used to formalize the documentation
 */
class SGL_Exception : public std::exception
{
private:
    std::string pErrMsg; ///< Pretty self explanatory
public:
    SGL_Exception(const char *errMsg)
        {
            pErrMsg = "(╥﹏╥) EXCEPTION: ";
            pErrMsg.append(errMsg);
        }
    const char* what() const noexcept
        {
            return pErrMsg.c_str();
        }
};

/**
 * @brief Probes the user's hardware and API versions
 */

struct SGL_DeviceInfo
{
    std::string hostOS;                                                      ///< The OS the machine running the program is using
    const unsigned char *glewVer;                                            ///< GLEW library version
    const unsigned char *glVer, *glVendor, *glRenderer, *glslVer, *glMaxVer; ///< OpenGL versions
    unsigned int majorSDLVer, minorSDLVer, patchSDLVer;                      ///< Compiled SDL2 versions
    unsigned int linkedMajorSDLVer, linkedMinorSDLVer, linkedPatchSDLVer;    ///< Linked SDL2 version

    inline void printDeviceInfo() const noexcept
        {
            std::stringstream ss;
            SGL_Log("<--- Hardware Specs --->", LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_GREEN);
            SGL_Log("OS: " + hostOS, LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_DEFAULT);
            SGL_Log("--- SDL Info --- ", LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_DEFAULT);
            SGL_Log("Compiled SDL Version", LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_DEFAULT);
            SGL_Log("Major: " + std::to_string(majorSDLVer), LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_DEFAULT);
            SGL_Log("Minor: " + std::to_string(minorSDLVer), LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_DEFAULT);
            SGL_Log("Patch: " + std::to_string(patchSDLVer), LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_DEFAULT);
            SGL_Log("--- Linked SDL Version ---", LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_DEFAULT);
            SGL_Log("Major: " + std::to_string(linkedMajorSDLVer), LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_DEFAULT);
            SGL_Log("Minor: " + std::to_string(linkedMinorSDLVer), LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_DEFAULT);
            SGL_Log("Patch: " + std::to_string(linkedPatchSDLVer), LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_DEFAULT);
            SGL_Log("--- OpenGL Info --- ", LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_DEFAULT);
            ss << glVer;
            SGL_Log("Version: " + ss.str(), LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_DEFAULT); ss.str("");
            ss << glVendor;
            SGL_Log("Vendor: " + ss.str(), LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_DEFAULT); ss.str("");
            ss << glRenderer;
            SGL_Log("Renderer: " + ss.str(), LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_DEFAULT); ss.str("");
            ss << glslVer;
            SGL_Log("Shading: " + ss.str(), LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_DEFAULT); ss.str("");
            ss << glMaxVer;
            SGL_Log("OpenGL version supported by this platform: " + ss.str(), LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_DEFAULT); ss.str("");
            ss << glewVer;
            SGL_Log("Using GLEW version: " + ss.str(), LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_DEFAULT);
        }
};
/**
 * @brief Ini file parser and loader
 *
 * @section DESCRIPTION
 *
 * The SGL_Window Class is will load a list of default values to be processed (mostly window creation data)
 * that if found, will override the values originally used to contruct the window instance. The key/value
 * pairs must be properly referenced by their respective section's title
 */
class SGL_IniParser
{
private:
    std::string pFilePath;                      ///< Path to the .ini file to load
    std::map<std::string, std::string> pValues; ///< The file's keys and values
    std::vector<SGL_IniField> pIniFields;       ///< Sections of the file
    bool pFileLoaded;                           ///< Has the file been previously loaded?

    // Theses variables work as delimeters to find the value in each individual line of the .ini file
    const char pToken = ':';                    ///< Key pToken Value
    const char pWhiteSpace = ' ';               ///< Whitespace (used by the parser)
    const char pTitleMarkerStart = '[';         ///< Starting token for a section title
    const char pTitleMarkerEnd = ']';           ///< Ending token for a section title
    const std::string pNotFound = "";           ///< Default return value for not found request

    // Only one pToken allowed between each key and value
    bool pCheckForMultipleTokens(const char token, const std::string &str) const noexcept;
public:
    SGL_IniParser(const std::string &filePath);
    // Get value as a C++ string
    std::string getRawValue(const std::string section, const std::string key) const noexcept;
    // Get value as a 32bit int
    int getIntValue(const std::string section, const std::string key)  const noexcept;
};




#endif // SRC_SKELETONGL_ASSETS_SGL_UTILITY_HPP
