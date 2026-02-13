#pragma once

#include <stdint.h>

enum PrimitiveType {
	PrimSphere,
	PrimPlane,
	PrimCylinder,
	PrimCone,
	PrimHyperboloid,
};

struct Primitive {
	PrimitiveType type;
	float radius;
	float height;
	float a;
	float b;
	float c;
	float d;
};
