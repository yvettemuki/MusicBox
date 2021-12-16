#version 460

layout(binding = 0) uniform samplerCube skybox_tex;

in vec3 texCoords;

out vec4 fragcolor;

void main()
{
	fragcolor = texture(skybox_tex, texCoords);
}