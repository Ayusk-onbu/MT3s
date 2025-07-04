#pragma once
#include "baseHeaders.h"
#define PI float(M_PI)

static const int kColumnWidth = 60;
static const int kRowHeight = 20;

struct Vector3 {
	float x, y, z;

	Vector3 operator+(const Vector3& other)const {
		return{ x + other.x,y + other.y, z + other.z };
	}
	Vector3 operator-(const Vector3& other)const {
		return{ x - other.x,y - other.y, z - other.z };
	}
	Vector3 operator*(const Vector3& other)const {
		return{ x * other.x,y * other.y, z * other.z };
	}
	Vector3 operator/(const Vector3& other)const {
		return{ x / other.x,y / other.y, z / other.z };
	}
	Vector3 operator*(float scalar)const {
		return{ x * scalar,y * scalar, z * scalar };
	}
	bool operator==(const Vector3& other)const {
		if (x == other.x && y == other.y && z == other.z) {
			return true;
		}
		return false;
	}
};

struct Matrix4x4 {
	float m[4][4];

	Matrix4x4 operator*(const Matrix4x4& other) const {
		Matrix4x4 result = {};
		for (int i = 0; i < 4; ++i) {
			for (int j = 0; j < 4; ++j) {
				result.m[i][j] = m[i][0] * other.m[0][j] +
					m[i][1] * other.m[1][j] +
					m[i][2] * other.m[2][j] +
					m[i][3] * other.m[3][j];
			}
		}
		return result;
	}

};

struct Sphere {
	Vector3 center;
	float radius;
	int color;
};

struct Camera {
	Vector3 scale;
	Vector3 rotate;
	Vector3 translate;
};
// 平面
struct Plane {
	Vector3 normal;// 法線
	float distance;// 距離
};
// 三角形
struct Triangle {
	Vector3 vertices[3];
};
//直線
struct Line {
	Vector3 origin;//始点
	Vector3 diff;//終点への差分べくとる
};
//半直線
struct Ray {
	Vector3 origin;//始点
	Vector3 diff;//終点への差分べくとる
};
//線分
struct Segment {
	Vector3 origin;//始点
	Vector3 diff;//終点への差分べくとる
};
// 軸並行う境界箱(Axis Aligned Bounding Box)
class AABB {
public:
	Vector3 min;
	Vector3 max;
	Vector3 center()const {
		Vector3 ret;
		ret.x = (min.x + max.x) / 2.0f;
		ret.y = (min.y + max.y) / 2.0f;
		ret.z = (min.z + max.z) / 2.0f;
		return ret;
	}
};