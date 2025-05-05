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
};
