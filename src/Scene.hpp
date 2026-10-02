#pragma once

#include "BVH.hpp"
#include "Texture.hpp"
#include "Material.hpp"
#include "Camera.hpp"
#include "Render.hpp"
#include "Environment.hpp"
#include "asset/Asset.hpp"

#include <unordered_map>
#include <functional>
#include <vector>

using namespace cu::math;

struct DrawCallView
{
	uint32_t         verticesIndex;
	uint32_t         verticesCount;
	uint32_t         indicesIndex;
	uint32_t         indecesCount;
	const Material&  material;
	const Transform& transform;
};

struct SceneObject
{
	std::string 	name;
	void*       	data;
	AssetType		type;
	int				parent_index_;
};

class Scene
{

private:
	std::vector<SceneObject> scene_hierarchy_;

	std::unordered_map<std::string, Texture> textures_;
	std::unordered_map<std::string, Material> materials_;
	std::unordered_map<std::string, Asset> assets_;
	std::unordered_map<std::string, Camera> cameras_;

	Environment environment_;

	std::optional<RenderSettings> render_settings_;

	BVH tlas_;


	Material default_material_ = {};

	friend class sceneIO::tdr::SceneLoader;

public:
	Scene() = default;
	Scene(Scene&&) = default;
	Scene& operator=(Scene&&) = default;
	~Scene() = default;

	uint32_t getVertexAmount();
	uint32_t getIndexAmount();
	void loadVertices(Vertex *buffer);
	void loadVerticesSSBO(VertexSSBO *buffer);
	void loadIndices(uint32_t *buffer);
	void loadHierarchy();

	void forEachSubMesh(std::function<void(const DrawCallView&)> callback) const;


	const std::unordered_map<std::string, Camera>& getCameras() const { return cameras_; }
	const std::unordered_map<std::string, Texture>& getTextures() const { return textures_; }
	const std::unordered_map<std::string, Material>& getMaterials() const { return materials_; }
	const std::unordered_map<std::string, Asset>& getAssets() const { return assets_; }
	std::unordered_map<std::string, Asset>& getAssets() { return assets_; }
	const std::vector<SceneObject>& getSceneHierarchy() const { return scene_hierarchy_; }
	const Camera* getCamera(const std::string& name) const
	{
		auto it = cameras_.find(name);
		return it != cameras_.end() ? &it->second : nullptr;
	}
	const Camera* getDefaultCamera() const
	{
		return cameras_.empty() ? nullptr : &cameras_.begin()->second;
	}

};
