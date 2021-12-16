#version 460

layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 uv;
layout(location = 2) in vec3 norm;

smooth out vec2 texCoord;
smooth out vec3 normal;
smooth out float w_sum;

uniform mat4 P;
uniform mat4 V;
uniform mat4 M;
uniform float time;
uniform float audio_offset;

void main()
{
	texCoord = uv;
	normal = norm;

	// Sine summation formula
	float L_i = 3.f;
	float w_i = 2.f / L_i;
	float A_i = 0.1f;
	float S_i = 15.f;
	float alpha_i = S_i * w_i;
	vec2 D_i = vec2(2.f, 5.f);
	// calculate height deviatioin
	float W_i = A_i * sin(dot(D_i, pos.xz) * w_i + time * alpha_i);
	// binormal(bitangent): calculate 3rd elem partial derivative
	float B_i = A_i * w_i * D_i.x * cos(dot(D_i, pos.xz) * w_i + time * alpha_i);
	// tangent: calculate 3rd elem partial derivative
	float T_i = A_i * w_i * D_i.y * cos(dot(D_i, pos.xz) * w_i + time * alpha_i);

	float L_i1 = 5.f;
	float w_i1 = 2.f / L_i1;
	float A_i1 = 0.2f;
	float S_i1 = 2.f;
	float alpha_i1 = S_i1 * w_i1;
	vec2 D_i1 = vec2(-3.f, -2.f);
	float W_i1 = A_i1 * sin(dot(D_i1, pos.xz) * w_i1 + time * alpha_i1);
	float B_i1 = A_i1 * W_i1 * D_i1.x * cos(dot(D_i1, pos.xz) * w_i1 + time * alpha_i1);
	float T_i1 = A_i1 * w_i1 * D_i1.y * cos(dot(D_i1, pos.xz) * w_i1 + time * alpha_i1);

	float L_i2 = 4.f;
	float w_i2 = 2.f / L_i2;
	float A_i2 = 0.1f;
	float S_i2 = 2.f;
	float alpha_i2 = S_i2 * w_i2;
	vec2 D_i2 = vec2(5.f, -3.f);
	float W_i2 = A_i2 * sin(dot(D_i2, pos.xy) * w_i2 + time * alpha_i2);
	float B_i2 = A_i2 * W_i2 * D_i2.x * cos(dot(D_i2, pos.xz) * w_i2 + time * alpha_i2);
	float T_i2 = A_i2 * w_i2 * D_i2.y * cos(dot(D_i2, pos.xz) * w_i2 + time * alpha_i2);

	float L_i3 = 6.f;
	float w_i3 = 2.f / L_i3; 
	float A_i3 = 0.5f;
	float S_i3 = 5.f;
	float alpha_i3 = S_i3 * w_i3;
	vec2 D_i3 = vec2(-1.f, 4.f);
	float W_i3 = A_i3 * sin(dot(D_i3, pos.xy) * w_i3 + time * alpha_i3);
	float B_i3 = A_i3 * W_i3 * D_i3.x * cos(dot(D_i3, pos.xz) * w_i3 + time * alpha_i3);
	float T_i3 = A_i3 * w_i3 * D_i3.y * cos(dot(D_i3, pos.xz) * w_i3 + time * alpha_i3);

	// sum z deviation direction
	float W = sin(W_i + W_i1 + W_i2 + W_i3);

	// calculate offset
	vec3 offset = vec3(0.f, 0.f, (0.3 + audio_offset) * W);

	// Interpolate output
	w_sum  = 0.1 * W * audio_offset;

	gl_Position = P * V * M * vec4(pos + offset, 1.f);
}