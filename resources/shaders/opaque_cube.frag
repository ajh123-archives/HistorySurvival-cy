#version 430

in vec2 uv;
in float ao;
flat in uint tex_id;
flat in uint normal;

uniform sampler2DArray texures;
uniform uint enable_diffuse;
uniform uint enable_ao;

out vec4 color;

float diffuse[6] = { 0.9f, 1.0f, 0.9f, 0.9f, 0.8f, 0.9f };

void main() {
	float factor = 1;
	float ao_ = 1-(ao-1)*(ao-1)/2;

	if (enable_ao == 1) factor *= ao_;
	if (enable_diffuse == 1) factor *= diffuse[normal];
	
    color = factor * texture(texures, vec3(uv, tex_id));
}