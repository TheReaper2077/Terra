#version 400 core

// in vec4 f_Color;
in vec2 f_TexCoord;
in float f_TextureIndex;

uniform sampler2D u_Textures[5];

const vec4 fogcolor = vec4(0.6, 0.8, 1.0, 1.0);
const float fogdensity = .00003;

void main() {
	float z = gl_FragCoord.z / gl_FragCoord.w;
  	float fog = clamp(exp(-fogdensity * z * z), 0.2, 1);

	gl_FragColor = mix(fogcolor, texture(u_Textures[int(f_TextureIndex)], f_TexCoord) * vec4(1), fog);
}
