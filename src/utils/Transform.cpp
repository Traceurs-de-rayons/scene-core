#include "Transform.hpp"

using namespace cu::math;

void Transform::setTranslation(const vec4& pos) {
	position_ = pos;
	matrixDirty_ = true;
}
void Transform::setTranslation(const vec3& pos) {
	position_ = {pos.x, pos.y, pos.z, 1};
	matrixDirty_ = true;
}
	
void Transform::setRotation(const quat& rot) {
	rotation_ = rot.normalized();
	matrixDirty_ = true;
}
	
void Transform::setEulerAngles(float yaw, float pitch, float roll) {
	rotation_ = quat::fromEuler(roll, pitch, yaw);
	matrixDirty_ = true;
}

void Transform::setScale(const vec4& scale) {
	scale_ = scale;
	matrixDirty_ = true;
}
void Transform::setScale(const vec3& scale) {
	scale_ = {scale.x, scale.y, scale.z, 1};
	matrixDirty_ = true;
}

const mat4& Transform::getMatrix() const {
	if (matrixDirty_) {
		cachedMatrix_ = cu::math::translate(mat4(1.0f), position_);
		cachedMatrix_ = cachedMatrix_ * rotation_.toMatrix();
		cachedMatrix_ = cu::math::scale(cachedMatrix_, scale_);
		matrixDirty_ = false;
	}
	return cachedMatrix_;
}

void Transform::rotate(const quat& delta) {
	rotation_ = rotation_ * delta;
	matrixDirty_ = true;
}

void Transform::translate(const vec4& delta) {
	position_ += delta;
	matrixDirty_ = true;
}
void Transform::translate(const vec3& delta) {
	position_ += vec4(delta.x, delta.y, delta.z, 0);
	matrixDirty_ = true;
}
