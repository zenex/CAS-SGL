#version 330 core

out vec4 colorOUT;


uniform vec4 color;
uniform float deltaTime;
uniform float timeElapsed;
uniform vec2 windowDimensions;

void main()
{
  //TexCoords.y = 1.0f - TexCoords.y;
  colorOUT = color;
  /* colorOUT = vec4(abs(sin(timeElapsed)),0.0,0.0,1.0); */
}
