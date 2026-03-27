#pragma once

#include "Vertex.hpp"

#include <stdint.h>
#include <vector>
#include <istream>
#include <string>

class Asset;
class Scene;

namespace sceneIO::parser {
	class ObjErrorCollector;
	void parseObj(Asset& asset, std::istream& in, ObjErrorCollector& errors,
				  uint64_t startLine, uint64_t startColumn);
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
	uint32_t indexBufferOffset_ = UINT32_MAX;
	uint32_t indexCount_ = 0;

	friend void sceneIO::parser::parseObj(Asset& asset, std::istream& in,
										  sceneIO::parser::ObjErrorCollector& errors,
										  uint64_t startLine, uint64_t startColumn);
	friend class Scene;

public:
	SubMesh(const std::string& mat = "default") : parsed_mat_name_(mat) {}
	~SubMesh() {}

	void print() const;
	void printVerbose() const;

};
