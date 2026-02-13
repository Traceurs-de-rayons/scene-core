#pragma once

#include "core-utils.hpp"

using namespace cu::math;

class Transform {

private:
	vec4 position_ = vec4(0.0f);
	quat rotation_;
	vec4 scale_ = vec4(1.0f);
	
	mutable mat4 cachedMatrix_;
	mutable bool matrixDirty_ = true;

public:
	void setPosition(const vec4& pos);
	void setPosition(const vec3& pos);
	const vec4& getPosition() const { return position_; }
	const vec3 getPositionVec3() const { return vec3(position_.x, position_.y, position_.z); }
	
	void setRotation(const quat& rot);
	const quat& getRotation() const { return rotation_; }
	
	void setEulerAngles(float yaw, float pitch, float roll);

	vec3 getEulerAngles() const { return quat::toEuler(rotation_);}
	
	void setScale(const vec4& scale);
	void setScale(const vec3& scale);
	const vec4& getScale() const { return scale_; }
	const vec3& getScaleVec3() const { return {scale_.x, scale_.y, scale_.z}; }
	
	const mat4& getMatrix() const;
	
	void rotate(const quat& delta);
	
	void translate(const vec4& delta);
	void translate(const vec3& delta);
};