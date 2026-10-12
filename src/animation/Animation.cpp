#include "Animation.hpp"

#include <algorithm>
#include <cctype>
#include <cmath>

namespace {

	// Translation, rotation and scale as one matrix (m[row][col], translation in the last column).
	mat4 compose(const vec3& translation, const quat& rotation, const vec3& scale)
	{
		mat4 m = rotation.toMatrix();
		const float s[3] = {scale.x, scale.y, scale.z};
		const float t[3] = {translation.x, translation.y, translation.z};
		for (int row = 0; row < 3; ++row)
		{
			for (int col = 0; col < 3; ++col)
				m.m[row][col] *= s[col];
			m.m[row][3] = t[row];
		}
		return m;
	}

	// Matrix of every node in model space; parents come before their children.
	void globalMatrices(const Model& model, const Pose& pose, std::vector<mat4>& out)
	{
		out.resize(model.nodes.size());
		for (size_t i = 0; i < model.nodes.size(); ++i)
		{
			const mat4 local = compose(pose.translations[i], pose.rotations[i], pose.scales[i]);
			const int parent = model.nodes[i].parent;
			out[i] = parent >= 0 ? out[static_cast<size_t>(parent)] * local : local;
		}
	}

	quat toQuat(const vec4& v) { return quat(v.w, v.x, v.y, v.z); }

