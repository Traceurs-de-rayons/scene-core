#pragma once

#include <stdint.h>
#include <string>
#include <variant>
#include <memory>

#include "core-utils.hpp"

struct Light
{
	std::string label;

	struct Point
	{
		cu::math::vec3 position;
	};

	struct Directional
	{
		cu::math::vec3 direction;
	};

	std::variant<Point, Directional> projection;

	cu::math::vec3 color;
	float intensity;
};
