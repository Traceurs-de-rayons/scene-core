#pragma once

#include "Mesh.hpp"
#include "utils/Transform.hpp"
#include "BVH.hpp"
#include "Primitive.hpp"

#include <stdint.h>
#include <vector>
#include <memory>

enum AssetType {
	AssetObject,
	AssetPrimitive,
	AssetInstance,
	// AssetVolume
};

class Asset {

private:
	AssetType type_;
	std::vector<std::unique_ptr<Mesh>> meshes_; // Si AssetObject
	std::unique_ptr<Primitive> primitive_; // Si AssetPrimitive
	uint32_t parent_asset_id_;  // Si AssetInstance

	Transform transform_;

	BVH blas_;
	uint32_t blasIndex_; // index du blas sur le buffer gpu, uniquement si tous les submesh sont bien envoye

	friend void sceneIO::parser::parseObj(Asset& asset, const std::string& path);

public:
	std::string name;

	Asset() {}
	Asset(Asset& other) = delete;
	Asset(Asset&& other) = default;
	Asset& operator=(Asset& other) = delete;
	Asset& operator=(Asset&& other) = default;
	~Asset() {}

	void print() const;
	void printVerbose() const;
	
};
