#pragma once

#include "Vertex.hpp"

#include <vector>
#include <stdint.h>
#include <filesystem>

struct MaterialGroup {
	std::string materialName;
	uint32_t indexOffset;
	uint32_t indexCount;
};

struct SubObject {
	std::string name;
	std::vector<MaterialGroup> materialGroups;

	SubObject(const std::string& n) : name(n) {}
};

class Object {
private:
	std::filesystem::path path_;
	std::vector<Vertex> vertices_;
	std::vector<uint32_t> indices_;
	std::vector<SubObject> subObjects_;
	
public:	
	Object() {}
	~Object() {}

	void addVertex(const Vertex& v) { vertices_.push_back(v); }
	void addIndex(uint32_t idx) { indices_.push_back(idx); }

	SubObject& addSubObject(const std::string& name) { subObjects_.push_back(name); return subObjects_.back(); }
	void addMaterialGroup(SubObject& subObj, const std::string& matName, uint32_t indexOffset, uint32_t indexCount);

	const std::filesystem::path& getPath() const { return path_; }
	const std::vector<Vertex>& getVertices() const { return vertices_; }
	const std::vector<uint32_t>& getIndices() const { return indices_; }
	const std::vector<SubObject>& getSubObjects() const { return subObjects_; }
	
	size_t getTriangleCount() const { return indices_.size() / 3; }
	size_t getSubObjectCount() const { return subObjects_.size(); }
};

