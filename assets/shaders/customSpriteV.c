#version 330 core

layout (location = 0) in vec2 vertex; //vec2 pos
layout (location = 1) in vec2 UVCoords;


out vec2 TexCoords;

uniform mat4 model;
uniform mat4 projection;


void main()
{
  //NOTE, flip the y axis on the UV coords since they default to inverted anyway
  //UPDATE, this messed up custom UV values, DON'T
    TexCoords = vec2(UVCoords.x, UVCoords.y);
  //flips image veritcally, opengl flips teh y axis, fixed in ortho matrix
    //TexCoords = vec2(vertex.z, 1.0f - vertex.w);
  gl_Position = projection * model * vec4(vertex.xy, 0.0, 1.0);
}

