#pragma once

#include "Model.hpp"

#include <vector>

// Posing a Model: sample its clips into a Pose, blend poses, then turn the result into the
// bone matrices a Scene asset is drawn with (Asset::setBoneMatrices()).
//
//     AnimationPlayer player;
//     player.play(model.findClip("Run"));
//     ...
//     player.update(dt);                        // every frame
//     player.pose(model, pose);
//     computeBoneMatrices(model, pose, matrices);
//     asset->setBoneMatrices(matrices);

// Transform of every node of a model, relative to its parent.
struct Pose
{
	std::vector<vec3> translations;
	std::vector<quat> rotations;
	std::vector<vec3> scales;

	// Turns a node around an axis given in model space (the node keeps its position).
	void rotateNode(const Model& model, int node, const vec3& axis, float radians);
};

void restPose(const Model& model, Pose& pose);
// Overwrites the nodes the clip animates; `pose` must already hold a full pose. A looping clip
// wraps the time around its duration, otherwise the last key is held.
void sampleClip(const Model& model, const AnimationClip& clip, float time, bool loop, Pose& pose);
// pose = pose * (1 - weight) + other * weight
void blendPoses(Pose& pose, const Pose& other, float weight);
// One matrix per Model::bones, in model space.
void computeBoneMatrices(const Model& model, const Pose& pose, std::vector<mat4>& matrices);
// Position of a node in model space.
mat4 nodeMatrix(const Model& model, const Pose& pose, int node);

// Plays one clip at a time, with a cross-fade when the clip changes.
class AnimationPlayer
{

private:
	struct Track
	{
		int	  clip = -1;
		float time = 0.0f;
		float speed = 1.0f;
		bool  loop = true;
	};

	Track current_;
	Track previous_;
	float fade_ = 0.0f;		// time left of the cross-fade
	float fadeTime_ = 0.0f;
	mutable Pose scratch_;

public:
	// Starts `clip` (an index into Model::clips) unless it is already the one playing.
	// `restart` plays it from the start even then.
	void play(int clip, float fadeTime = 0.15f, bool loop = true, bool restart = false);
	void setSpeed(float speed) { current_.speed = speed; }
	void update(float dt);
	void pose(const Model& model, Pose& out) const;

	int	  clip() const { return current_.clip; }
	float time() const { return current_.time; }
	// True once a clip that does not loop has reached its end.
	bool finished(const Model& model) const;

};
