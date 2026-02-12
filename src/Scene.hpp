#pragma once

#include "core-utils.hpp"
#include "BVH.hpp"
#include "Vertex.hpp"
#include "Texture.hpp"
#include "Object.hpp"

#include <vector>
#include <filesystem>

using namespace cu::math;


struct Material {
	
};

enum PrimitiveType {
	PrimSphere,
	PrimPlane,
	PrimCylinder,
	PrimCone,
	PrimHyperboloid,
};

struct Primitive {
	vec4 orientation;
	PrimitiveType type;
	uint32_t radius;
	uint32_t height;
	int32_t a;
	int32_t b;
	int32_t c;
	int32_t d;
};

struct SubMesh {
	std::string name;
	uint32_t materialIndex;

	uint32_t indexOffset;
	uint32_t indexCount;
};

struct Mesh {
	std::string name;
	std::vector<SubMesh> subMeshes;
};

enum AssetType {
	AssetObject,
	AssetPrimitive
};

struct Asset { // en faire une class
	std::string name;
	AssetType type;
	std::filesystem::path sourcePath;
	uint32_t primitive_id;
	std::vector<Mesh> meshes;
	mat4 worldTransform;
	uint32_t blasIndex;
	uint32_t vertexOffset;
	uint32_t vertexCount;
};

class Scene {

private:
	std::vector<Vertex> vertices_;
	std::vector<uint32_t> indexes_;
	std::vector<Material> materials_;
	std::vector<Primitive> primitives_;

	// Faudra un unique buffer avec tous les blas nodes car la y va y'avoir du pointeur dans blas
	std::vector<BVH> blas_;
	BVH tlas_;

	// CPU ONLY (ou envoyé differament au gpu)
	std::vector<Asset> assets_;
	std::vector<Texture> textures_;
	

public:
	Scene() {}
	~Scene() {}

	void loadObject(Object &obj);

};
