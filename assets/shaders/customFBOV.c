#version 330 core

layout (location = 0) in vec2 vertex; // <vec2 position, vec2 texCoords>
layout (location = 1) in vec2 UVCoords;

out vec2 TexCoords;

uniform bool  chaos;
uniform bool  confuse;
uniform bool  shake;
uniform float time;


void main()
{
  //gl_Position = vec4(vertex.xy, 0.0f, 1.0f);
  gl_Position = vec4(vertex.x, vertex.y, 0.0f, 1.0f);
  vec2 texture = vec2(UVCoords.x, UVCoords.y);
  if(chaos)
    {
      float strength = 0.3;
      vec2 pos = vec2(texture.x + sin(time) * strength, texture.y + cos(time) * strength);
      TexCoords = pos;
    }
  else if(confuse)
    {
      TexCoords = vec2(1.0 - texture.x, 1.0 - texture.y);
    }
  else
    {
      TexCoords = texture;
    }
  if (shake)
    {
      float strength = 0.02;
      gl_Position.x += cos(time * 10) * strength;
      gl_Position.y += cos(time * 15) * strength;
    }
}
