#pragma once

#include "core-utils.hpp"

using namespace cu::math;

struct alignas(16) Vertex {
	vec3 pos;
	float pad1_ = 0;
	vec3 normal;
	float pad2_ = 0;
	vec2 uv;
};
