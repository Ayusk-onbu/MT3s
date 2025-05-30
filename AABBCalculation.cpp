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

bool IsHitAABB2Segment(const AABB& a, const Segment& segment) {
	
	float txMin = (a.min.x - segment.origin.x) / segment.diff.x;
	float txMax = (a.max.x - segment.origin.x) / segment.diff.x;
	float tyMin = (a.min.y - segment.origin.y) / segment.diff.y;
	float tyMax = (a.max.y - segment.origin.y) / segment.diff.y;
	float tzMin = (a.min.z - segment.origin.z) / segment.diff.z;
	float tzMax = (a.max.z - segment.origin.z) / segment.diff.z;

	float tNearX = std::min(txMin, txMax);
	float tNearY = std::min(tyMin, tyMax);
	float tNearZ = std::min(tzMin, tzMax);
	float tFarX = std::max(txMin, txMax);
	float tFarY = std::max(tyMin, tyMax);
	float tFarZ = std::max(tzMin, tzMax);
	// AABBとの衝突点(貫通点)のtが小さいほう
	float tMin = std::max(std::max(tNearX, tNearY), tNearZ);
	// AABBとの衝突点(貫通点)のtが大きいほう
	float tMax = std::min(std::min(tFarX, tFarY), tFarZ);
	if (tMin <= tMax) {
		return true;
	}
	return false;
}