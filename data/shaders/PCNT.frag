#version 330

uniform sampler2D colorTexture;
smooth in vec4 color;
smooth in vec2 texCoord;
out vec4 fragColor;

void main(void)
{
	vec4 textureColor = texture2D(colorTexture, texCoord);
    fragColor = textureColor * color;
}