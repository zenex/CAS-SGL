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
 * @file    src/skeletonGL/utility/SGL_Shader.cpp
 * @author  SENEX @ XENOBYTE.XYZ
 * @date    26/01/2021
 * @version 2.1
 *
 * @brief Wraps around all necessary shader functionality
 *
 * @section DESCRIPTION
 *
 * Encapsulates all the required shader functionality
 */

// C++
#include "iostream"

#include "SGL_Shader.hpp"


/**
 * @brief Constructor
 *
 * @return nothing
 */
SGL_Shader::SGL_Shader()
{

}

/**
 * @brief Destructor
 *
 * @return nothing
 */
SGL_Shader::~SGL_Shader()
{
    this->shaderType = SHADER_TYPE::NO_SHADER;
}

/**
 * @brief Enable the shader
 * @param oglm The windows OpenGL context
 *
 * @return nothing
 */
SGL_Shader &SGL_Shader::use(SGL_OpenGLManager &oglm) noexcept
{
    oglm.useProgram(this->ID);
    return *this;
}


/**
 * @brief Disable the shader
 * @param oglm The windows OpenGL context
 *
 * @return nothing
 */
void SGL_Shader::unbind(SGL_OpenGLManager &oglm) const noexcept
{
    oglm.useProgram(0);
}


/**
 * @brief Compiles and links a shader
 * @param oglm The windows OpenGL context
 * @param shadetType Shader category
 * @param vertexSource C string to the vertex shader file
 * @param fragmentSource C string to the fragment shader file
 * @param geometrySource C string to the geometry shader file (optional)
 *
 * @return nothing
 */
void SGL_Shader::compileShaders(SGL_OpenGLManager &oglm, SHADER_TYPE shaderType, const GLchar *vertexSource, const GLchar *fragmentSource, const GLchar *geometrySource) noexcept

{
    GLuint sVertex, sFragment, sGeometry;

    sVertex = oglm.createShader(GL_VERTEX_SHADER);
    oglm.shaderSource(sVertex, 1, &vertexSource, NULL);
    oglm.compileShader(sVertex);
    checkForErrors(oglm, sVertex, "VERTEX");

    sFragment = oglm.createShader(GL_FRAGMENT_SHADER);
    oglm.shaderSource(sFragment, 1, &fragmentSource, NULL);
    oglm.compileShader(sFragment);
    checkForErrors(oglm, sFragment, "FRAGMENT");

    if(geometrySource != nullptr)
    {
        sGeometry = oglm.createShader(GL_GEOMETRY_SHADER);
        oglm.shaderSource(sGeometry, 1, &geometrySource, NULL);
        oglm.compileShader(sGeometry);
        checkForErrors(oglm, sGeometry, "GEOMETRY");
    }

    this->ID = glCreateProgram();
    oglm.attachShader(this->ID, sVertex);
    oglm.attachShader(this->ID, sFragment);
    if(geometrySource != nullptr)
    {
        oglm.attachShader(this->ID, sGeometry);
    }
    oglm.linkProgram(this->ID);
    this->shaderType = shaderType;

    checkForErrors(oglm, this->ID, "PROGRAM");

    oglm.deleteShader(sVertex);
    oglm.deleteShader(sFragment);
    if(geometrySource != nullptr)
        oglm.deleteShader(sGeometry);
}


/**
 * @brief Sets a single float value
 * @param oglm The windows OpenGL context
 * @param name Variable to parse
 * @param value The integer value to parse
 * @param useShader Enable the shader?
 *
 * @return nothing
 */
void SGL_Shader::setFloat(SGL_OpenGLManager &oglm, const GLchar *name, GLfloat value, GLboolean useShader) noexcept
{
    if (useShader)
        this->use(oglm);
    oglm.uniform1f(glGetUniformLocation(this->ID, name), value);
}

/**
 * @brief Sets a single integer value
 * @param oglm The windows OpenGL context
 * @param name Variable to parse
 * @param value The integer value to parse
 * @param useShader Enable the shader?
 *
 * @return nothing
 */
void SGL_Shader::setInteger(SGL_OpenGLManager &oglm, const GLchar *name, GLint value, GLboolean useShader) noexcept
{
    if (useShader)
        this->use(oglm);
    oglm.uniform1i(glGetUniformLocation(this->ID, name), value);
}

/**
 * @brief Sets a 2D vector
 * @param oglm The windows OpenGL context
 * @param name Variable to parse
 * @param x X value
 * @param y Y value
 * @param useShader Enable the shader?
 *
 * @return nothing
 */
void SGL_Shader::setVector2f(SGL_OpenGLManager &oglm, const GLchar *name, GLfloat x, GLfloat y, GLboolean useShader) noexcept
{
    if (useShader)
        this->use(oglm);
    oglm.uniform2f(glGetUniformLocation(this->ID, name), x, y);
}


