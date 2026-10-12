
#include "Scene.hpp"
#include <string.h>
#include <cmath>

namespace {

	// Transform::getMatrix() is m[row][col] with the translation in the last column.
	vec3 transformPoint(const mat4& m, const vec3& p)
	{
		return {m.m[0][0] * p.x + m.m[0][1] * p.y + m.m[0][2] * p.z + m.m[0][3],
				m.m[1][0] * p.x + m.m[1][1] * p.y + m.m[1][2] * p.z + m.m[1][3],
				m.m[2][0] * p.x + m.m[2][1] * p.y + m.m[2][2] * p.z + m.m[2][3]};
	}

	// Moller-Trumbore; `t` is the distance along the normalized `direction`.
	bool rayTriangle(const vec3& origin, const vec3& direction, const vec3& a, const vec3& b, const vec3& c, float& t)
	{
		const vec3  edge1 = b - a;
		const vec3  edge2 = c - a;
		const vec3  p     = vec3::cross(direction, edge2);
		const float det   = vec3::dot(edge1, p);
		if (std::abs(det) < 1e-12f)
			return false;

		const float inv = 1.0f / det;
		const vec3  s   = origin - a;
		const float u   = vec3::dot(s, p) * inv;
		if (u < 0.0f || u > 1.0f)
			return false;

		const vec3  q = vec3::cross(s, edge1);
		const float v = vec3::dot(direction, q) * inv;
		if (v < 0.0f || u + v > 1.0f)
			return false;

		t = vec3::dot(edge2, q) * inv;
		return t > 1e-4f;
	}

}

uint32_t Scene::getVertexAmount()
{
	uint32_t res = 0;

	for (const auto& [key, asset] : assets_)
	{
		if (!std::holds_alternative<Asset::ObjectData>(asset.content_)) continue;

		for (const auto& mesh : std::get<Asset::ObjectData>(asset.content_).meshes)
			res += mesh->vertices_.size();
	}
	return res;
}

uint32_t Scene::getIndexAmount()
{
	uint32_t res = 0;

	for (const auto& [key, asset] : assets_)
	{
		if (!std::holds_alternative<Asset::ObjectData>(asset.content_)) continue;

		for (const auto& mesh : std::get<Asset::ObjectData>(asset.content_).meshes)
		{
			for (const auto& submesh : mesh->subMeshes_)
				res += submesh->indices_.size();
		}
	}
	return res;
}

void Scene::loadVertices(Vertex *buffer)
{
	uint32_t pos = 0;

	for (const auto& [key, asset] : assets_)
	{
		if (!std::holds_alternative<Asset::ObjectData>(asset.content_)) continue;

		for (const auto& mesh : std::get<Asset::ObjectData>(asset.content_).meshes)
		{
			const auto& verts = mesh->vertices_;
			memcpy(buffer + pos, verts.data(), verts.size() * sizeof(Vertex));
			mesh->vertexCount_ = mesh->vertices_.size();
			mesh->vertexBufferOffset_ = pos;
			pos += mesh->vertices_.size();
		}
	}
}

void Scene::loadVertexSkins(VertexSkin *buffer)
{
	uint32_t pos = 0;

	for (const auto& [key, asset] : assets_)
	{
		if (!std::holds_alternative<Asset::ObjectData>(asset.content_)) continue;

		for (const auto& mesh : std::get<Asset::ObjectData>(asset.content_).meshes)
		{
			const size_t count = mesh->vertices_.size();
			if (mesh->skin_.size() == count)
				memcpy(buffer + pos, mesh->skin_.data(), count * sizeof(VertexSkin));
			else
				memset(buffer + pos, 0, count * sizeof(VertexSkin));
			pos += count;
		}
	}
}

void Scene::loadVerticesSSBO(VertexSSBO *buffer)
{

}

void Scene::loadIndices(uint32_t *buffer)
{
	uint32_t pos = 0;

	for (const auto& [key, asset] : assets_)
	{
		if (!std::holds_alternative<Asset::ObjectData>(asset.content_)) continue;

		for (const auto& mesh : std::get<Asset::ObjectData>(asset.content_).meshes)
		{
			for (const auto& submesh : mesh->subMeshes_)
			{
				const auto& indices = submesh->indices_;
				memcpy(buffer + pos, indices.data(), indices.size() * sizeof(uint32_t));
				submesh->indicesCount_ = submesh->indices_.size();
				submesh->indicesBufferOffset_ = pos;
				pos += submesh->indices_.size();
			}
		}
	}
}

