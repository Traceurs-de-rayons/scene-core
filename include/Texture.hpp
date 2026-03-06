#pragma once

#include <stdint.h>
#include <string>
#include <variant>
#include <memory>

#include "core-utils.hpp"

enum class TextureState
{
	Declared,
	Loaded,
	Uploaded,
};

struct Texture
{
	std::string name = "";
	std::string label = "";

	struct FromFile
	{
		std::string path = "";
		TextureState state = TextureState::Declared;
		std::unique_ptr<uint8_t[]> data = nullptr;
		int width = 0;
		int height = 0;
		int channels = 0;
	};

	struct CheckerLocal
	{
		cu::math::vec3 even = cu::math::vec3();
		cu::math::vec3 odd = cu::math::vec3();
		float scale = 1.0f;
	};

	struct CheckerGlobal
	{
		cu::math::vec3 even = cu::math::vec3();
		cu::math::vec3 odd = cu::math::vec3();
		float scale = 1.0f;
	};

	std::variant<FromFile, CheckerLocal, CheckerGlobal> data;

	Texture() : data(FromFile{}) {}
	~Texture() = default;
};