#pragma once

#include <stdint.h>
#include <string>

enum TextureType {
	RGBA,
	RGBE
};

struct rgba_t {
	uint8_t r;
	uint8_t g;
	uint8_t b;
	uint8_t a;
};

struct rgbe_t {
	uint8_t r;
	uint8_t g;
	uint8_t b;
	uint8_t e;
};

struct Texture {
	std::string name;
	TextureType type;
	uint32_t width;
	uint32_t height;
	union
	{
		rgba_t *rgba;
		rgbe_t *rgbe;
	};
};
