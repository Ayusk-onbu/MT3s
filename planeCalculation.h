#pragma once
#include "structures.h"

float GetDistanceFromPlane2PlanePoint(Plane& plane, Vector3& point);
/// <summary>
/// 任意の点が平面上にあるか否か
/// </summary>
/// <param name="plane 平面"></param>
/// <param name="point 知りたい点"></param>
/// <returns></returns>
bool IsPlanePoint(Plane& plane, Vector3& point);

float DistancePlane2Point(Plane& plane, Vector3& point);

bool IsPlaneHitPoint(Plane& plane, Vector3& point, float length);

