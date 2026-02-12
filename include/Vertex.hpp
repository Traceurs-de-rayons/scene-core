#pragma once

#include "core-utils.hpp"

using namespace cu::math;

struct alignas(16) Vertex {
	vec4 pos;
	vec4 normal;
	vec2 uv;
};
