#pragma once

#include "Mesh.hpp"
#include "utils/Transform.hpp"
#include "BVH.hpp"
#include "Primitive.hpp"

#include <stdint.h>
#include <vector>
#include <memory>
#include <optional>
#include <variant>
#include <unordered_map>

enum AssetType {
	AssetObject,
	AssetPrimitive,
	AssetInstance,
	Light,
	Sun,
};

namespace sceneIO::tdr { class SceneLoader; }

class Asset
{

private:
	struct ObjectData
	{
		std::vector<std::unique_ptr<Mesh>> meshes;
	};

	struct PrimitiveData
	{
		struct Plane
		{
			vec3 normal;
		};

		struct Sphere
		{
			float radius;
		};

		struct Cylinder
		{
			float radius;
			float height;
		};

		struct Cone
		{
			float radius;
			float height;
		};

		struct Hyperboloid
		{
			float height;
			float a;
			float b;
			float c;
			float shape;
		};

		std::variant<Plane, Sphere, Cylinder, Cone, Hyperboloid> primitive;
	};

	struct InstanceData
	{
		std::string parent_name;
	};

	struct LightData
	{
		struct Point
		{
			vec3 position;
		};

		struct Directional
		{
			vec3 direction;
		};

		std::string label;
		vec3 color = vec3(1.0f);
		float intensity = 1.0f;
		std::variant<Point, Directional> projection;
	};

	// Infinitely distant directional light (parallel rays, no position).
	struct SunData
	{
		vec3 direction = vec3(0.0f, -1.0f, 0.0f); // direction the light travels
		vec3 color = vec3(1.0f);
		float intensity = 1.0f;
		float angle = 0.53f; // apparent angular diameter in degrees, softens shadows (0 = hard)
	};

	std::string name_;
	std::string label_;

	std::variant<ObjectData, PrimitiveData, InstanceData, LightData, SunData> content_;

	std::string material_;
	Transform transform_;

	BVH blas_;
	uint32_t blasIndex_;

	friend void sceneIO::parser::parseObj(Asset& asset, std::istream& in,
										  sceneIO::parser::ObjErrorCollector& errors,
										  uint64_t startLine, uint64_t startColumn);
	friend class sceneIO::tdr::SceneLoader;
	friend class Scene;

public:
	Asset() {}
	Asset(Asset& other) = delete;
	Asset(Asset&& other) = default;
	Asset& operator=(Asset& other) = delete;
	Asset& operator=(Asset&& other) = default;
	~Asset() {}

	const std::string& getName() const { return name_; }
	const std::string& getLabel() const { return label_; }
	const std::string& getMaterial() const { return material_; }
	void setMaterial(const std::string& material) { material_ = material; }

	Transform& getTransform() { return transform_; }
	const Transform& getTransform() const { return transform_; }

	AssetType getType() const
	{
		if (std::holds_alternative<ObjectData>(content_))
			return AssetObject;
		if (std::holds_alternative<PrimitiveData>(content_))
			return AssetPrimitive;
		if (std::holds_alternative<InstanceData>(content_))
			return AssetInstance;
		if (std::holds_alternative<LightData>(content_))
			return AssetType::Light;
		return AssetType::Sun;
	}

	LightData* getLightData() { return std::get_if<LightData>(&content_); }
	const LightData* getLightData() const { return std::get_if<LightData>(&content_); }
	SunData* getSunData() { return std::get_if<SunData>(&content_); }
	const SunData* getSunData() const { return std::get_if<SunData>(&content_); }

};
