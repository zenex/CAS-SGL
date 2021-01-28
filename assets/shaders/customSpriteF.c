#version 330 core

in vec2 TexCoords;
out vec4 color;

uniform sampler2D image;
uniform vec4 spriteColor;
uniform float deltaTime;
uniform vec2 spriteDimensions;

vec2 currentPos = gl_FragCoord.xy / spriteDimensions.xy;

float wave(float x, float amount) {
  return (sin(x * amount) + 1.) * .5;
}

void main()
{
  vec4 texColor = spriteColor * texture(image, TexCoords);
  color = texColor;
  /* color.r = wave(texColor.r, 10.); */
  /* color.g = wave(texColor.g, 20.); */
  /* color.b = wave(texColor.b, 40.); */
  /* color.a = texColor.a; */
}
