#version 330

in vec2 fragTexCoord;

uniform sampler2D texture0;
uniform vec4 colDiffuse;

out vec4 finalColor;

void main()
{
    vec3 color = texture(texture0, fragTexCoord).rgb*colDiffuse.rgb;
    finalColor = vec4(color, 1.0);
}
