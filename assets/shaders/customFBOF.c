#version 330 core
in  vec2  TexCoords;
out vec4  color;

uniform sampler2D scene;
uniform vec2 textureDimensions;
uniform vec2 mousePosition;
uniform float deltaTime;
/* uniform vec2      offsets[9]; */
/* uniform int       edge_kernel[9]; */
/* uniform float     blur_kernel[9]; */


uniform bool chaos;
uniform bool confuse;
uniform bool shake;

vec2 currentPos = gl_FragCoord.xy / textureDimensions.xy;
vec2 mousePos = mousePosition.xy / textureDimensions.xy;

// Plot a line on Y using a value between 0.0-1.0
float plot(vec2 st, float pct){
    return  smoothstep( pct-0.02, pct, st.y) -
        smoothstep( pct, pct+0.02, st.y);
}

void main()
{
    //NOTE: on a 960GTX, the color is by default initialized to
    //proper values, in some intel HD integrated cards they do not
    //you have to explicitly initialize the color to the value you want
    color = vec4(0.0, 0.0, 0.0, 0.0);
    //BLOOM
    /* vec4 sum = vec4(0); */
    /* vec2 texcoord = TexCoords; */
    /* int j; */
    /* int i; */


    /* for(i = 0 ; i < 10; i++) */
    /*   { */
    /*     for (j = 0; j < 10; j++) */
    /*       { */
    /*         sum += texture2D(scene, texcoord + vec2(j, i)*0.0006) * 0.10; */
    /*       } */
    /*   } */
    /* if (texture2D(scene, texcoord).r < 0.3) */
    /*   { */
    /*     color = sum*sum*0.012 + texture2D(scene, texcoord); */
    /*   } */
    /* else */
    /*   { */
    /*     if (texture2D(scene, texcoord).r < 0.5) */
    /*       { */
    /*         color = sum*sum*0.009 + texture2D(scene, texcoord); */
    /*       } */
    /*     else */
    /*       { */
    /*         color = sum*sum*0.0075 + texture2D(scene, texcoord); */
    /*       } */
    /*   } */

    /* lens */
    /* vec2 lensRadius = vec2(0.31, 0.30); */
    /* vec4 Color = texture(scene, TexCoords.xy); */
    /* float dist = distance(TexCoords.xy, vec2(0.5, 0.5)); */
    /* Color.rgb *= smoothstep(lensRadius.x, lensRadius.y, dist); */
    /* color = Color; */


    /* /\* default *\/ */
    /* vec3 sample[9]; */
    /* // sample from texture offsets if using convolution matrix */
    /* if(chaos || shake) */
    /*   for(int i = 0; i < 9; i++) */
    /*     sample[i] = vec3(texture(scene, TexCoords.st + offsets[i])); */

    /* // process effects */
    /* if(chaos) */
    /*   { */
    /*     for(int i = 0; i < 9; i++) */
    /*       color += vec4(sample[i] * edge_kernel[i], 0.0f); */
    /*     color.a = 1.0f; */
    /*   } */
    /* else if(confuse) */
    /*   { */
    /*     color = vec4(1.0 - texture(scene, TexCoords).rgb, 1.0); */
    /*   } */
    /* else if(shake) */
    /*   { */
    /*     for(int i = 0; i < 9; i++) */
    /*       color += vec4(sample[i] * blur_kernel[i], 0.0f); */
    /*     color.a = 1.0f; */
    /*   } */
    /* else */
    /*   { */
    /*     color = texture(scene, TexCoords); */
    /*   } */
    /* color.x = 0; */

    color = texture(scene, TexCoords);
    /* vec2 currentPos = gl_FragCoord.xy / textureDimensions.xy; */
    /* vec2 mousePos = mousePosition.xy / textureDimensions.xy; */
    color.xyz -= mousePos.y;
    color.xyz += mousePos.x;
    if (currentPos.y == mousePos.y)
        color = vec4(0.0);

    /* vec3 newColor = vec3(currentPos.x); */
    /* // plot a line */
    /* float pct = plot(currentPos, currentPos.x); */
    /* newColor = (1.0-pct)*newColor+pct*vec3(0.0,1.0,0.0); */

    /* color = vec4(newColor.xyz, color.w); */

    // color animation
    /* vec3 timeColor = vec3(0.0); */
    /* float anim = abs(sin(deltaTime)); */
    /* vec3 colorA = vec3(0.149,0.141,0.912); */
    /* vec3 colorB = vec3(1.000,0.833,0.224); */

    /* timeColor = mix(colorA, colorB, anim); */
    /* color = vec4(timeColor.xyz, color.w); */


}
