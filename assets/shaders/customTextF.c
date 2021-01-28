#version 330 core
in vec2 TexCoords;
out vec4 color;

uniform sampler2D text;
uniform vec4 textColor;
uniform float deltaTime;


void main()
{
    /* float pct = abs(sin(deltaTime)); */

    /* vec3 newColor = vec3(0.0); */
    /* newColor = mix(colorA, colorB, pct); */

    /* // vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r); */
    /* vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r); */
    /* color = vec4(newColor, 1.0) * sampled; */

  vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);
  color = vec4(textColor) * sampled;
}
