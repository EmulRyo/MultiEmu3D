#version 330

in vec2 fragTexCoord;

uniform sampler2D texture0;
uniform vec4 colDiffuse;

out vec4 finalColor;

void main()
{
    float color = texture(texture0, fragTexCoord).r;
    finalColor = vec4(color, color, color, 1.0) * colDiffuse;
}
