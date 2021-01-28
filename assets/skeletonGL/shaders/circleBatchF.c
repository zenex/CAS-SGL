#version 330 core

// TexCoords are always normalized to 00 - 1.0 within the sprite boundaries
in vec2 TexCoords;
out vec4 colorOUT;

uniform sampler2D image;

uniform vec4 color;
uniform float deltaTime;
uniform float timeElapsed;
uniform vec2 windowDimensions;
uniform float circleBorder;



// NOTE: RAW position values (i.e. vec2(2.0, 4.0)) are relative to the
// SCREEN / viewport



float circle(vec2 pos, float r)
{
    return step(length(pos), r);
}

void main()
{
    /* HALF */
    /* if (gl_FragCoord.x > 640.0) */
    /* if (gl_FragCoord.x > windowDimensions.x/2) */
    /* if (gl_FragCoord.x > spritePosition.x) */
    /* { */
    /*     color = vec4(1.0, 0.0, 0.0, 1.0); */
    /* } */
    /* else */
    /* { */
    /*     vec4 texColor = spriteColor * texture(image, TexCoords); */
    /*     color = texColor; */

    /* } */

    /* vec4 texColor = spriteColor * texture(image, TexCoords); */
    /* texColor.a -= TexCoords.x; */

    /* color = texColor; */

    /* white circle black bg */
    /* vec2 centerOffset = vec2(0.5, 0.5); */
    /* float c = circle(TexCoords - centerOffset, 0.3); */
    /* color = vec4(vec3(c), 1.0); */

    vec2 uv = TexCoords;
    /* float circleBorder = 0.03; */
    float circleRadius = 0.5 - circleBorder; // Offset the center of the sprite minus the circle thickness toascii avoid cutting the circle
    vec4 bgColor = texture2D(image, TexCoords);

    // Offset the circle center
    uv -= vec2(0.5,0.5);

    float dist = sqrt(dot(uv,uv));
    if ( (dist > (circleRadius + circleBorder)) || (dist < (circleRadius - circleBorder)) )
        colorOUT = bgColor;
    else
        colorOUT = color;
    // works with time elapsed as DT
    // color = vec4(abs(sin(deltaTime)),0.0,0.0,1.0);
}
