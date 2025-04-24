#include "vector3Calculation.h"

//加算
Vector3 Add(const Vector3& v1, const Vector3& v2) {
	Vector3 ret;
	ret.x = v1.x + v2.x;
	ret.y = v1.y + v2.y;
	ret.z = v1.z + v2.z;
	return ret;
}
//減算
Vector3 Subtract(const Vector3& v1, const Vector3& v2) {
	Vector3 ret;
	ret.x = v1.x - v2.x;
	ret.y = v1.y - v2.y;
	ret.z = v1.z - v2.z;
	return ret;
}
//スカラー倍
Vector3 Multiply(float scaler, const Vector3& v) {
	Vector3 ret;
	ret.x = v.x * scaler;
	ret.y = v.y * scaler;
	ret.z = v.z * scaler;
	return ret;
}
//内積
float Dot(const Vector3& v1, const Vector3& v2) {
	float ret;
	ret = v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
	return ret;
}
//長さ
float Length(const Vector3& v) {
	float ret;
	ret = sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
	return ret;
}
//正規化
Vector3 Normalize(const Vector3& v) {
	Vector3 ret;
	ret.x = v.x / Length(v);
	ret.y = v.y / Length(v);
	ret.z = v.z / Length(v);
	return ret;
}