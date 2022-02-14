#version 400 core

layout (location = 0) in vec4 v_Position;
layout (location = 1) in vec4 v_Color;
layout (location = 2) in vec3 v_Normal;

layout (std140) uniform Matrices {
    mat4 u_Projection;
    mat4 u_View;
};
uniform mat4 u_Model;
uniform mat3 u_NormalMatrix;

out vec4 f_Color;
out vec3 f_Normal;
out vec3 f_FragPos;

void main() {
	gl_Position = u_Projection * u_View * u_Model * v_Position;
	f_Color = v_Color;
	f_Normal = v_Normal;
	f_FragPos = vec3(u_Model * v_Position);
}