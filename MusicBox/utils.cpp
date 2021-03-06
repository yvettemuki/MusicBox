#include <iostream>
#include <sstream>
#include <fstream>
#include "utils.hpp"

// import stb_image
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using namespace std;

GLuint compileShader(GLenum type, string filename, string prepend) {
	// Read the file
	ifstream file(filename);
	if (!file.is_open()) {
		stringstream ss;
		ss << "Could not open " << filename << "!" << endl;
		throw runtime_error(ss.str());
	}
	stringstream buffer;
	buffer << prepend << endl;
	buffer << file.rdbuf();
	string bufStr = buffer.str();
	const char* bufCStr = bufStr.c_str();
	GLint length = bufStr.length();

	// Compile the shader
	GLuint shader = glCreateShader(type);
	glShaderSource(shader, 1, &bufCStr, &length);
	glCompileShader(shader);

	// Make	sure compilation succeeded
	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE) {
		// Compilation failed, get the info log
		GLint logLength;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
		vector<GLchar> logText(logLength);
		glGetShaderInfoLog(shader, logLength, NULL, logText.data());

		// Construct an error message with the compile log
		stringstream ss;
		string typeStr = "";
		switch (type) {
		case GL_VERTEX_SHADER:
			typeStr = "vertex"; break;
		case GL_FRAGMENT_SHADER:
			typeStr = "fragment"; break;
		}
		ss << "Error compiling " + typeStr + " Shader!" << endl << endl << logText.data() << endl;

		// Cleanup shader and throw an exception
		glDeleteShader(shader);
		throw runtime_error(ss.str());
	}

	return shader;
}

GLuint linkProgram(vector<GLuint> shaders) {
	GLuint program = glCreateProgram();

	// Attach the sahders and link the program
	for (auto it = shaders.begin(); it != shaders.end(); ++it)
		glAttachShader(program, *it);
	glLinkProgram(program);

	// Detach shaders
	for (auto it = shaders.begin(); it != shaders.end(); ++it)
		glDetachShader(program, *it);

	// Make sure link succeeded
	GLint status;
	glGetProgramiv(program, GL_LINK_STATUS, &status);
	if (status == GL_FALSE) {
		// Link failed, get the info log
		GLint logLength;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
		vector<GLchar> logText(logLength);
		glGetProgramInfoLog(program, logLength, NULL, logText.data());

		// Construct an error message with the compile log
		stringstream ss;
		ss << "Error linking program!" << endl << endl << logText.data() << endl;

		// Cleanup program and throw an exception
		glDeleteProgram(program);
		throw runtime_error(ss.str());
	}

	return program;

}