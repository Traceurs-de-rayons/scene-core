#pragma once

#include <optional>
#include <variant>
#include <string>

#include "Texture.hpp"
#include "math/vec3.hpp"

struct Environment
{
	struct Skybox
	{
		std::string path;
		TextureState state = TextureState::Declared;
		std::unique_ptr<uint8_t[]> data = nullptr; // faudra surement changer le type en float ou un truc dans le genre
		int width = 0;
		int height = 0;
		float rotation;
	};

	std::variant<Skybox, cu::math::vec3> env;

	Environment() : env(cu::math::vec3(0.7)) {}
};
