#version 400 core

layout (location = 0) in vec3 v_Position;
layout (location = 1) in vec3 v_Color;
// layout (location = 2) in vec2 v_TexCoord;
// layout (location = 3) in float v_TextureIndex;

layout (std140) uniform Matrices {
    mat4 u_MVP;
};

uniform vec3 offset;

out float f_TextureIndex;
out vec2 f_TexCoord;
// out vec4 f_Color;

void main() {
	gl_Position = u_MVP * vec4(v_Position + offset, 1.0f);
	// f_Color = v_Color;
	f_TextureIndex = v_Color.z;
	f_TexCoord = v_Color.xy;
}