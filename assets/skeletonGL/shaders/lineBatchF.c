#version 330 core

out vec4 colorOUT;


uniform vec4 color;
uniform float deltaTime;
uniform float timeElapsed;
uniform vec2 windowDimensions;

void main()
{
    /* colorOUT = vec4(abs(sin(timeElapsed)),0.0,0.0,1.0); */
    colorOUT = color;
}
