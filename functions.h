#pragma once
#include "structures.h"

float cot(float theta);

int GetDegreeFromRadian(const float theta);

float GetCosThetaFromVertex3(const Vector3& A, const Vector3& B, const Vector3& C);

float Lerp(float v1, float v2, float t);

Vector3 Lerp(const Vector3& v1, const Vector3& v2, float t);

/// <summary>/// /// </summary>/// <param name="scale"></param>/// <param name="rotate"></param>/// <param name="translate"></param>/// <param name="world"></param>/// <returns></returns>
Matrix4x4 MakeHierarchy(Vector3& scale, Vector3& rotate, Vector3& translate, Matrix4x4 world);