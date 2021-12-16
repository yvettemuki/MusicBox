#version 460

smooth in vec2 texCoord;
smooth in vec3 normal;
smooth in float w_sum;

layout(binding = 0) uniform sampler2D reflectTex;

out vec4 fragcolor;

void main()
{
	vec3 color_center = vec3(0.498, 0.498, 0.835);
	vec3 color_boundary = vec3(0.525, 0.658, 0.905);
	float mix_value = distance(texCoord.xy, vec2(1.f, 0.f));

	vec3 color_output = mix(color_center, color_boundary, mix_value);
	vec3 tex_color = 0.1 * texture(reflectTex, texCoord).rgb;
	fragcolor = vec4(tex_color + color_output + w_sum * 2, 1.f);
}