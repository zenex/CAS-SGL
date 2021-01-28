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
 * @file    src/skeletonGL/utility/SGL_Utility.cpp
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

#include "SGL_Utility.hpp"

/**
 * @brief Adds a string to the log history, formatting optional
 *
 * @param &msg C++ string to append
 * @param logLevel Log priority
 * @param logColor Color the output should be outputted as (supported terminals only)
 * @return nothing
 */
std::vector<std::string> SGL_LOG_HISTORY;
LOG_LEVEL SGL_STDOUT_FILTER = LOG_LEVEL::DEBUG;

void SGL_Log(const std::string &msg, LOG_LEVEL logLevel, LOG_COLOR logColor)
{
    std::string msgLogLevel, msgColorPrefix, msgColorPostfix;
    msgColorPostfix = "\033[0m";
    switch (logLevel)
    {
    case LOG_LEVEL::NO_LOG_LEVEL:
        break;
    case LOG_LEVEL::DEBUG:
        msgLogLevel = "\033[1;32mDEBUG_MSG-";
        break;
    case LOG_LEVEL::ERROR:
        msgLogLevel = "\033[1;31mERROR_MSG-";
        break;
    case LOG_LEVEL::SGL_DEBUG:
        msgLogLevel = "\033[1;36mSGL-";
        break;
    default:
        break;
    }

    switch (logColor)
    {
    case LOG_COLOR::TERM_RED:
        msgColorPrefix = "\033[1;31m";
        break;
    case LOG_COLOR::TERM_BLUE:
        msgColorPrefix = "\033[1;34m";
        break;
    case LOG_COLOR::TERM_GREEN:
        msgColorPrefix = "\033[1;32m";
        break;
    case LOG_COLOR::TERM_YELLOW:
        msgColorPrefix = "\033[1;33m";
        break;
    case LOG_COLOR::TERM_BLACK:
        msgColorPrefix = "\033[1;30m";
        break;
    case LOG_COLOR::TERM_MAGENTA:
        msgColorPrefix = "\033[1;35m";
        break;
    case LOG_COLOR::TERM_CYAN:
        msgColorPrefix = "\033[1;36m";
        break;
    case LOG_COLOR::TERM_WHITE:
        msgColorPrefix = "\033[1;37m";
        break;
    case LOG_COLOR::TERM_DEFAULT:
        msgColorPrefix = "";
        break;
    default:
        msgColorPrefix = "";
        break;
    }

    std::time_t logTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::string now = std::ctime(&logTime);
    // Remove newline from ctime generated str
    if (!now.empty() && now[now.length() - 1] == '\n')
        now.erase(now.length() - 1);
    //Trim unnecessary info
    now = now.erase(0, 11); // trim date data
    now = now.erase(now.length() - 5, 5); // trim year data
    now = "[" + now + "]: ";

    SGL_LOG_HISTORY.push_back(now + msg);

    switch (SGL_STDOUT_FILTER)
    {
    case LOG_LEVEL::NO_LOG_LEVEL: // OUTPUT EVERYTHING
    {
        std::cout << msgLogLevel << now << msgColorPostfix << msgColorPrefix << msg << msgColorPostfix << std::endl;
        break;
    }
    case LOG_LEVEL::ERROR: // OUTPUT ONLY ERROR MESSAGES (CHOOSE THIS FOR PRODUCTION)
    {
        if (logLevel == LOG_LEVEL::ERROR)
            std::cout << msgLogLevel << now << msgColorPostfix << msgColorPrefix << msg << msgColorPostfix << std::endl;
        break;
    }

    case LOG_LEVEL::DEBUG: // OUTPUT NON SGL DEBUG AND ERROR MESSAGES
    {
        if (logLevel == LOG_LEVEL::DEBUG || logLevel == LOG_LEVEL::ERROR)
            std::cout << msgLogLevel << now << msgColorPostfix << msgColorPrefix << msg << msgColorPostfix << std::endl;
        break;
    }

    case LOG_LEVEL::SGL_DEBUG: // OUTPUT EVERYTHING
    {
        std::cout << msgLogLevel << now << msgColorPostfix << msgColorPrefix << msg << msgColorPostfix << std::endl;
        break;
    }
    }
}




