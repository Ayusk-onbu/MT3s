#include "planeCalculation.h"
#include "vector3Calculation.h"
#include "matrix4x4Calculation.h"
#include <Novice.h>
#include "functions.h"
#include "drawSeries.h"

std::vector<Vector3> GetPlaneVertex(const Plane& plane, Matrix4x4 viewProjectionMatrix, Matrix4x4 viewPortMatrix) {
	// 中心を決める
	Vector3 center = Multiply(plane.distance, plane.normal);
	// ?
	Vector3 perpendiculars[4];
	perpendiculars[0] = Normalize(Perpendicular(plane.normal));
	perpendiculars[1] = { -perpendiculars[0].x,-perpendiculars[0].y,-perpendiculars[0].z };
	perpendiculars[2] = CrossProduct(plane.normal, perpendiculars[0]);
	perpendiculars[3] = { -perpendiculars[2].x,-perpendiculars[2].y,-perpendiculars[2].z };

	std::vector<Vector3> points(4);
	for (int32_t index = 0;index < 4;++index) {
		Vector3 extend = Multiply(2.0f, perpendiculars[index]);
		Vector3 point = Add(center, extend);
		points[index] = Transform(Transform(point, viewProjectionMatrix), viewPortMatrix);
	}
	return points;
}

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

bool IsHitPlane2Segment(Plane& plane, Segment& segment, Matrix4x4 viewProjectionMatrix, Matrix4x4 viewPortMatrix) {
	// 平行かどうか判断する
	float dot = Dot(segment.diff, plane.normal);
	if (dot == 0) { return false; }

	// 当たり判定を取る
	// ために当たっている点を求める（ d = a・n）( a = o + tb)( d = ( o + tb )・n)( t = (d - o・n)/b・n
	float t = (plane.distance - Dot(segment.origin, plane.normal)) / dot;
	if (t < 0 || t > 1) { return false; }
	Vector3 point = { segment.origin + Multiply(t,segment.diff) };
	Vector3 start = Transform(Transform(point, viewProjectionMatrix), viewPortMatrix);
	if (plane.distance == point.x * plane.normal.x + point.y * plane.normal.y + point.z * plane.normal.z) {
		std::vector<Vector3> pointData = GetPlaneVertex(plane, viewProjectionMatrix, viewPortMatrix);
		Vector3 points[4];
		for (int i = 0;i < 4;++i) {
			points[i] = pointData[i];
		}
		DrawSphere({ point,0.05f,int(0xff0000ff) }, { 1.0f,1.0f,0.250f }, { 0.26f,0.0f,0.0f }, { 0.0f,1.9f,-6.49f }, int(0xff0000ff));
		for (int i = 0;i < 4;++i) {
			Novice::DrawLine(int(start.x), int(start.y), int(points[i].x), int(points[i].y), 0x000000ff);
		}
		int isHit = 0;;
		if (Dot((CrossProduct(points[2] - points[0], start - points[2])), plane.normal) >= 0.0f) {
			isHit += 1;
		}
		if (Dot((CrossProduct(points[3] - points[1], start - points[3])), plane.normal) >= 0.0f) {
			isHit += 2;
		}
		if (Dot((CrossProduct(points[1] - points[2], start - points[1])), plane.normal) >= 0.0f) {
			isHit += 4;
		}
		if (Dot((CrossProduct(points[0] - points[3], start - points[0])), plane.normal) >= 0.0f) {
			isHit += 8;
		}
		if (isHit == 15) {
			return true;
		}
	}
	return false;
}