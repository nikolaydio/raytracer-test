#include "Scene.h"


bool IntersectSphere(Sphere& sph, glm::vec3 rpos, glm::vec3 rdir, float* dist_out) {
	glm::vec3 L = sph.pos - rpos;
	float tca = glm::dot(L, rdir);
	if(tca < 0) return false;
	float d2 = glm::dot(L, L) - tca * tca;
	if(d2 > sph.radius) return false;
	float thc = sqrt(sph.radius - d2);
	float t0 = tca - thc;
	float t1 = tca + thc;


	if(t0 > t1) {
		float temp = t0;
		t0 = t1;
		t1 = temp;
	}

	if(t1 < 0)
		return false;

	if(t0 < 0){
		*dist_out = t1;
		return true;
	}else{
		*dist_out = t0;
		return true;
	}
}

bool Scene::Trace(glm::vec3 pos, glm::vec3 dir, glm::vec3* out_pos, glm::vec3* out_normal, glm::vec3* color, glm::vec3* emit) {
	float distance = 0;
	float best_dist = 999999.0f;
	bool found = false;
	for(int i = 0; i < spheres.size(); ++i) {
		if(IntersectSphere(spheres[i], pos, dir, &distance)) {
			if(best_dist > distance) {
				glm::vec3 inters_point = pos + dir * distance;
				*out_normal = glm::normalize(inters_point - spheres[i].pos);
				*out_pos = inters_point;
				*color = spheres[i].color;
				*emit = spheres[i].emit;
				best_dist = distance;
				found = true;
			}
		}
	}
	return found;
}

glm::vec3 std_val(0.0,0.0,0.0);
glm::vec3 Scene::TraceRec(glm::vec3 pos, glm::vec3 dir, int tail) {
	if(tail > 2)
		return std_val;
	glm::vec3 color, emit, nrml, in_pos;
	tail++;
	if(Trace(pos, dir, &in_pos, &nrml, &color, &emit)) {
		glm::vec3 res_color = emit;
		// color from secondary trace
		res_color += TraceRec(in_pos, glm::reflect(dir, nrml), tail) * color * glm::max(0.0f, glm::dot(glm::reflect(dir, nrml), glm::normalize(in_pos - pos)));

		// global dir light
		float k = glm::dot(nrml, -this->directional);
		k = glm::clamp(k, 0.0f, 1.0f);
		res_color += this->dir_color * k;
		return res_color;
	}else{
		return std_val;
	}
}

glm::vec3 Scene::TracePixel(glm::vec3 pos, glm::vec3 dir) {
	return TraceRec(pos, dir, 0);
}