#ifndef MESH_HPP
#define MESH_HPP

#include <string>
#include <vector>
#include <utility>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <GL/glew.h>

class Mesh {
public:
	Mesh(std::string filename, GLuint type);
	~Mesh() { release(); }

	// Return the bounding box of this object
	std::pair<glm::vec3, glm::vec3> boundingBox() const
	{ return std::make_pair(minBB, maxBB); }

	void load(std::string filename, GLuint type);
	void draw();
	glm::mat4 fixBB();

	// Mesh vertex format
	struct Vtx {
		glm::vec3 pos;		// Position
		glm::vec3 norm;		// Normal
		glm::vec2 tc;       // Tex Coord
		int type;
	};
	// Local geometry data
	std::vector<Vtx> vertices;

protected:
	void release();		// Release OpenGL resources

	// Bounding box
	glm::vec3 minBB;
	glm::vec3 maxBB;

	// OpenGL resources
	GLuint vao;		// Vertex array object
	GLuint vbuf;	// Vertex buffer
	GLsizei vcount;	// Number of vertices
	GLuint type;

private:
	// Disallow copy and move
	Mesh(const Mesh& other);
	Mesh(Mesh&& other);
	Mesh& operator=(const Mesh& other);
	Mesh& operator=(Mesh&& other);
};

#endif
