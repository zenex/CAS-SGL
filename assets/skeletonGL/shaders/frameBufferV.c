#version 330 core

layout (location = 0) in vec2 vertex; // <vec2 position, vec2 texCoords>
layout (location = 1) in vec2 UVCoords;

out vec2 TexCoords;

uniform float time;


void main()
{
  //gl_Position = vec4(vertex.xy, 0.0f, 1.0f);
  gl_Position = vec4(vertex.x, vertex.y, 0.0f, 1.0f);
  TexCoords = vec2(UVCoords.x, UVCoords.y);
}
