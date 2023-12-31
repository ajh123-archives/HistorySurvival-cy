#version 430

layout (location = 0) in uint in_data;

uniform mat4 MVP;

out vec2 uv;
out float ao;
out float logz;
flat out uint tex_id;
flat out uint normal;

float aos[4] = { 0, 1.0f/3, 2.0f/3, 1 };

vec2 uvs[4] = {
	vec2(0, 0),
	vec2(0, 1),
	vec2(1, 1),
	vec2(1, 0)
};

void main() {
	uint data = in_data;
	
	uint in_x = data & 63; data >>= 6;
	uint in_y = data & 63; data >>= 6;
	uint in_z = data & 63; data >>= 6;
	
	uint in_normal = data & 7; data >>= 3;
	uint in_ao = data & 3; data >>= 2;
	uint in_tex_id = data;

	float x = float(in_x) - 16;
	float y = float(in_y) - 16;
	float z = float(in_z) - 16;

    gl_Position = MVP * vec4(x, y, z, 1.0f);
	
	// logarithmic depth buffers
	// https://outerra.blogspot.com/2012/11/maximizing-depth-buffer-range-and.html
	const float C = 1;
	const float far = 1500;
	const float FC = 1.0/log(far*C + 1);
	
	gl_Position.z = (2.0*log(gl_Position.w*C + 1))/log(far*C+1) - 1; 
    gl_Position.z *= gl_Position.w;
	logz = log(gl_Position.w*C + 1)*FC;
	
	uv = uvs[gl_VertexID % 4];
	tex_id = in_tex_id;
	normal = in_normal;
	ao = aos[in_ao];
}