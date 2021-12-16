#version 460

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normal; 
layout(location = 2) in vec2 tc;
layout(location = 3) in int type;

uniform float current_time;
uniform float prev_time;
uniform float delta_time;
uniform mat4 M;
uniform mat4 V;
uniform mat4 P;
uniform float audio_offset;

smooth out vec3 norm;
smooth out vec2 texCoord;
smooth out vec3 cubeTexCoord;
smooth out vec3 pos_w;
smooth out vec3 nw;


void main()
{
	mat4 mvp = P * V * M;
	gl_Position = mvp * vec4(pos + normal * audio_offset, 1.f);  // offset from center
	//norm = normal;
	texCoord = tc; 
	cubeTexCoord = pos;
	pos_w = vec3(M * vec4(pos, 1.f));
	nw = vec3(M * vec4(normal, 0.f));
}

