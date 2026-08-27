#version 330

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;

// Input uniform values
uniform sampler2D texture0;
uniform vec4 colDiffuse;

// Output fragment color
out vec4 finalColor;

void main()
{
    // Texel color fetching from texture sampler
    float texelColor = texture(texture0, fragTexCoord).r;

    // final color is the color from the texture
    //    times the tint color (colDiffuse)
    //    times the fragment color (interpolated vertex color)
    finalColor = vec4(texelColor, texelColor, texelColor, 1.0)*colDiffuse*fragColor;
}
