#ifndef AUDIO_WAVE_H
#define AUDIO_WAVE_H

#include <gl/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <vector>

using namespace glm;
using namespace std;

struct vert {
	vec3 pos;
	vec2 tc;
	vec3 norm;
};

class AudioWave
{
public:
	AudioWave(GLuint shader, GLuint width, GLuint height) : shader(shader), width(width), height(height) 
	{
		init();
	};
	void init();
	void draw();
	//void initShaders();
	void initVertices(int width, int height);
	void initTexture();

private:
	GLuint width;
	GLuint height;
	GLuint vao;
	GLuint vbo;
	GLuint ibo;
	GLuint shader;
	GLuint vcount;
	vector<vert> vertices;
	vector<GLint> indexs;
};

#endif // AUDIO_WAVE_H

