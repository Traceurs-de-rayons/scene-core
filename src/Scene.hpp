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
	bool             visible = true;
};

// Closest intersection found by Scene::raycast().
struct RayHit
{
	std::string name;     // asset that was hit
	float       distance; // world-space distance from the ray origin
	vec3        position; // world-space hit point
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

	// Bumped whenever an asset is added or removed: GPU buffers built from an older version are stale.
	uint64_t structure_version_ = 0;

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

	// Adds `asset` under `name`. Returns nullptr if the name is empty or already taken.
	Asset* addAsset(const std::string& name, Asset&& asset);
	// Adds an object made of one mesh: `indices` are triangles into `vertices`.
	Asset* addMesh(const std::string& name, std::vector<Vertex> vertices, std::vector<uint32_t> indices,
				   const std::string& material = "default");
	bool removeAsset(const std::string& name);

	Asset* getAsset(const std::string& name);
	const Asset* getAsset(const std::string& name) const;
	bool hasAsset(const std::string& name) const { return assets_.find(name) != assets_.end(); }

	// Return false if no asset has that name.
	bool setAssetVisible(const std::string& name, bool visible);
	bool hideAsset(const std::string& name) { return setAssetVisible(name, false); }
	bool showAsset(const std::string& name) { return setAssetVisible(name, true); }
	bool isAssetVisible(const std::string& name) const;

	uint64_t getStructureVersion() const { return structure_version_; }

	// Closest visible object hit by the ray, tested triangle by triangle in world space.
	std::optional<RayHit> raycast(const vec3& origin, const vec3& direction) const;

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
