#pragma once

#include "SubMesh.hpp"

#include <stdint.h>
#include <vector>
#include <memory>

class Asset;

class Mesh {

private:
	std::vector<Vertex> vertices_;
	std::vector<std::unique_ptr<SubMesh>> subMeshes_;

	// GPU related variables
	uint32_t vertexBufferOffset_ = UINT32_MAX;
	uint32_t vertexCount_ = 0;

	friend void sceneIO::parser::parseObj(Asset& asset, std::istream& in,
										  sceneIO::parser::ObjErrorCollector& errors,
										  uint64_t startLine, uint64_t startColumn);
	friend class Scene;

public:
	std::string name;

	Mesh() {}
	Mesh(const std::string& n) : name(n) {}
	Mesh(Mesh& other) = delete;
	Mesh(Mesh&& other) = default;

	~Mesh() {}

	void print() const;
	void printVerbose() const;

};