	std::string lowered(const std::string& text)
	{
		std::string out = text;
		std::transform(out.begin(), out.end(), out.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
		return out;
	}

	vec3 transformPoint(const mat4& m, const vec3& p)
	{
		return {m.m[0][0] * p.x + m.m[0][1] * p.y + m.m[0][2] * p.z + m.m[0][3],
				m.m[1][0] * p.x + m.m[1][1] * p.y + m.m[1][2] * p.z + m.m[1][3],
				m.m[2][0] * p.x + m.m[2][1] * p.y + m.m[2][2] * p.z + m.m[2][3]};
	}

}

int Model::findNode(const std::string& nodeName) const
{
	for (size_t i = 0; i < nodes.size(); ++i)
		if (nodes[i].name == nodeName)
			return static_cast<int>(i);
	return -1;
}

int Model::findClip(const std::string& clipName) const
{
	for (size_t i = 0; i < clips.size(); ++i)
		if (clips[i].name == clipName)
			return static_cast<int>(i);
	return -1;
}

int Model::findClipContaining(const std::string& text) const
{
	const std::string wanted = lowered(text);
	for (size_t i = 0; i < clips.size(); ++i)
		if (lowered(clips[i].name).find(wanted) != std::string::npos)
			return static_cast<int>(i);
	return -1;
}

int Model::attach(const Model& part, int node, const mat4& placement)
{
	if (node < 0 || static_cast<size_t>(node) >= nodes.size() || bones.size() >= 256 || part.meshes.empty())
		return -1;

	// The piece follows `node` from where the bind pose puts that node: the inverse bind matrix
	// of a bone already on the node gives it. A node no vertex is bound to has none; the piece
	// is then placed relative to the node itself.
	mat4 nodeInverseBind = mat4::identity();
	for (const ModelBone& bone : bones)
	{
		if (bone.node == node)
		{
			nodeInverseBind = bone.inverseBind;
			break;
		}
	}
	const int boneIndex = static_cast<int>(bones.size());
	bones.push_back(ModelBone{node, nodeInverseBind * placement});

	const std::string prefix = part.name + ".";
	for (ModelTexture texture : part.textures)
	{
		texture.name = prefix + texture.name;
		textures.push_back(std::move(texture));
	}
	for (Material material : part.materials)
	{
		material.name = prefix + material.name;
		if (material.albedo && std::holds_alternative<std::string>(material.albedo->value))
			material.albedo->value = prefix + std::get<std::string>(material.albedo->value);
		materials.push_back(std::move(material));
	}

	// Flattened: the part loses its own skeleton and becomes vertices bound to the one bone.
	Pose pose;
	restPose(part, pose);
	std::vector<mat4> partBones;
	computeBoneMatrices(part, pose, partBones);

	for (ModelMesh mesh : part.meshes)
	{
		mesh.name = prefix + mesh.name;
		mesh.material = prefix + mesh.material;
		for (size_t i = 0; i < mesh.vertices.size(); ++i)
		{
			VertexSkin& skin = mesh.skin[i];
			const int heaviest = static_cast<int>(std::max_element(skin.weights, skin.weights + 4) - skin.weights);
			if (skin.weights[heaviest] > 0)
			{
				const mat4& matrix = partBones[skin.bones[heaviest]];
				mesh.vertices[i].pos = transformPoint(matrix, mesh.vertices[i].pos);
				const vec3 n = mesh.vertices[i].normal;
				const vec3 turned = {matrix.m[0][0] * n.x + matrix.m[0][1] * n.y + matrix.m[0][2] * n.z,
									 matrix.m[1][0] * n.x + matrix.m[1][1] * n.y + matrix.m[1][2] * n.z,
									 matrix.m[2][0] * n.x + matrix.m[2][1] * n.y + matrix.m[2][2] * n.z};
				mesh.vertices[i].normal = turned.length() > 0.0f ? vec3::normalize(turned) : n;
			}
			skin = VertexSkin{};
			skin.bones[0] = static_cast<uint8_t>(boneIndex);
			skin.weights[0] = 255;
		}
		meshes.push_back(std::move(mesh));
	}
	return boneIndex;
}

void Model::bounds(vec3& min, vec3& max) const
{
	Pose pose;
	restPose(*this, pose);
	bounds(min, max, pose);
}

void Model::bounds(vec3& min, vec3& max, const Pose& pose) const
{
	std::vector<mat4> matrices;
	computeBoneMatrices(*this, pose, matrices);

	min = vec3(1e30f);
	max = vec3(-1e30f);
	for (const ModelMesh& mesh : meshes)
	{
		for (size_t i = 0; i < mesh.vertices.size(); ++i)
		{
			vec3 position = mesh.vertices[i].pos;
			const VertexSkin& skin = mesh.skin[i];
			// The heaviest bone is enough for a bounding box.
			const int heaviest = static_cast<int>(std::max_element(skin.weights, skin.weights + 4) - skin.weights);
			if (skin.weights[heaviest] > 0)
				position = transformPoint(matrices[skin.bones[heaviest]], position);

			min = {std::min(min.x, position.x), std::min(min.y, position.y), std::min(min.z, position.z)};
			max = {std::max(max.x, position.x), std::max(max.y, position.y), std::max(max.z, position.z)};
		}
	}
}

void Pose::rotateNode(const Model& model, int node, const vec3& axis, float radians)
{
	if (node < 0 || static_cast<size_t>(node) >= rotations.size())
		return;

	// Rotation of the parent in model space, to bring the axis into the space the node turns in.
	quat parent;
	std::vector<int> chain;
	for (int i = model.nodes[static_cast<size_t>(node)].parent; i >= 0; i = model.nodes[static_cast<size_t>(i)].parent)
		chain.push_back(i);
	for (auto it = chain.rbegin(); it != chain.rend(); ++it)
		parent = parent * rotations[static_cast<size_t>(*it)];

	const vec3 localAxis = parent.conjugate().rotate(axis);
	rotations[static_cast<size_t>(node)] = (quat::fromAxisAngle(localAxis, radians) * rotations[static_cast<size_t>(node)]).normalized();
}

void restPose(const Model& model, Pose& pose)
{
	const size_t count = model.nodes.size();
	pose.translations.resize(count);
	pose.rotations.resize(count);
	pose.scales.resize(count);
	for (size_t i = 0; i < count; ++i)
	{
		pose.translations[i] = model.nodes[i].translation;
		pose.rotations[i] = model.nodes[i].rotation;
		pose.scales[i] = model.nodes[i].scale;
	}
}

void sampleClip(const Model& model, const AnimationClip& clip, float time, bool loop, Pose& pose)
{
	if (clip.duration > 0.0f)
		time = loop ? std::fmod(std::max(time, 0.0f), clip.duration) : std::clamp(time, 0.0f, clip.duration);
	else
		time = 0.0f;

	for (const AnimationChannel& channel : clip.channels)
	{
		const size_t node = static_cast<size_t>(channel.node);
		if (channel.times.empty() || node >= model.nodes.size())
			continue;

		// Keys around `time`: `next` is the first one after it.
		const size_t next = static_cast<size_t>(std::upper_bound(channel.times.begin(), channel.times.end(), time) - channel.times.begin());
		const size_t from = next == 0 ? 0 : next - 1;
		const size_t to	  = std::min(next, channel.times.size() - 1);

		float blend = 0.0f;
		if (!channel.step && to != from)
			blend = std::clamp((time - channel.times[from]) / (channel.times[to] - channel.times[from]), 0.0f, 1.0f);

		const vec4& a = channel.values[from];
		const vec4& b = channel.values[to];
		if (channel.path == AnimationChannel::Path::Rotation)
		{
			pose.rotations[node] = quat::slerp(toQuat(a), toQuat(b), blend);
		}
		else
		{
			const vec4 value = a + (b - a) * blend;
			vec3& target = channel.path == AnimationChannel::Path::Translation ? pose.translations[node] : pose.scales[node];
			target = {value.x, value.y, value.z};
		}
	}
}

void blendPoses(Pose& pose, const Pose& other, float weight)
{
	const size_t count = std::min(pose.rotations.size(), other.rotations.size());
	for (size_t i = 0; i < count; ++i)
	{
		pose.translations[i] = pose.translations[i] + (other.translations[i] - pose.translations[i]) * weight;
		pose.rotations[i] = quat::slerp(pose.rotations[i], other.rotations[i], weight);
		pose.scales[i] = pose.scales[i] + (other.scales[i] - pose.scales[i]) * weight;
	}
}

void computeBoneMatrices(const Model& model, const Pose& pose, std::vector<mat4>& matrices)
{
	std::vector<mat4> globals;
	globalMatrices(model, pose, globals);

	matrices.resize(model.bones.size());
	for (size_t i = 0; i < model.bones.size(); ++i)
		matrices[i] = globals[static_cast<size_t>(model.bones[i].node)] * model.bones[i].inverseBind;
}

mat4 nodeMatrix(const Model& model, const Pose& pose, int node)
{
	mat4 result = mat4::identity();
	for (int i = node; i >= 0; i = model.nodes[static_cast<size_t>(i)].parent)
	{
		const size_t index = static_cast<size_t>(i);
		result = compose(pose.translations[index], pose.rotations[index], pose.scales[index]) * result;
	}
	return result;
}

void AnimationPlayer::play(int clip, float fadeTime, bool loop, bool restart)
{
	if (clip == current_.clip && !restart)
	{
		current_.loop = loop;
		return;
	}

	previous_ = current_;
	current_  = Track{clip, 0.0f, 1.0f, loop};
	fadeTime_ = previous_.clip >= 0 ? fadeTime : 0.0f;
	fade_	  = fadeTime_;
}

void AnimationPlayer::update(float dt)
{
	current_.time += dt * current_.speed;
	previous_.time += dt * previous_.speed;
	fade_ = std::max(fade_ - dt, 0.0f);
}

void AnimationPlayer::pose(const Model& model, Pose& out) const
{
	const auto sample = [&](const Track& track, Pose& target) {
		restPose(model, target);
		if (track.clip >= 0 && static_cast<size_t>(track.clip) < model.clips.size())
			sampleClip(model, model.clips[static_cast<size_t>(track.clip)], track.time, track.loop, target);
	};

	sample(current_, out);
	if (fade_ > 0.0f && fadeTime_ > 0.0f)
	{
		sample(previous_, scratch_);
		blendPoses(out, scratch_, fade_ / fadeTime_);
	}
}

bool AnimationPlayer::finished(const Model& model) const
{
	if (current_.clip < 0 || static_cast<size_t>(current_.clip) >= model.clips.size())
		return true;
	return !current_.loop && current_.time >= model.clips[static_cast<size_t>(current_.clip)].duration;
}
