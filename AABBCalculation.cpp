#include "AABBCalculation.h"
#include "SphereCalculation.h"
#include "vector3Calculation.h"
#include <algorithm>

bool IsHitAABB2AABB(const AABB& a, const AABB& b) {
	if (a.min.x <= b.max.x && b.min.x <= a.max.x &&
		a.min.y <= b.max.y && b.min.y <= a.max.y &&
		a.min.z <= b.max.z && b.min.z <= a.max.z) {
		return true;
	}
	return false;
}

bool IsHitAABB2Sphere(const AABB& a, const Sphere& sphere) {
	Vector3 closestPoint;
	closestPoint.x = std::clamp(sphere.center.x, a.min.x, a.max.x);
	closestPoint.y = std::clamp(sphere.center.y, a.min.y, a.max.y);
	closestPoint.z = std::clamp(sphere.center.z, a.min.z, a.max.z);
	float distance = Length(closestPoint-sphere.center);
	if (distance <= sphere.radius) {
		return true;
	}
	return false;
}