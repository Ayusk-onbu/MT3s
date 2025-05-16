#include "planeCalculation.h"
#include "vector3Calculation.h"
#include <Novice.h>

float GetDistanceFromPlane2PlanePoint(Plane& plane, Vector3& point) {
	plane.distance = point.x * plane.normal.x + point.y * plane.normal.y + point.z * plane.normal.z;
	return plane.distance;
}

bool IsPlanePoint(Plane& plane, Vector3& point) {
	if (plane.distance == point.x * plane.normal.x + point.y * plane.normal.y + point.z * plane.normal.z) {
		return true;
	}
	return false;
}

float DistancePlane2Point(Plane& plane, Vector3& point) {
	// 任意の点 point から平面に下した点 q
	Vector3 q;
	// qを求めたいので q = point - kn のkを求める
	// k = normal・point - distance
	float k = (Dot(Normalize(plane.normal), point) - plane.distance);
	
	Novice::ScreenPrintf(10, 10, "k %f", k);
	q = Subtract(point, Multiply(k, Normalize(plane.normal)));
	// point と q の距離を調べる
	float distance = Length(Subtract(point, q));
	Novice::ScreenPrintf(10, 40, "distance %f", distance);
	return distance;
}

bool IsPlaneHitPoint(Plane& plane, Vector3& point, float length) {
	if (DistancePlane2Point(plane, point) <= length) {
		return true;
	}
	return false;
}