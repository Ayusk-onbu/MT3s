#include "functions.h"
#include "vector3Calculation.h"
#include <algorithm>
#include "matrix4x4Calculation.h"

float cot(float theta) {
	float ret = 1.0f / std::tanf(theta);
	return ret;
}

int GetDegreeFromRadian(const float theta) {
	float ret = theta * 180.0f / PI;
	return static_cast<int>(ret);
}

float GetCosThetaFromVertex3(const Vector3& A, const Vector3& B, const Vector3& C) {
	
	float ab = Length(B - A);
	float ac = Length(C - A);
	
	if (ab == 0.0f || ac == 0.0f) {
	    return 0;  // 角度が計算できない場合
	}
	
	Vector3 AB = B - A;
	Vector3 AC = C - A;
	float cosTheta = Dot(AB, AC) / (ac * ab);
	
	// acos に渡す前に値をクランプ
	//cosTheta = std::clamp(cosTheta, -1.0, 1.0);
	//float theta = static_cast<float>(std::acos(cosTheta));
	
	return cosTheta;
	
}

float Lerp(float v1, float v2, float t) {
	return v1 + (v2 - v1) * t;
}

Vector3 Lerp(const Vector3& v1, const Vector3& v2, float t) {
	return {
		Lerp(v1.x,v2.x,t),
		Lerp(v1.y,v2.y,t),
		Lerp(v1.z,v2.z,t)
	};
}

Matrix4x4 MakeHierarchy(Vector3& scale, Vector3& rotate, Vector3& translate, Matrix4x4 world) {
	Matrix4x4 children;
	children = MakeAffineMatrix(scale, rotate, translate);
	children = Multiply(children, world);
	return children;
}

void UniformCircularMotion(Vector3 origin, Vector3& pos, float& angle, float angularVelocity, float deltaTime) {
	angle += angularVelocity * deltaTime;
	pos.x = origin.x + std::cos(angle);
	pos.y = origin.y + std::sin(angle);
	pos.z = origin.z;
}

Vector2 UniformCircularMotionSpeed(float radius, float angularVelocity, float angle) {
	return { -radius * angularVelocity * std::sin(angle),radius * angularVelocity * std::cos(angle) };
}

float UniformCircularMotionAcceleration(float angularVelocity, float radius) {
	return -std::powf(angularVelocity, 2.0f) * radius;
}

void PendulumMotion(Vector3& pos, Vector3& anchor, float& length, float& angle, float& angularVelocity, float& angularAcceleration, float deltaTime) {
	angularAcceleration = -(9.8f / length) * std::sin(angle);
	angularVelocity += angularAcceleration * deltaTime;
	angle += angularVelocity * deltaTime;
	pos.x = anchor.x + std::sin(angle) * length;
	pos.y = anchor.y - std::cos(angle) * length;
	pos.z = anchor.z;
}

void ConicalPendulumMotion(Vector3& pos, Vector3& anchor, float& length, float& halfApexAngle, float& angle, float& angularVelocity, float deltaTime){
	angularVelocity = std::sqrt(9.8f / (length * std::cos(halfApexAngle)));
	angle += angularVelocity * deltaTime;
	float radius = std::sin(halfApexAngle) * length;
	float height = std::cos(halfApexAngle) * length;
	pos.x = anchor.x + std::cos(angle) * radius;
	pos.y = anchor.y - height;
	pos.z = anchor.z - std::sin(angle) * radius;
}