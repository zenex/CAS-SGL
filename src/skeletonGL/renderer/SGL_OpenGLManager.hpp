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
 * @file    src/skeletonGL/utility/SGL_OpenGLManager.hpp
 * @author  SENEX @ XENOBYTE.XYZ
 * @date    26/01/2021
 * @version 2.1
 *
 * @brief Encapsulates ALL OpenGL related function calls and manages its resources
 *
 * @section DESCRIPTION
 *
 * This OpenGL Manager is designed to keep tabs on the current state of OpenGL's state machine
 * and allow for a much easier debugging experience. Any and all OpenGL functions that may be
 * required in the future should be added here.
 */

#ifndef SRC_SKELETONGL_RENDERER_OPENGLMANAGER_HPP
#define SRC_SKELETONGL_RENDERER_OPENGLMANAGER_HPP

// C++
#include <iostream>
#include <vector>
#include <map>
#include <sstream>
// Dependencies
#include <GL/glew.h>
// SkeletonGL
#include "../utility/SGL_DataStructures.hpp"
#include "../utility/SGL_Utility.hpp"

/**
 * @brief Encapsulates an OpenGL context.
 * @section DESCRIPTION
 *
 * This class encapsulates all OpenGL related functions and keeps a record
 * of the current state of the internal OpenGL state machine to eased debugging.
 */
class SGL_OpenGLManager
{
private:
    CurrentSettings currentGLSettings;        ///< OpenGL rendering settings
    CurrentBuffers currentGLBuffers;          ///< Currently active buffers

    std::map<std::string, GLBuffer>VBO;       ///< All the context VBOs
    std::map<std::string, GLBuffer>EBO;       ///< All the context EBOs
    std::map<std::string, GLBuffer>VAO;       ///< All the context VAOs
    std::map<std::string, GLBuffer>FBO;       ///< All the context FBOs
    std::map<std::string, GLSetting>Settings; ///< Supported OpenGL render settings
    GLuint currentShaderID, currentTextureID; ///< Bound shader and texture IDs
    SGL_Color currentClearColor;              ///< Clear screen color

    // Currently, an opengl error arises when checking for
    // errors after initializing the GL context, doesn't
    // seem to affect rendering at all
    uint8_t initialErrorCap;

public:
    // Constructor
    SGL_OpenGLManager();

    // Destructor
    ~SGL_OpenGLManager();

    // Most of these functions are wrappers for C-style OpenGL calls that rely on error codes, not exceptions
    // or object modifications

    // Create a VBO
    void createVBO(const std::string &name) noexcept;
    // Create an EBO
    void createEBO(const std::string &name) noexcept;
    // Create a VAO
    void createVAO(const std::string &name) noexcept;
    // Create a FBO
    void createFBO(const std::string &name) noexcept;
    // Create a RBO
    void createRBO(const std::string &name) noexcept;
    // Bind the selected buffer
    void bindVBO(const std::string &name) noexcept;
    // Bind the selected buffer
    void bindEBO(const std::string &name) noexcept;
    // Bind the selected VAO
    void bindVAO(const std::string &name) noexcept;
    // Bind the selected FBO
    void bindFBO(const std::string &name, GLCONSTANTS::FBO type = GLCONSTANTS::RW) noexcept;
    // Unbind the current buffer
    void unbindVBO() noexcept;
    // Unbind the current EBO
    void unbindEBO() noexcept;
    // Unbind the current VAO
    void unbindVAO() noexcept;
    // Unbind the current FBO
    void unbindFBO() noexcept;
    // Cleans all buffer storage
    void clearBuffers() noexcept;
    // Deletes all the VBO
    void deleteVBO(const std::string &name) noexcept;
    // Deletes all the EBO
    void deleteEBO(const std::string &name) noexcept;
    // Deletes all the VAO
    void deleteVAO(const std::string &name) noexcept;
    // Deletes all the FBO
    void deleteFBO(const std::string &name) noexcept;

    // Activates the specified shader program
    void useProgram(GLuint program) noexcept;
    // Creates a shader
    GLuint createShader(GLenum shaderType) const noexcept;
    // Compile a shader program
    void compileShader(GLuint shader) const noexcept;
    // Specify the shader data
    void shaderSource(GLuint shader, GLsizei count, const GLchar **string, const GLint *length) const noexcept;
    // Link shaders into a program
    void attachShader(GLuint program, GLuint shader) const noexcept;
    // Delete a shader
    void deleteShader(GLuint shader) const noexcept;
    // Create the shader program
    GLuint createProgram() const noexcept;
    // Link a compiled shader program
    void linkProgram(GLuint program) const noexcept;
    // Get shader data
    void getShaderiv(GLuint shader, GLenum pname, GLint *params) const noexcept;
    // Gets the shader info log
    void getShaderInfoLog(GLuint shader,  GLsizei maxLength, GLsizei *length, GLchar *infoLog) const noexcept;
    // Gets them program data
    void getProgramiv(GLuint shader, GLenum pname, GLint *params) const noexcept;
    // Gets the program log
    void getProgramInfoLog(GLuint shader,  GLsizei maxLength, GLsizei *length, GLchar *infoLog) const noexcept;

