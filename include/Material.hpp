#pragma once

#include <optional>
#include <variant>
#include <string>

#include "math/vec3.hpp"

template<typename T>
struct MaterialParam
{
	std::variant<T, std::string> value;
};

struct Material
{
	std::string name;
	std::string label;

	std::optional<MaterialParam<cu::math::vec3>> albedo;
	std::optional<MaterialParam<float>> metallic;
	std::optional<MaterialParam<float>> roughness;
	std::optional<MaterialParam<float>> transmission;
	std::optional<MaterialParam<float>> ambient_occlusion;
	std::optional<MaterialParam<float>> emission_strength;
	std::optional<MaterialParam<cu::math::vec3>> emission_color;

	std::optional<float> ior;
	std::optional<float> texture_scale;
	std::optional<std::string> normal_map;
	std::optional<float> normal_intensity;
};
