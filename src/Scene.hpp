#pragma once

#include "core-utils.hpp"
#include "BVH.hpp"
#include "Vertex.hpp"
#include "Texture.hpp"
#include "Material.hpp"
#include "asset/Asset.hpp"

#include <vector>
#include <unordered_map>
#include <filesystem>

using namespace cu::math;

namespace sceneIO::tdr { class SceneLoader; }

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
	
	std::unordered_map<std::string, Texture>  textures_;
	std::unordered_map<std::string, Material>  materials_;

	BVH tlas_;

	std::unordered_map<std::string, Asset>  assets_;
	
	friend class sceneIO::tdr::SceneLoader;

public:
	Scene() = default;
	Scene(Scene&&) = default;
	Scene& operator=(Scene&&) = default;
	~Scene() = default;

	void loadAsset(Asset &obj);

};
