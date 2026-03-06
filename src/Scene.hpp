#pragma once

#include "core-utils.hpp"
#include "BVH.hpp"
#include "Vertex.hpp"
#include "Texture.hpp"
#include "Material.hpp"
#include "Camera.hpp"
#include "asset/Asset.hpp"

#include <vector>
#include <unordered_map>
#include <filesystem>

using namespace cu::math;

class Scene
{

private:
	// GPU buffers :
	// vertex
	// indexes
	// material
	// primitives
	// tlas
	// blas (selon la structure blas et tlas peuvent etre dans le meme buffer)
	
	std::unordered_map<std::string, Texture> textures_;
	std::unordered_map<std::string, Material> materials_;
	std::unordered_map<std::string, Asset> assets_;

	std::unordered_map<std::string, Camera> cameras_;

	BVH tlas_;

	friend class sceneIO::tdr::SceneLoader;

public:
	Scene() = default;
	Scene(Scene&&) = default;
	Scene& operator=(Scene&&) = default;
	~Scene() = default;

	void loadAsset(Asset &obj);

};
