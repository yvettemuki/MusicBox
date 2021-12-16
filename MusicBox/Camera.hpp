#ifndef CAMERA_H
#define CAMERA_H

#include <gl/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace glm;

class Camera
{
public:
	Camera(GLuint shader, float fov, GLuint w, GLuint h, vec3 camOrigin, vec3 lookAtPoint) :
		shader(shader), 
		fov(fov), 
		winWidth(w), 
		winHeight(h), 
		camOrigin(camOrigin), 
		lookAtPoint(lookAtPoint)
	{
		uniView = glGetUniformLocation(shader, "V");
		uniProj = glGetUniformLocation(shader, "P");
		setup();
	};

	// Set up Camera
	void setup();
	void reset();
	mat4 getViewMatrix();
	mat4 getProjectionMatrix();

private:
	GLuint shader;
	float fov;
	mat4 view;
	mat4 proj;
	GLuint winWidth;
	GLuint winHeight;
	vec3 camOrigin;
	vec3 lookAtPoint;
	GLuint uniView;
	GLuint uniProj;
};

#endif 
