#version 330 core

layout (location = 0) in vec2 vertex; //vec2 pos, vec texcoord
layout (location = 3) in vec2 instanceVertex;


uniform mat4 projection;

void main()
{
    gl_Position = projection * vec4(vertex + instanceVertex, 0.0, 1.0);
}