/**
 * @brief Sets a 2D vector
 * @param oglm The windows OpenGL context
 * @param name Variable to parse
 * @param value glm::vec2 to use
 * @param useShader Enable the shader?
 *
 * @return nothing
 */
void SGL_Shader::setVector2f(SGL_OpenGLManager &oglm, const GLchar *name, glm::vec2 value, GLboolean useShader) noexcept
{
    if (useShader)
        this->use(oglm);
    oglm.uniform2f(glGetUniformLocation(this->ID, name), value.x, value.y);
}


/**
 * @brief Sets a 3D vector
 * @param oglm The windows OpenGL context
 * @param name Variable to parse
 * @param x X value
 * @param y Y value
 * @param z Z value
 * @param useShader Enable the shader?
 *
 * @return nothing
 */
void SGL_Shader::setVector3f(SGL_OpenGLManager &oglm, const GLchar *name, GLfloat x, GLfloat y, GLfloat z, GLboolean useShader) noexcept
{
    if (useShader)
        this->use(oglm);
    oglm.uniform3f(glGetUniformLocation(this->ID, name), x, y, z);
}


/**
 * @brief Sets a 3D vector
 * @param oglm The windows OpenGL context
 * @param name Variable to parse
 * @param value glm::vec3 to use
 * @param useShader Enable the shader?
 *
 * @return nothing
 */
void SGL_Shader::setVector3f(SGL_OpenGLManager &oglm, const GLchar *name, glm::vec3 value, GLboolean useShader) noexcept
{
    if (useShader)
        this->use(oglm);
    oglm.uniform3f(glGetUniformLocation(this->ID, name), value.x, value.y, value.z);
}


/**
 * @brief Sets a 4D vector
 * @param oglm The windows OpenGL context
 * @param name Variable to parse
 * @param x X value
 * @param y Y value
 * @param z Z value
 * @param w W value
 * @param useShader Enable the shader?
 *
 * @return nothing
 */
void SGL_Shader::setVector4f(SGL_OpenGLManager &oglm, const GLchar *name, GLfloat x, GLfloat y, GLfloat z, GLfloat w, GLboolean useShader) noexcept
{
    if (useShader)
        this->use(oglm);
    oglm.uniform4f(glGetUniformLocation(this->ID, name), x, y, z, w);
}


/**
 * @brief Sets a 4D vector
 * @param oglm The windows OpenGL context
 * @param name Variable to parse
 * @param value glm::vec4 to use
 * @param useShader Enable the shader?
 *
 * @return nothing
 */
void SGL_Shader::setVector4f(SGL_OpenGLManager &oglm, const GLchar *name, glm::vec4 value, GLboolean useShader) noexcept
{
    if (useShader)
        this->use(oglm);
    oglm.uniform4f(glGetUniformLocation(this->ID, name), value.x, value.y, value.z, value.w);

}


/**
 * @brief Sets a 4D vector
 * @param oglm The windows OpenGL context
 * @param name Variable to parse
 * @param matrix glm::mat4 to use
 * @param useShader Enable the shader?
 *
 * @return nothing
 */
void SGL_Shader::setMatrix4(SGL_OpenGLManager &oglm, const GLchar *name, glm::mat4 matrix, GLboolean useShader) noexcept
{
    if(useShader)
        this->use(oglm);
    oglm.uniformMatrix4fv(glGetUniformLocation(this->ID, name), 1, GL_FALSE, glm::value_ptr(matrix));
}


/**
 * @brief Sets a 4D vector
 * @param oglm The windows OpenGL context
 * @param object ID to check
 * @param type The shader type
 *
 * @return nothing
 */
void SGL_Shader::checkForErrors(SGL_OpenGLManager &oglm, GLuint object, std::string type)
{
    GLint success;
    GLchar infoLog[1024];

    if(type != "PROGRAM")
    {
        oglm.getShaderiv(object, GL_COMPILE_STATUS, &success);
        if(!success)
        {
            oglm.getShaderInfoLog(object, 1024, NULL, infoLog);
            std::string errMsg = "Error compiling shaders! SGL_Shader::checkForErrors \n ERROR::SHADER: Compile time error: Type: " + type + "\n Error Message: " + infoLog;
            throw SGL_Exception(errMsg.c_str());
        }
    }
    else
    {
        oglm.getProgramiv(object, GL_LINK_STATUS, &success);
        if(!success)
        {
            oglm.getProgramInfoLog(object, 1024, NULL, infoLog);
            std::string errMsg = "Error linking shaders SGL_Shader::checkForErrors \n ERROR::PROGRAM: Link time error: Type: " + type + "\n Error Message: " + infoLog;
            throw SGL_Exception(errMsg.c_str());
        }
    }
}
