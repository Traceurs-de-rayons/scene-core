#include "Object.hpp"

SubObject& Object::addSubObject(const std::string& name)
{
	subObjects_.push_back(name);
	return subObjects_.back();
}

void Object::addMaterialGroup(SubObject& subObj, const std::string& matName, uint32_t indexOffset, uint32_t indexCount)
{
	subObj.materialGroups.push_back({matName, indexOffset, indexCount});
}
