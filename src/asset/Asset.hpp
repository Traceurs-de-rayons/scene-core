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

class Asset 
{

private:
	struct ObjectData {
		std::vector<std::unique_ptr<Mesh>> meshes;
	};

	struct PrimitiveData {
		Primitive primitive;
	};

	struct InstanceData {
		std::string parent_name;
	};

	std::string name_;

	std::variant<ObjectData, PrimitiveData, InstanceData> content_;

	Transform transform_;

	BVH blas_;
	uint32_t blasIndex_; // index du blas sur le buffer gpu, uniquement si tous les submesh sont bien envoye

	friend void sceneIO::parser::parseObj(Asset& asset, const std::string& path);

public:
	Asset() {}
	Asset(Asset& other) = delete;
	Asset(Asset&& other) = default;
	Asset& operator=(Asset& other) = delete;
	Asset& operator=(Asset&& other) = default;
	~Asset() {}

	void print() const;
	void printVerbose() const;
	
};
