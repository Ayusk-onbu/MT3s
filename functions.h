#pragma once
#include "structures.h"

float cot(float theta);

int GetDegreeFromRadian(const float theta);

float GetCosThetaFromVertex3(const Vector3& A, const Vector3& B, const Vector3& C);

float Lerp(float v1, float v2, float t);

Vector3 Lerp(const Vector3& v1, const Vector3& v2, float t);

/// <summary>/// /// </summary>/// <param name="scale"></param>/// <param name="rotate"></param>/// <param name="translate"></param>/// <param name="world"></param>/// <returns></returns>
Matrix4x4 MakeHierarchy(Vector3& scale, Vector3& rotate, Vector3& translate, Matrix4x4 world);
/// <summary> 等速円運動 </summary>
/// <param name="origin 中心(x, y, z)"></param>
/// <param name="pos 円の座標(x, y, z)"></param>
/// <param name="angle 角度(θ)"></param>
/// <param name="angularVelocity 角速度(ω (θ/s) )"></param>
/// <param name="deltaTime(s)"></param>
void UniformCircularMotion(Vector3 origin,Vector3& pos, float& angle, float angularVelocity,float deltaTime = 1.0f / 60.0f);

Vector2 UniformCircularMotionSpeed(float radius, float angularVelocity, float angle);

float UniformCircularMotionAcceleration(float angularVelocity, float radius);

void PendulumMotion(Vector3& pos,Vector3& anchor, float& length, float& angle, float& angularVelocity, float& angularAcceleration,float deltaTime = 1.0f / 60.0f);

void ConicalPendulumMotion(Vector3& pos, Vector3& anchor, float& length, float& halfApexAngle, float& angle, float& angularVelocity, float deltaTime = 1.0f / 60.0f);

Vector3 Reflect(const Vector3& input,const Vector3& normal);

uint32_t Factorial(const uint32_t& num);

uint32_t Permutation(const uint32_t& n, const uint32_t& k);

/// <summary> ロドリゲスの回転 </summary>
/// <param name="axis : 回転軸の向き"></param>
/// <param name="angle : どの位回転させるか"></param>
Matrix4x4 MakeRotateAxisAngle(const Vector3& axis, float angle);