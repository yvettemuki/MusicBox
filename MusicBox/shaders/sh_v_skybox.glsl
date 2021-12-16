#version 460

layout(location = 0) in vec3 pos;

out vec3 texCoords;

uniform mat4 P;
uniform mat4 V;

void main()
{
	texCoords = pos;
	gl_Position = P * V * vec4(pos, 1.f);
}