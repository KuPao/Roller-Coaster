#include <vector>
#include <windows.h>
#include <string>
#include <iostream>
#include <glm/glm.hpp>
#include "ObjLoader.h"

std::string ExePath() {
	char buffer[MAX_PATH];
	GetModuleFileNameA(NULL, buffer, MAX_PATH);
	std::string::size_type pos = std::string(buffer).find_last_of("\\/");
	return std::string(buffer).substr(0, pos);
}

ObjLoader::ObjLoader()
{
}

bool ObjLoader::LoadObjFromFile(const char * path,
	std::vector<glm::vec3> & out_vertices,
	std::vector<glm::vec2> & out_uvs,
	std::vector<glm::vec3> & out_normals,
	std::vector<unsigned int> &out_materialIndices,
	std::vector<std::string> &out_mtls) {
	printf("Loading OBJ file %s...\n", path);

	std::vector<unsigned int> vertexIndices, uvIndices, normalIndices, materialIndices;
	std::vector<glm::vec3> temp_vertices;
	std::vector<glm::vec2> temp_uvs;
	std::vector<glm::vec3> temp_normals;
	std::vector<std::string> temp_mtls;

	FILE * file = fopen(path, "r");
	if (file == NULL) {
		printf("Impossible to open the .obj file ! Are you in the right path ? See Tutorial 1 for details\n");
		return false;
	}
	int fcount = 0;//face count for material
	while (1) {

		char lineHeader[128];

		// read the first word of the line
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
			break; // EOF = End Of File. Quit the loop.

				   // else : parse lineHeader

		if (strcmp(lineHeader, "v") == 0) {//'v':°OÂI
			glm::vec3 vertex;
			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			temp_vertices.push_back(vertex);

		}
		else if (strcmp(lineHeader, "vt") == 0) {
			glm::vec2 uv;
			fscanf(file, "%f %f\n", &uv.x, &uv.y);
			// Invert V coordinate since we will only use DDS texture,
			//which are inverted.Remove if you want to use TGA or BMP loaders.
			temp_uvs.push_back(uv);
		}
		else if (strcmp(lineHeader, "vn") == 0) {
			glm::vec3 normal;
			fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
			temp_normals.push_back(normal);
		}
		else if (strcmp(lineHeader, "f") == 0) {
			//	std::string vertex1, vertex2, vertex3;
			unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
			int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0],
				&vertexIndex[1], &uvIndex[1], &normalIndex[1],
				&vertexIndex[2], &uvIndex[2], &normalIndex[2]);
			if (matches != 9) {
				printf("File can't be read by our simple parser :-( Try exporting with other options\n");
				return false;
			}
			fcount++;
			vertexIndices.push_back(vertexIndex[0]);
			vertexIndices.push_back(vertexIndex[1]);
			vertexIndices.push_back(vertexIndex[2]);
			uvIndices.push_back(uvIndex[0]);
			uvIndices.push_back(uvIndex[1]);
			uvIndices.push_back(uvIndex[2]);
			normalIndices.push_back(normalIndex[0]);
			normalIndices.push_back(normalIndex[1]);
			normalIndices.push_back(normalIndex[2]);
		}
		else if (strcmp(lineHeader, "usemtl") == 0) {
			out_materialIndices.push_back(fcount);//last material used face counts	
			char material[50];
			fscanf(file, "%s", material);
			out_mtls.push_back(material);
			fcount = 0;//face count for next material
		}
		else {
			// Probably a comment, eat up the rest of the line
			char stupidBuffer[1000];
			fgets(stupidBuffer, 1000, file);
		}
	}
	out_materialIndices.push_back(fcount);//at the end , push the last faces
										  // For each vertex of each triangle
	for (unsigned int i = 0; i<vertexIndices.size(); i++) {
		// Get the indices of its attributes
		unsigned int vertexIndex = vertexIndices[i];
		unsigned int uvIndex = uvIndices[i];
		unsigned int normalIndex = normalIndices[i];

		// Get the attributes thanks to the index
		glm::vec3 vertex = temp_vertices[vertexIndex - 1];
		glm::vec2 uv = temp_uvs[uvIndex - 1];
		glm::vec3 normal = temp_normals[normalIndex - 1];

		// Put the attributes in buffers
		out_vertices.push_back(vertex);
		out_uvs.push_back(uv);
		out_normals.push_back(normal);
	}

	return true;


	/*char lineHeader[1000];
	std::vector<float> temp_vertices;
	std::vector<float> temp_uvs;
	std::vector<float> temp_normals;
	std::vector<unsigned int> indices;
	float vertex[3], uv[2], normal[3];
	unsigned int index[3][3], f = 0;
	std::cout << "my directory is " << ExePath() << "\n";
	FILE* objfile = fopen(path, "r");
	if (objfile == NULL) {
		printf("Impossible to open the file !\n");
		return false;
	}

	while (fscanf(objfile, "%s", lineHeader) != EOF) {
		if (strcmp(lineHeader, "v") == 0) {
			fscanf(objfile, "%f %f %f\n", &vertex[0], &vertex[1], &vertex[2]);
			temp_vertices.push_back(vertex[0]);
			temp_vertices.push_back(vertex[1]);
			temp_vertices.push_back(vertex[2]);
		}
		else if (strcmp(lineHeader, "vt") == 0) {
			fscanf(objfile, "%f %f\n", &uv[0], &uv[1]);
			uv[1] = -uv[1];
			temp_uvs.push_back(uv[0]);
			temp_uvs.push_back(uv[1]);
		}
		else if (strcmp(lineHeader, "vn") == 0) {
			fscanf(objfile, "%f %f %f\n", &normal[0], &normal[1], &normal[2]);
			temp_normals.push_back(normal[0]);
			temp_normals.push_back(normal[1]);
			temp_normals.push_back(normal[2]);
		}
		else if (strcmp(lineHeader, "f") == 0) {
			int matches = fscanf(objfile, "%u/%u/%u %u/%u/%u %u/%u/%u",
				&index[0][0], &index[1][0], &index[2][0], &index[0][1], &index[1][1], &index[2][1], &index[0][2], &index[1][2], &index[2][2]);
			//printf("matches=%d,f=%u\n", matches, f++);
			if (matches != 9) {
				printf("File can't be read by our simple parser ( Try exporting with other options\n");
				return false;
			}
			indices.push_back(index[0][0]);
			indices.push_back(index[1][0]);
			indices.push_back(index[2][0]);
			indices.push_back(index[0][1]);
			indices.push_back(index[1][1]);
			indices.push_back(index[2][1]);
			indices.push_back(index[0][2]);
			indices.push_back(index[1][2]);
			indices.push_back(index[2][2]);
		}
	}

	for (unsigned int i = 0; i < indices.size(); i += 3) {
		vertices->push_back(temp_vertices[(indices[i] - 1) * 3]);
		vertices->push_back(temp_vertices[(indices[i] - 1) * 3 + 1]);
		vertices->push_back(temp_vertices[(indices[i] - 1) * 3 + 2]);
		int x = (indices[i + 1] - 1) * 3;
		uvs->push_back(temp_normals[(indices[i + 1] - 1) * 3]);
		uvs->push_back(temp_normals[(indices[i + 1] - 1) * 3 + 1]);
		uvs->push_back(temp_normals[(indices[i + 1] - 1) * 3 + 2]);
		normals->push_back(temp_normals[(indices[i + 2] - 1) * 3]);
		normals->push_back(temp_normals[(indices[i + 2] - 1) * 3 + 1]);
		normals->push_back(temp_normals[(indices[i + 2] - 1) * 3 + 2]);
	}
	return true;*/
}

