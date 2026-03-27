#pragma once

#include "Mesh.hpp"
#include "utils/Transform.hpp"
#include "BVH.hpp"
#include "Primitive.hpp"

#include <stdint.h>
#include <vector>
#include <memory>
#include <optional>
#include <variant>

enum AssetType {
	AssetObject,
	AssetPrimitive,
	AssetInstance,
	// AssetVolume
};

namespace sceneIO::tdr { class SceneLoader; }

class Asset 
{

private:
	struct ObjectData
	{
		std::vector<std::unique_ptr<Mesh>> meshes;
	};

	struct PrimitiveData
	{
		struct Plane
		{
			vec3 normal;
		};

		struct Sphere
		{
			float radius;
		};

		struct Cylinder
		{
			float radius;
			float height;
		};

		struct Cone
		{
			float radius;
			float height;
		};

		struct Hyperboloid
		{
			float height;
			float a;
			float b;
			float c;
			float shape;
		};

		std::variant<Plane, Sphere, Cylinder, Cone, Hyperboloid> primitive;
	};

	struct InstanceData
	{
		std::string parent_name;
	};

	std::string name_;
	std::string label_;

	std::variant<ObjectData, PrimitiveData, InstanceData> content_;

	std::string material_;
	Transform transform_;

	BVH blas_;
	uint32_t blasIndex_; // index du blas sur le buffer gpu, uniquement si tous les submesh sont bien envoye

	friend void sceneIO::parser::parseObj(Asset& asset, std::istream& in,
	                                      sceneIO::parser::ObjErrorCollector& errors,
	                                      uint64_t startLine, uint64_t startColumn);
	friend class sceneIO::tdr::SceneLoader;
	friend class Scene;
	
public:
	Asset() {}
	Asset(Asset& other) = delete;
	Asset(Asset&& other) = default;
	Asset& operator=(Asset& other) = delete;
	Asset& operator=(Asset&& other) = default;
	~Asset() {}

	// void print() const;
	// void printVerbose() const;
	
};
