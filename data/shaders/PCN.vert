#version 330

uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

layout(location=0) in vec4 in_Position;
layout(location=2) in vec4 in_Color;
smooth out vec4 ex_Color;

void main(void)
{
    gl_Position = (ProjectionMatrix * ViewMatrix * ModelMatrix) * in_Position;
	ex_Color = vec4(0.0, 1.0, 0.0, 1.0);
}