void Scene::forEachSubMesh(std::function<void(const DrawCallView&)> callback) const
{
	for (const auto& [key, asset] : assets_)
	{
		if (!std::holds_alternative<Asset::ObjectData>(asset.content_)) continue;

		const auto& objectData = std::get<Asset::ObjectData>(asset.content_);

		for (const auto& mesh : objectData.meshes)
		{
			for (const auto& submesh : mesh->subMeshes_)
			{
				auto matIt = materials_.end();
				if (const auto replaced = asset.materialOverrides_.find(submesh->parsed_mat_name_); replaced != asset.materialOverrides_.end())
					matIt = materials_.find(replaced->second);
				if (matIt == materials_.end())
					matIt = materials_.find(submesh->parsed_mat_name_);
				const Material *mat = nullptr;
				if (matIt == materials_.end())
					mat = &default_material_;
				else
					mat = &(matIt->second);

				DrawCallView view {
					.verticesIndex = mesh->vertexBufferOffset_,
					.verticesCount = mesh->vertexCount_,
					.indicesIndex = submesh->indicesBufferOffset_,
					.indecesCount = submesh->indicesCount_,
					.material = *mat,
					.transform = asset.transform_,
					.visible = asset.visible_,
					.bones = asset.bones_.empty() ? nullptr : &asset.bones_,
					.viewModel = asset.viewModel_,
				};
				callback(view);
			}
		}
	}
}

Asset* Scene::addAsset(const std::string& name, Asset&& asset)
{
	if (name.empty() || assets_.find(name) != assets_.end())
		return nullptr;

	Asset& added = assets_[name] = std::move(asset);
	added.name_ = name;

	++structure_version_;
	loadHierarchy();
	return &added;
}

Asset* Scene::addMesh(const std::string& name, std::vector<Vertex> vertices, std::vector<uint32_t> indices,
					  const std::string& material)
{
	if (vertices.empty() || indices.empty() || indices.size() % 3 != 0)
		return nullptr;

	for (uint32_t index : indices)
	{
		if (index >= vertices.size())
			return nullptr;
	}

	auto submesh = std::make_unique<SubMesh>(material);
	submesh->indices_ = std::move(indices);

	auto mesh = std::make_unique<Mesh>(name);
	mesh->vertices_ = std::move(vertices);
	mesh->subMeshes_.push_back(std::move(submesh));

	Asset::ObjectData object;
	object.meshes.push_back(std::move(mesh));

	Asset asset;
	asset.content_ = std::move(object);
	asset.material_ = material;
	return addAsset(name, std::move(asset));
}

Asset* Scene::addModel(const std::string& name, const Model& model)
{
	if (model.meshes.empty() || model.bones.size() > 256)
		return nullptr;

	// Materials and textures are shared by every instance of the model.
	const std::string prefix = model.name + ":";
	for (const ModelTexture& source : model.textures)
	{
		Texture texture;
		texture.name = prefix + source.name;
		if (source.encoded.empty())
			texture.data = Texture::FromFile{ .path = source.path };
		else
			texture.data = Texture::Encoded{ source.encoded };
		addTexture(texture.name, std::move(texture));
	}
	for (Material material : model.materials)
	{
		material.name = prefix + material.name;
		if (material.albedo && std::holds_alternative<std::string>(material.albedo->value))
			material.albedo->value = prefix + std::get<std::string>(material.albedo->value);
		addMaterial(material);
	}

	Asset::ObjectData object;
	for (const ModelMesh& source : model.meshes)
	{
		if (source.vertices.empty() || source.indices.empty() || source.skin.size() != source.vertices.size())
			continue;

		auto submesh = std::make_unique<SubMesh>(prefix + source.material);
		submesh->indices_ = source.indices;

		auto mesh = std::make_unique<Mesh>(source.name);
		mesh->vertices_ = source.vertices;
		mesh->skin_ = source.skin;
		mesh->subMeshes_.push_back(std::move(submesh));
		object.meshes.push_back(std::move(mesh));
	}
	if (object.meshes.empty())
		return nullptr;

	Asset asset;
	asset.content_ = std::move(object);
	Pose pose;
	restPose(model, pose);
	computeBoneMatrices(model, pose, asset.bones_);
	return addAsset(name, std::move(asset));
}

Texture* Scene::addTexture(const std::string& name, Texture&& texture)
{
	if (name.empty() || textures_.find(name) != textures_.end())
		return nullptr;

	// Texture itself cannot be moved: fill a new one in place.
	Texture& added = textures_[name];
	added.name = texture.name;
	added.label = texture.label;
	added.data = std::move(texture.data);
	++structure_version_;
	return &added;
}

Material* Scene::addMaterial(const Material& material)
{
	if (material.name.empty() || materials_.find(material.name) != materials_.end())
		return nullptr;

	Material& added = materials_[material.name] = material;
	++structure_version_;
	return &added;
}

