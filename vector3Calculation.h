#pragma once
#include "structures.h"

//加算
Vector3 Add(const Vector3& v1, const Vector3& v2);
//減算
Vector3 Subtract(const Vector3& v1, const Vector3& v2);
//スカラー倍
Vector3 Multiply(float scaler, const Vector3& v);
//内積
float Dot(const Vector3& v1, const Vector3& v2);
//長さ
float Length(const Vector3& v);
//正規化
Vector3 Normalize(const Vector3& v);
//正射影
Vector3 Project(const Vector3& v1, const Vector3& v2);
//最近接点
Vector3 ClosestPoint(const Vector3& point, const Segment& segment);