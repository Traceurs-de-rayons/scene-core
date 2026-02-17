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

	friend void sceneIO::parser::parseObj(Asset& asset, const std::string& path);

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