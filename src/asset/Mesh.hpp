#pragma once

#include "SubMesh.hpp"

#include <stdint.h>
#include <vector>
#include <memory>

class Mesh {

private:
	std::vector<std::unique_ptr<SubMesh>> subMeshes_;

public:
	std::string name;

	Mesh() {}
	~Mesh() {}

};