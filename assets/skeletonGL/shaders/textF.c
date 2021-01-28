#version 330 core
in vec2 TexCoords;
out vec4 colorOUT;

uniform sampler2D text;

uniform vec4 color;
uniform float deltaTime;
uniform float timeElapsed;
uniform vec2 windowDimensions;

void main()
{
  vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);
  colorOUT = vec4(color) * sampled;

}
