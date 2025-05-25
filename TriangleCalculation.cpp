#include "TriangleCalculation.h"
#include "vector3Calculation.h"
#include "matrix4x4Calculation.h"

float GetDistanceFromNormal2Point(const Vector3&normal, const Vector3& point) {
	float distance = point.x * normal.x + point.y * normal.y + point.z * normal.z;
	return distance;
}

Vector3 GetNormalFromTriangle(const Triangle& triangle) {
	Vector3 ab = triangle.vertices[1] - triangle.vertices[0];
	Vector3 bc = triangle.vertices[2] - triangle.vertices[1];
	Vector3 normal = CrossProduct(ab, bc);
	return Normalize(normal);
}

bool IsHitTriangle2Segment(const Triangle& triangle, const Segment& segment) {
	// 三角形の法線を求める
	Vector3 normal = GetNormalFromTriangle(triangle);
	// 平行かどうか判断する
	float dot = Dot(segment.diff, normal);
	if (dot == 0) { return false; }

	// 当たり判定を取る
	// ために当たっている点を求める（ d = a・n）( a = o + tb)( d = ( o + tb )・n)( t = (d - o・n)/b・n
	float distance = GetDistanceFromNormal2Point(normal, triangle.vertices[0]);
	float t = (distance - Dot(segment.origin, normal)) / dot;
	if (t < 0 || t > 1) { return false; }
	Vector3 point = { segment.origin + Multiply(t,segment.diff) };
	bool isHit = false;
	if (distance == point.x * normal.x + point.y * normal.y + point.z * normal.z) {
		isHit = true;
	}
	if (isHit) {
		Vector3 cross01 = CrossProduct(triangle.vertices[1] - triangle.vertices[0], point - triangle.vertices[1]);
		Vector3 cross12 = CrossProduct(triangle.vertices[2] - triangle.vertices[1], point - triangle.vertices[2]);
		Vector3 cross20 = CrossProduct(triangle.vertices[0] - triangle.vertices[2], point - triangle.vertices[0]);
		if (Dot(cross01, normal) >= 0.0f && 
			Dot(cross12, normal) >= 0.0f && 
			Dot(cross20, normal) >= 0.0f) {
			return true;
		}
	}
	return false;
}