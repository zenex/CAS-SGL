#version 330 core

in vec2 TexCoords;
out vec4 colorOUT;

uniform sampler2D image;

uniform vec4 color;
uniform float deltaTime;
uniform float timeElapsed;
uniform vec2 windowDimensions;
uniform float circleBorder;

void main()
{
    //TexCoords.y = 1.0f - TexCoords.y;
    /* float tmp = abs(sin(timeElapsed)); */
    /* vec4 c = vec4(color.x * tmp, color.y * tmp, color.z * tmp, 1.0); */
    /* colorOUT = c * texture(image, TexCoords); */
    colorOUT = color * texture(image, TexCoords);
}
