#version 400 core

in vec4 f_Color;
in vec3 f_Normal;
in vec3 f_FragPos;

uniform vec3 u_LightPos;
uniform vec3 u_ViewPos;
uniform vec3 u_LightColor;

void main() {
	// ambient
	float ambient_strength = 0.1f;
	vec3 ambient = ambient_strength * u_LightColor;

	// diffuse
	vec3 norm = normalize(f_Normal);
	vec3 light_dir = normalize(u_LightPos - f_FragPos);
	float diff = max(dot(norm, light_dir), 0.0);
	vec3 diffuse = diff * light_dir;
	
	float specularStrength = 0.5;
    vec3 viewDir = normalize(u_ViewPos - f_FragPos);
    vec3 reflectDir = reflect(-light_dir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 2);
    vec3 specular = specularStrength * spec * f_Color.xyz;  

	vec3 result = ambient + diffuse + specular;
	gl_FragColor = vec4(result, 1.0f) * f_Color;
}