#pragma once

#include <stdint.h>
#include <string>
#include <variant>
#include <memory>

#include "core-utils.hpp"

struct Camera
{
	std::string name;
	std::string label;

	struct Perspective
	{
		enum class SensorFit
		{
			HORIZONTAL,
			VERTICAL
		};
		struct PhysicalFOV
		{
			SensorFit sensor_fit;
			float focal_length;
			float sensor_width;
			float sensor_height;
		};
		std::variant<float, PhysicalFOV> fov;
		float f_stop;
		int aperture_blades;
		float aperture_rotation;
		float shutter_speed;
	};

	struct Orthographic
	{
		float ortho_scale;
	};

	enum class FisheyeMapping
	{
		EQUIDISTANT,
		EQUISOLID,
		ORTHOGRAPHIC,
		STEREIGRAPHIC
	};

	struct Fisheye
	{
		float fisheye_fov;
		FisheyeMapping mapping;
	};

	enum class PanoramicType
	{
		EQUIRECTANGULAR,
		MERCATOR
	};

	struct Panoramic
	{
		PanoramicType panoramic_type;
	};

	std::variant<Perspective, Orthographic, Fisheye, Panoramic> projection;

	struct LookAt
	{
		cu::math::vec3 lookat;
		cu::math::vec3 up;
	};

	std::variant<LookAt, cu::math::quat> rotation;
	vec3 position;
};