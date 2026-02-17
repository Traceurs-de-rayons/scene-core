#include "Asset.hpp"
#include <iostream>

void Asset::print() const
{
	std::cout << "Asset: " << name << std::endl;
	std::cout << "  Type: ";
	switch (type_) {
		case AssetObject:
			std::cout << "Object" << std::endl;
			std::cout << "  Mesh Count: " << meshes_.size() << std::endl;
			for (size_t i = 0; i < meshes_.size(); ++i) {
				meshes_[i]->print();
			}
			break;
		case AssetPrimitive:
			std::cout << "Primitive" << std::endl;
			break;
		case AssetInstance:
			std::cout << "Instance (Parent ID: " << parent_asset_id_ << ")" << std::endl;
			break;
	}
	std::cout << "  Transform:" << std::endl;
	std::cout << "    Position: (" << transform_.getTranslation().x << ", " 
	          << transform_.getTranslation().y << ", " 
	          << transform_.getTranslation().z << ")" << std::endl;
}

void Asset::printVerbose() const
{
	std::cout << "Asset: " << name << std::endl;
	std::cout << "  Type: ";
	switch (type_) {
		case AssetObject:
			std::cout << "Object" << std::endl;
			std::cout << "  Mesh Count: " << meshes_.size() << std::endl;
			for (size_t i = 0; i < meshes_.size(); ++i) {
				meshes_[i]->printVerbose();
			}
			break;
		case AssetPrimitive:
			std::cout << "Primitive" << std::endl;
			break;
		case AssetInstance:
			std::cout << "Instance (Parent ID: " << parent_asset_id_ << ")" << std::endl;
			break;
	}
	std::cout << "  Transform:" << std::endl;
	std::cout << "    Position: (" << transform_.getTranslation().x << ", " 
	          << transform_.getTranslation().y << ", " 
	          << transform_.getTranslation().z << ")" << std::endl;
}