Material* Scene::addColorMaterial(const std::string& name, const vec3& color, float emission)
{
	Material material;
	material.name = name;
	material.albedo = MaterialParam<vec3>{ color };
	if (emission > 0.0f)
		material.emission_strength = MaterialParam<float>{ emission };
	return addMaterial(material);
}

Material* Scene::getMaterial(const std::string& name)
{
	auto it = materials_.find(name);
	return it != materials_.end() ? &it->second : nullptr;
}

Asset* Scene::addSun(const std::string& name, const vec3& direction, const vec3& color, float intensity, float angle)
{
	Asset::SunData sun;
	sun.direction = direction;
	sun.color = color;
	sun.intensity = intensity;
	sun.angle = angle;

	Asset asset;
	asset.content_ = sun;
	return addAsset(name, std::move(asset));
}

Asset* Scene::addDirectionalLight(const std::string& name, const vec3& direction, const vec3& color, float intensity)
{
	Asset::LightData light;
	light.color = color;
	light.intensity = intensity;
	light.projection = Asset::LightData::Directional{ direction };

	Asset asset;
	asset.content_ = light;
	return addAsset(name, std::move(asset));
}

Asset* Scene::addPointLight(const std::string& name, const vec3& position, const vec3& color, float intensity)
{
	Asset::LightData light;
	light.color = color;
	light.intensity = intensity;
	light.projection = Asset::LightData::Point{ position };

	Asset asset;
	asset.content_ = light;
	return addAsset(name, std::move(asset));
}

Asset* Scene::addSpotLight(const std::string& name, const vec3& position, const vec3& direction, const vec3& color,
						   float intensity, float angle, float blend)
{
	Asset::LightData light;
	light.color = color;
	light.intensity = intensity;
	light.projection = Asset::LightData::Spot{ position, direction, angle, blend };

	Asset asset;
	asset.content_ = light;
	return addAsset(name, std::move(asset));
}

bool Scene::removeAsset(const std::string& name)
{
	if (assets_.erase(name) == 0)
		return false;

	++structure_version_;
	loadHierarchy();
	return true;
}

Asset* Scene::getAsset(const std::string& name)
{
	auto it = assets_.find(name);
	return it != assets_.end() ? &it->second : nullptr;
}

const Asset* Scene::getAsset(const std::string& name) const
{
	auto it = assets_.find(name);
	return it != assets_.end() ? &it->second : nullptr;
}

bool Scene::setAssetVisible(const std::string& name, bool visible)
{
	Asset* asset = getAsset(name);
	if (!asset)
		return false;

	asset->visible_ = visible;
	return true;
}

bool Scene::isAssetVisible(const std::string& name) const
{
	const Asset* asset = getAsset(name);
	return asset && asset->visible_;
}

std::optional<RayHit> Scene::raycast(const vec3& origin, const vec3& direction) const
{
	const vec3 dir = vec3::normalize(direction);
	std::optional<RayHit> closest;
	std::vector<vec3> world;

	for (const auto& [key, asset] : assets_)
	{
		if (!asset.visible_ || !std::holds_alternative<Asset::ObjectData>(asset.content_)) continue;

		const mat4& matrix = asset.transform_.getMatrix();

		for (const auto& mesh : std::get<Asset::ObjectData>(asset.content_).meshes)
		{
			world.clear();
			world.reserve(mesh->vertices_.size());
			for (const Vertex& vertex : mesh->vertices_)
				world.push_back(transformPoint(matrix, vertex.pos));

			for (const auto& submesh : mesh->subMeshes_)
			{
				const auto& indices = submesh->indices_;
				for (size_t i = 0; i + 2 < indices.size(); i += 3)
				{
					float t = 0.0f;
					if (!rayTriangle(origin, dir, world[indices[i]], world[indices[i + 1]], world[indices[i + 2]], t))
						continue;
					if (!closest || t < closest->distance)
						closest = RayHit{key, t, origin + dir * t};
				}
			}
		}
	}
	return closest;
}

void Scene::loadHierarchy()
{
	scene_hierarchy_.clear();

	for (auto& [key, asset] : assets_)
	{
		AssetType type;
		if (std::holds_alternative<Asset::ObjectData>(asset.content_))
			type = AssetObject;
		else if (std::holds_alternative<Asset::PrimitiveData>(asset.content_))
			type = AssetPrimitive;
		else if (std::holds_alternative<Asset::InstanceData>(asset.content_))
			type = AssetInstance;
		else if (std::holds_alternative<Asset::LightData>(asset.content_))
			type = AssetType::Light;
		else
			type = AssetType::Sun;

		scene_hierarchy_.push_back(SceneObject{
			.name = key,
			.data = &asset,
			.type = type,
			.parent_index_ = 0,
		});
	}
}
