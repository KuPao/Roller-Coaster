#pragma once
#include <vector>
#include <string>
#include <glm\glm.hpp>
class ObjLoader {
public:
	ObjLoader();
	bool LoadObjFromFile(const char * path,
		std::vector<glm::vec3> & out_vertices,
		std::vector<glm::vec2> & out_uvs,
		std::vector<glm::vec3> & out_normals,
		std::vector<unsigned int> &out_materialIndices,
		std::vector<std::string> &out_mtls);
	bool loadMTL(const char * path,
		std::vector<glm::vec3> &Kd,
		std::vector<glm::vec3> &Ka,
		std::vector<glm::vec3> &Ks,
		std::vector<std::string> &out_name,
		std::vector<std::string> &texture
	);
};