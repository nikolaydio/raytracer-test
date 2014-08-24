#pragma once

#include <vector>
#include <glm\glm.hpp>


inline void ClampVec(glm::vec3& val) {
	val.x = glm::clamp(val.x, 0.0f, 1.0f);
	val.y = glm::clamp(val.y, 0.0f, 1.0f);
	val.z = glm::clamp(val.z, 0.0f, 1.0f);
}

class Sphere {
public:
	Sphere(glm::vec3 p, float r) : pos(p), radius(r) {}

	glm::vec3 pos;
	float radius;
	glm::vec3 color;
	glm::vec3 emit;
};
class Scene {
public:
	bool Trace(glm::vec3 pos, glm::vec3 dir, glm::vec3* out_pos, glm::vec3* out_normal, glm::vec3* color, glm::vec3* emit);
	glm::vec3 TraceRec(glm::vec3 pos, glm::vec3 dir, int tail);
	glm::vec3 TracePixel(glm::vec3 pos, glm::vec3 dir);

	glm::vec3 dir_color;
	glm::vec3 directional;
	std::vector<Sphere> spheres;
};