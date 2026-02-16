#pragma once

#include "Vertex.hpp"

#include <stdint.h>
#include <vector>

class Asset;
namespace sceneIO::parser {
	void parseObj(Asset& asset, const std::string& path);
}

class SubMesh {

private:
	std::vector<uint32_t> indices_;

	std::string parsed_mat_name_;
	uint32_t materialId_ = 0;
	
	// si on veut settup du culling coté rasterisation
	// AABB localBounds;

	// Informations relatives au GPU
	// Si isGPUResident est a false, alors les infos ne sont pas sur le GPU
	// Dans le cas contraire, vertexBufferOffset et indexBufferOffset et leurs count associé doivent etre set
	uint32_t vertexBufferOffset_ = UINT32_MAX;
	uint32_t indexBufferOffset_ = UINT32_MAX;
	uint32_t vertexCount_ = 0;
	uint32_t indexCount_ = 0;

	bool isGPUResident_ = false;

	friend void sceneIO::parser::parseObj(Asset& asset, const std::string& path);

public:
	SubMesh(const std::string& mat = "default") : parsed_mat_name_(mat) {}
	~SubMesh() {}

};