bool ObjLoader::loadMTL(const char * path,
	std::vector<glm::vec3> &Kd,
	std::vector<glm::vec3> &Ka,
	std::vector<glm::vec3> &Ks,
	std::vector<std::string> &out_name,
	std::vector<std::string> &texture
)
{
	printf("Loading MTL file %s...\n", path);
	FILE * file = fopen(path, "r");
	if (file == NULL) {
		printf("Impossible to open the .mtl file ! Are you in the right path ? See Tutorial 1 for details\n");
		return false;
	}
	while (1) {
		char lineHeader[128];
		// read the first word of the line
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
			break; // EOF = End Of File. Quit the loop.

				   // else : parse lineHeader
		if (strcmp(lineHeader, "newmtl") == 0) {
			char material[50];
			fscanf(file, "%s", material);
			out_name.push_back(material);
		}
		else if (strcmp(lineHeader, "Kd") == 0) {
			glm::vec3 diffuse;
			fscanf(file, "%f %f %f\n", &diffuse.x, &diffuse.y, &diffuse.z);
			Kd.push_back(diffuse);
		}
		else if (strcmp(lineHeader, "Ka") == 0) {
			glm::vec3 ambient;
			fscanf(file, "%f %f %f\n", &ambient.x, &ambient.y, &ambient.z);
			Ka.push_back(ambient);
		}
		else if (strcmp(lineHeader, "Ks") == 0) {
			glm::vec3 specular;
			fscanf(file, "%f %f %f\n", &specular.x, &specular.y, &specular.z);
			Ks.push_back(specular);
		}
		else if (strcmp(lineHeader, "map_Kd") == 0) {
			char tex[50];
			fscanf(file, "%s\n", tex);
			texture.push_back(tex);
		}
		else {
			// Probably a comment, eat up the rest of the line
			char stupidBuffer[1000];
			fgets(stupidBuffer, 1000, file);
		}
	}

	return true;
}