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

	// Multiplied with the albedo (a texture or a color): the tint a level gives to a surface
	// whose texture holds the detail. Absent = white.
	std::optional<cu::math::vec3> albedo_factor;

	// Multiplied with the metallic, roughness and occlusion above, when those are textures.
	// Absent = 1.
	std::optional<float> roughness_factor;
	std::optional<float> metallic_factor;
	std::optional<float> occlusion_factor;

	// How the alpha of the albedo texture is read: "opaque" (the default), "cutout" (what is
	// below `alpha_cutoff` is not drawn at all) or "blend" (see the render graph).
	std::optional<std::string> alpha_mode;
	std::optional<float> alpha_cutoff;

	std::optional<float> ior;
	// How many metres one tile of the textures covers (the map gives its geometry UVs in
	// metres). Absent = 1: the UVs are used as they are, which is what a model wants.
	std::optional<float> texture_scale;
	std::optional<std::string> normal_map;
	std::optional<float> normal_intensity;
};
