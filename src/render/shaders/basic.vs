#version 400 core

layout (location = 0) in vec4 v_Position;
layout (location = 1) in vec4 v_Color;

layout (std140) uniform Matrices {
    mat4 u_Projection;
    mat4 u_View;
	mat4 u_Model;
};

out vec4 f_Color;

void main() {
	gl_Position = u_Projection * u_View * u_Model * v_Position;
	f_Color = v_Color;
}