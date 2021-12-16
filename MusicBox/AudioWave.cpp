#include "AudioWave.hpp"
#include "utils.hpp"

//void AudioWave::initShaders()
//{
//	vector<GLuint> shaders;
//	shaders.push_back(compileShader(GL_VERTEX_SHADER, "shaders/sh_v_gpgpu.glsl"));
//	shaders.push_back(compileShader(GL_FRAGMENT_SHADER, "shaders/sh_f_gpgpu.glsl"));
//	shader = linkProgram(shaders);
//	// Release shader sources
//	for (auto s = shaders.begin(); s != shaders.end(); ++s)
//		glDeleteShader(*s);
//	shaders.clear();
//
//	// get uniform location
//
//
//	glUseProgram(0);
//
//	assert(glGetError() == GL_NO_ERROR);
//}

void AudioWave::initVertices(int width, int height)
{
	if (vertices.size() > 0) {
		vertices.clear();
	}
	if (indexs.size() > 0) {
		indexs.clear();
	}

	float du = 0, dv = 0;
	int offsetU = width / 2, offsetV = height / 2;
	if (width % 2 == 0) du = 0.5;
	if (height % 2 == 0) dv = 0.5;

	for (int v = 0; v < height; v++) {
		for (int u = 0; u < width; u++) {
			// Create vertex to draw grid
			int idx = u + v * width;
			vert v0 = {
			 vec3((float)(u - offsetU + du), (float)(v - offsetV + dv), 0.f),
			 vec2((float)u / (float)(width - 1), (float)v / (float)(height - 1)),
			 vec3(0.f, 0.f, 1.f)
			};
			vertices.push_back(v0);
			/*cout << v0.pos.x << ", " << v0.pos.y << endl;
			cout << v0.tc.x << "- " << v0.tc.y << endl;*/

			// Create element idx to draw grid
			if (u != width - 1 && v != height - 1) {
				// When not to the grid edge index
				// Horizontal line index
				indexs.push_back(idx);
				indexs.push_back(idx + 1);
				indexs.push_back(idx + width + 1);
				// Vertical line index
				indexs.push_back(idx);
				indexs.push_back(idx + width);
				indexs.push_back(idx + width + 1);
			}
		}
	}

	vcount = indexs.size();

	// Bind buffers
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vert), vertices.data(), GL_DYNAMIC_DRAW);
	// Specify vertex attributes
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vert), 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(vert), (GLvoid*)sizeof(glm::vec3));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(vert), (GLvoid*)(sizeof(glm::vec3) + sizeof(glm::vec2)));
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexs.size() * sizeof(GLuint), indexs.data(), GL_DYNAMIC_DRAW);

	// Cleanup state
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void AudioWave::initTexture()
{
	
}

void AudioWave::init()
{
	//initShaders();
	initVertices(width, height);
	initTexture();
}

void AudioWave::draw()
{
	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, vcount, GL_UNSIGNED_INT, NULL);
	glBindVertexArray(NULL);
}