    // Sets a uniform location from the specified shader
    GLint getUniformLocation(GLuint program, const GLchar *name) const noexcept;
    // Sets a float uniform location from the specified shader
    void uniform1f(GLint location, GLfloat v0) const noexcept;
    // Sets an integer location from the specified shader
    void uniform1i(GLint location, GLint v0) const noexcept;
    // Sets a two floats uniforms from the specified shader
    void uniform2f(GLint location, GLfloat v0, GLfloat v1) const noexcept;
    // Sets a three float uniforms from the specified shader
    void uniform3f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2) const noexcept;
    // Sets a four float uniforms from the specified shader
    void uniform4f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3) const noexcept;
    // Sets a 4x4 float uniform matrix from the specified shader
    void uniformMatrix4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value) const noexcept;
    // Sets a 2fv from the specified shader
    void uniform2fv(GLint location, GLsizei count, const GLfloat *value) const noexcept;
    // Sets a 1iv from the specified shader
    void uniform1iv(GLint location, GLsizei count, const GLint *value) const noexcept;
    // Sets a 1fv from the specified shader
    void uniform1fv(GLint location, GLsizei count, const GLfloat *value) const noexcept;

    // Sets the clear screen color
    void setClearColor(SGL_Color &newColor) const noexcept;
    // Clears the color buffer
    void clearColorBuffer() const noexcept;
    // Clears the depth buffer (UNUSED AS OF v 1.0)
    void clearDepthBuffer() const noexcept;
    // Clears both color and depth buffers
    void clearColorAndDepthBuffers() const noexcept;
    // Check for depth testing?
    void depthTesting(bool value) noexcept;
    // Enable depth mask?
    void depthMask(bool value) noexcept;
    // Enable face culling?
    void faceCulling(bool value) noexcept;
    // Set blending type
    void blending(bool value = true, BLENDING_TYPE type = BLENDING_TYPE::DEFAULT_RENDERING, GLenum sfactor = 0, GLenum dfactor = 0) noexcept;

    // Generate a texture
    void genTextures(GLsizei n, GLuint *textures) const noexcept;
    // Bind texture
    void bindTexture(GLenum target, GLuint texture) const noexcept;
    // Activates a texture
    void activeTexture(GLenum texture) noexcept;
    // Deletes textures
    void deleteTextures(GLsizei n, const GLuint *textures) const noexcept;
    // Texture pixel format
    void pixelStorei(GLenum pname, GLint param) const noexcept;
    // Texture creation parameters
    void texParameteri(GLenum target, GLenum pname, GLint param) const noexcept;
    // Texture creation parameters
    void texParameterf(GLenum target, GLenum pname, GLfloat param) const noexcept;
    // Creates a 2D texture
    void texImage2D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *data) const noexcept;

    // Draws loaded state machine
    void drawArrays(GLenum mode, GLint first, GLsizei count) const noexcept;
    // Draw multiple instances of a range of elements
    void drawArraysInstanced(GLenum mode, GLint first, GLsizei count, GLsizei instancecount) const noexcept;
    // Draw multiple instances of a set of elements
    void drawElementsInstanced(GLenum mode, GLsizei count, GLenum type, const void *indices, GLsizei instancecount) const noexcept;
    // Add data to buffer
    void bufferData(GLenum target, GLsizeiptr size, const GLvoid* data, GLenum usage) const noexcept;
    // Replace data to buffer
    void bufferSubData(GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid* data) const noexcept;
    // Enable or disable a generic vertex attribute array
    void enableVertexAttribArray(GLuint index) const noexcept;
    // Define an array of generic vertex attribute data
    void vertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid *pointer) const noexcept;
    // modify the rate at which generic vertex attributes advance during instanced rendering
    void vertexAttribDivisor(GLuint index, GLint divisor) const noexcept;
    // Deletes a compiled and linked shader program
    void deleteProgram(GLuint program) const noexcept;
    // Generate framebuffer
    void frameBufferTexture2D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level) const noexcept;
    // Render framebuffer
    void blitFrameBuffer(GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter) const noexcept;

    // Set the width (float) for GL_LINES
    void lineWidth(GLfloat width) const noexcept;
    void pixelSize(GLfloat size) const noexcept;

    // Checks the current FBO status
    GLenum checkFrameBufferStatus(GLenum target) const noexcept;
    // Resize the viewport
    void resizeGL(int x, int y, int w, int h) const noexcept;
    // Error reporting
    void checkForGLErrors() noexcept;
    // Total amount of active VBO for this OpenGL context
    int totalVBO() const noexcept;
    // Total amount of active VAOs for this OpenGL context
    int totalVAO() const noexcept;
    // Total amount of active EBOs for this OpenGL context
    int totalEBO() const noexcept;
    // Total amount of active FBO for this OpenGL context
    int totalFBO() const noexcept;

    // En / Dis able OpenGL capabilities
    void enable(GLenum cap) const noexcept;
    void disable(GLenum cap) const noexcept;
    // void toggleLineAA(bool toggle) const noexcept;
};

#endif // SRC_SKELETONGL_RENDERER_OPENGLMANAGER_HPP
