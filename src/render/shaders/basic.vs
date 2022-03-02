#version 400 core

layout (location = 0) in vec3 v_Position;
layout (location = 1) in vec3 v_Color;

layout (std140) uniform Matrices {
    mat4 u_MVP;
};

uniform vec3 offset;
out vec4 f_Color;

void main() {
	gl_Position = u_MVP * vec4(v_Position, 1.0f);
	f_Color = vec4(v_Color, 1.0f);
}