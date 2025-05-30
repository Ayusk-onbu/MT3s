#pragma once
#include "structures.h"

bool IsHitAABB2AABB(const AABB& a, const AABB& b);

bool IsHitAABB2Sphere(const AABB& a, const Sphere& sphere);

bool IsHitAABB2Segment(const AABB& a, const Segment& segment);


