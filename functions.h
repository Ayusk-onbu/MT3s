#pragma once
#include "structures.h"

float cot(float theta);

int GetDegreeFromRadian(const float theta);

float GetCosThetaFromVertex3(const Vector3& A, const Vector3& B, const Vector3& C);

float Lerp(float v1, float v2, float t);

Vector3 Lerp(const Vector3& v1, const Vector3& v2, float t);