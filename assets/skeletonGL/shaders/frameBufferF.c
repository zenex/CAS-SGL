#version 330 core

in  vec2  TexCoords;
out vec4  colorOUT;

uniform sampler2D scene;
uniform vec2 fboTextureDimensions;
uniform vec2 mousePosition;
uniform float deltaTime;
uniform float timeElapsed;

void main()
{
  //NOTE: on a 960GTX, the color is by default initialized to
  //proper values, in some intel HD integrated cards they do not
  //you have to explicitly initialize the color to the value you want
  colorOUT = vec4(0.0, 0.0, 0.0, 0.0);
  colorOUT = texture(scene, TexCoords);
}
