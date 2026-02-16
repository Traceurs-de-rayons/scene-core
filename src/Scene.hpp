#pragma once

#include "core-utils.hpp"
#include "BVH.hpp"
#include "Vertex.hpp"
#include "Texture.hpp"
#include "asset/Asset.hpp"

#include <vector>
#include <filesystem>

using namespace cu::math;


struct Material {
	
};

class Scene {

private:
	// GPU buffers :
	// vertex
	// indexes
	// material
	// primitives
	// tlas
	// blas (selon la structure blas et tlas peuvent etre dans le meme buffer)
	
	std::vector<Material> materials_;

	BVH tlas_;

	std::vector<Asset> assets_;
	std::vector<Texture> textures_;
	

public:
	Scene() {}
	~Scene() {}

	void loadAsset(Asset &obj);

};
