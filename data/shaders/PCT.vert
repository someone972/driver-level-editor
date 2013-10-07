#version 330

uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

layout(location=0) in vec4 in_Position;
layout(location=2) in vec4 in_Color;
layout(location=3) in vec2 in_TexCoord;
smooth out vec4 color;
smooth out vec2 texCoord;

void main(void)
{
    gl_Position = (ProjectionMatrix * ViewMatrix * ModelMatrix) * in_Position;
	color = in_Color;
	texCoord = in_TexCoord;
}