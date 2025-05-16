#pragma once
#include "baseHeaders.h"
#define PI float(M_PI)

static const int kColumnWidth = 60;
static const int kRowHeight = 20;

struct Vector3 {
	float x, y, z;
};

struct Matrix4x4 {
	float m[4][4];
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