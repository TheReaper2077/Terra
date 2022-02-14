#version 400 core

in vec4 f_Color;
in vec2 f_TexCoord;
in float f_TextureIndex;

uniform sampler2D u_Textures[5];

void main() {
	gl_FragColor = texture(u_Textures[int(f_TextureIndex)], f_TexCoord) * f_Color;
}
