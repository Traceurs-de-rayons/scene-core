#pragma once

#include "core-utils.hpp"
#include "BVH.hpp"

#include <vector>
#include <filesystem>

using namespace cu::math;


struct Material {
	
};

struct SubMesh {
	std::string name;
	uint32_t materialIndex;

	uint32_t vertexOffset;
	uint32_t vertexCount;
	uint32_t indexOffset;
	uint32_t indexCount;
};

struct Mesh {
	std::string name;
	std::vector<SubMesh> subMeshes;
};

struct Asset {
	std::string name;
	std::filesystem::path sourcePath;
	std::vector<Mesh> meshes;
	mat4 worldTransform;
	uint32_t blasIndex;
};

struct alignas(16) Vertex {
	vec4 pos;
	vec4 normal;
	vec2 uv;
};

class Scene {

private:
	std::vector<Vertex> vertices_;
	std::vector<unsigned int> indexes_;
	std::vector<Material> materials_;

	std::vector<BVH> blas_;
	BVH tlas_;

	// CPU ONLY
	std::vector<Asset> assets_;

public:
	

};
