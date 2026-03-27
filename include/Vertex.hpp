#pragma once

#include "core-utils.hpp"

using namespace cu::math;

struct alignas(16) Vertex {
	vec3 pos;
	vec3 normal;
	vec2 uv;
};

struct alignas(16) VertexSSBO {
	vec3 pos;
	float pad1_ = 0;
	vec3 normal;
	float pad2_ = 0;
	vec2 uv;
	float pad3_ = 0;
	float pad4_ = 0;
};
