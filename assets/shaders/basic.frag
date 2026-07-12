#version 430 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D texture1;

void main()
{
	FragColor.rgb = texture(texture1, TexCoord).rgb;
}

