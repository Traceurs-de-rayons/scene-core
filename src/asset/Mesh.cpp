#include "Mesh.hpp"
#include <iostream>

void Mesh::print() const
{
	std::cout << "  Mesh: " << name << std::endl;
	std::cout << "    Vertex Count: " << vertices_.size() << std::endl;
	std::cout << "    SubMesh Count: " << subMeshes_.size() << std::endl;
	
	for (size_t i = 0; i < subMeshes_.size(); ++i) {
		subMeshes_[i]->print();
	}
}

void Mesh::printVerbose() const
{
	std::cout << "  Mesh: " << name << std::endl;
	std::cout << "    Vertex Count: " << vertices_.size() << std::endl;
	std::cout << "    SubMesh Count: " << subMeshes_.size() << std::endl;
	
	std::cout << "    Vertices (" << vertices_.size() << "):" << std::endl;
	for (size_t i = 0; i < vertices_.size(); ++i) {
		std::cout << "      [" << i << "]: pos=(" << vertices_[i].pos.x << ", " 
		          << vertices_[i].pos.y << ", " << vertices_[i].pos.z << ") "
		          << "normal=(" << vertices_[i].normal.x << ", " 
		          << vertices_[i].normal.y << ", " << vertices_[i].normal.z << ") "
		          << "uv=(" << vertices_[i].uv.x << ", " << vertices_[i].uv.y << ")" << std::endl;
	}
	
	for (size_t i = 0; i < subMeshes_.size(); ++i) {
		subMeshes_[i]->printVerbose();
	}
}

