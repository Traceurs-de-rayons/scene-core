#include "SubMesh.hpp"
#include <iostream>

void SubMesh::print() const
{
	std::cout << "    SubMesh:" << std::endl;
	std::cout << "      Material: " << parsed_mat_name_ << std::endl;
	std::cout << "      Material ID: " << materialId_ << std::endl;
	std::cout << "      Index Count: " << indices_.size() << std::endl;
	std::cout << "      GPU Resident: " << (isGPUResident_ ? "Yes" : "No") << std::endl;
	if (isGPUResident_) {
		std::cout << "      Vertex Buffer Offset: " << vertexBufferOffset_ << std::endl;
		std::cout << "      Index Buffer Offset: " << indexBufferOffset_ << std::endl;
	}
}

void SubMesh::printVerbose() const
{
	std::cout << "    SubMesh:" << std::endl;
	std::cout << "      Material: " << parsed_mat_name_ << std::endl;
	std::cout << "      Material ID: " << materialId_ << std::endl;
	std::cout << "      Index Count: " << indices_.size() << std::endl;
	std::cout << "      GPU Resident: " << (isGPUResident_ ? "Yes" : "No") << std::endl;
	if (isGPUResident_) {
		std::cout << "      Vertex Buffer Offset: " << vertexBufferOffset_ << std::endl;
		std::cout << "      Index Buffer Offset: " << indexBufferOffset_ << std::endl;
	}
	
	std::cout << "      Indices (" << indices_.size() << "):" << std::endl;
	for (size_t i = 0; i < indices_.size(); ++i) {
		std::cout << "        [" << i << "]: " << indices_[i] << std::endl;
	}
}

