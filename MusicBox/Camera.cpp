#include "Camera.hpp"
#include <glm/gtc/type_ptr.hpp>

void Camera::setup()
{
	float winAspect = (float)winWidth / (float)winHeight;

	// Create perspective projection matrix
	proj = perspective(fov, winAspect, 0.1f, 100.0f);

	// Create view transformation matrix
	view = lookAt(
		vec3(camOrigin.x, camOrigin.y, camOrigin.z),
		vec3(lookAtPoint.x, lookAtPoint.y, lookAtPoint.z),
		vec3(0.0f, 1.0f, 0.0f)
	);

	glUniformMatrix4fv(uniView, 1, GL_FALSE, value_ptr(view));
	glUniformMatrix4fv(uniProj, 1, GL_FALSE, value_ptr(proj));
}

mat4 Camera::getViewMatrix()
{
	return view;
}

mat4 Camera::getProjectionMatrix()
{
	return proj;
}