/**
 * @brief Adds a string to the log history, formatting optional
 *
 * @param &msg C++ string to append
 * @param logLevel Log priority
 * @param logColor Color the output should be outputted as (supported terminals only)
 * @return nothing
 */
SGL_IniParser::SGL_IniParser(const std::string &filePath)
{
    std::ifstream iniFile(filePath.c_str());
    if (!iniFile.is_open()) // No need to crash the process since default values are provided to SGL_Window
    {
        SGL_Log("No config.ini file found.", LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_DEFAULT);
        pFileLoaded = false;
        return;
    }
    std::string line;
    int lineNum = 1;
    std::string currentTitle = "[DEFAULT]";
    try
    {
        while (std::getline(iniFile, line))
        {
            std::string prefix, postfix;
            if (line == "\n" || line == "") // skip over newlines and empty lines
                continue;

            // trim all whitespaces
            line.erase(std::remove(line.begin(), line.end(), pWhiteSpace), line.end());
            if (line[0] == '#') // its a comment
                continue;
            if (line[0] == pTitleMarkerStart && line[line.length() - 1] == pTitleMarkerEnd) // isolate potential section title
            {
                SGL_Log("Section in INI file detected: " + line, LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_DEFAULT);
                currentTitle = line;
            }
            else if (!pCheckForMultipleTokens(pToken, line)) // check for incorrectly formatted line
            {
                throw SGL_Exception("Invalid format at line " + static_cast<char>(lineNum));
            }
            else // check for key and value
            {
                auto npos = line.find(pToken);
                if (npos != std::string::npos)
                {
                    postfix = line.substr(npos + 1);
                    prefix = line.substr(0, npos);
                    SGL_Log("key: " + prefix + " value: " + postfix, LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_DEFAULT);
                    pValues[prefix] = postfix;

                    SGL_IniField field;
                    field.title = currentTitle;
                    field.key = prefix;
                    field.value = postfix;

                    pIniFields.push_back(field);
                }
            }
            lineNum++;
        }
        pFileLoaded = true;
    }
    catch (SGL_Exception &e)
    {
        SGL_Log(e.what());
    }
}


/**
 * @brief Retrieves a key value from the loaded .ini file as a string or as an empty string if not found
 *
 * @param section Section in the .ini file where the key is
 * @param ley The key that points to the desired value
 * @return string
 */
std::string SGL_IniParser::getRawValue(const std::string section, const std::string key)  const noexcept
{
    if (!pFileLoaded)
        return pNotFound;

    for (std::vector<SGL_IniField>::const_iterator it = pIniFields.begin(); it != pIniFields.end(); ++it)
    {
        if (((*it).title) == section && ((*it).key) == key)
            return (*it).value;
    }
    return pNotFound;
}


/**
 * @brief Retrieves a key value from the loaded .ini file as an integer or 0 if not found
 *
 * @param section Section in the .ini file where the key is
 * @param ley The key that points to the desired value
 * @return int
 */
int SGL_IniParser::getIntValue(const std::string section, const std::string key)  const noexcept
{
    std::size_t returnValue = 0;
    if (!pFileLoaded)
        return static_cast<int>(returnValue);

    std::string foundValue = "";
    for (std::vector<SGL_IniField>::const_iterator it = pIniFields.begin(); it != pIniFields.end(); ++it)
    {
        if (((*it).title) == section && ((*it).key) == key)
            foundValue =  (*it).value;
    }
    if (foundValue != "")
        returnValue = std::stoi(foundValue, &returnValue);

    return static_cast<int>(returnValue);
}


/**
 * @brief Checks for multiple delimiting tokens on a single key/value pair
 *
 * @param token Token to look for
 * @param str String to test
 * @return bool
 */
bool SGL_IniParser::pCheckForMultipleTokens(const char token, const std::string &str)  const noexcept
{
    int count = 0;
    for (int i = 0; i < str.size(); ++i)
    {
        if (str.at(i) == token)
            count++;
    }
    return ((count == 1) ? true : false);
}
