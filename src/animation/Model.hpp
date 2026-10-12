#pragma once

#include "Material.hpp"
#include "Vertex.hpp"

#include <cstdint>
#include <string>
#include <vector>

// A model that can be animated: meshes bound to a hierarchy of nodes, and the clips moving
// those nodes. It is only data (see sceneIO::loadGltf() to read one from a file); a Scene
// draws it through Scene::addModel(), and Animation.hpp poses it.

// Up to 4 bones per vertex, as indices into Model::bones. The weights add up to 255; a vertex
// whose weights are all 0 is not skinned.
struct VertexSkin
{
	uint8_t bones[4] = {0, 0, 0, 0};
	uint8_t weights[4] = {0, 0, 0, 0};
};

struct ModelNode
{
	std::string name;
	int parent = -1; // index into Model::nodes, always lower than the node's own index
	// Rest pose, relative to the parent.
	vec3 translation{0.0f};
	quat rotation;
	vec3 scale{1.0f};
};

// What a vertex can be attached to: a node, and the matrix bringing the vertex from model
// space into the space of that node at bind time.
struct ModelBone
{
	int	 node = 0;
	mat4 inverseBind = mat4::identity();
};

struct AnimationChannel
{
	enum class Path { Translation, Rotation, Scale };

	int	 node = 0;
	Path path = Path::Translation;
	bool step = false; // hold each key instead of interpolating to the next
	std::vector<float> times; // seconds, increasing
	std::vector<vec4> values; // one per time: xyz, or a quaternion as (x, y, z, w)
};

struct AnimationClip
{
	std::string name;
	float duration = 0.0f; // seconds
	std::vector<AnimationChannel> channels;
};

struct ModelMesh
{
	std::string name;
	std::string material; // name of one of Model::materials
	std::vector<Vertex> vertices;
	std::vector<VertexSkin> skin; // one per vertex
	std::vector<uint32_t> indices; // triangles
};

struct ModelTexture
{
	std::string name;
	std::string path; // image file, when `encoded` is empty
	std::vector<uint8_t> encoded; // image file kept in memory (PNG, JPEG...)
};

struct Model
{
	std::string name;
	std::vector<ModelNode> nodes;
	std::vector<ModelBone> bones; // at most 256
	std::vector<AnimationClip> clips;
	std::vector<ModelMesh> meshes;
	std::vector<Material> materials; // an albedo given as a string names one of `textures`
	std::vector<ModelTexture> textures;

	// -1 when there is none.
	int findNode(const std::string& nodeName) const;
	int findClip(const std::string& clipName) const;
	// First clip whose name contains `text`, ignoring case; -1 when there is none.
	int findClipContaining(const std::string& text) const;

	// Adds the meshes of `part` (with its materials and textures) as one rigid piece following
	// `node`: a helmet on the head, a weapon in a hand. `placement` puts the part, taken in its
	// rest pose, where it sits while this model is in its bind pose (model space).
	// Returns the bone the piece hangs from, or -1 when it cannot be added: giving that bone a
	// null matrix in a pose hides the piece.
	int attach(const Model& part, int node, const mat4& placement);

	// Bounding box of the meshes in the rest pose, or in the given pose.
	void bounds(vec3& min, vec3& max) const;
	void bounds(vec3& min, vec3& max, const struct Pose& pose) const;
};
