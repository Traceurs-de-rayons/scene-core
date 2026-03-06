#pragma once

#include <optional>
#include <variant>
#include <string>

#include "math/vec3.hpp"

struct RenderSettings
{
	uint32_t width;
	uint32_t height;
	std::string camera;
	std::optional<uint32_t> samples;
	uint32_t max_bounces;
	std::string output_file;
};
