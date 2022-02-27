#version 400 core

layout (location = 0) in vec4 v_Position;
layout (location = 1) in vec4 v_Color;

layout (std140) uniform Matrices_2D {
	mat4 u_MVP;
};

out vec4 f_Color;

void main() {
	gl_Position = u_MVP * v_Position;
	f_Color = v_Color;
}