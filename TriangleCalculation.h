#pragma once
#include "structures.h"

float GetDistanceFromNormal2Point(const Vector3& normal, const Vector3& point);

Vector3 GetNormalFromTriangle(const Triangle& triangle);

bool IsHitTriangle2Segment(const Triangle& triangle, const Segment& segment);

