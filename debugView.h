#pragma once
#include "structures.h"
#include <imgui.h>

void VectorScreenPrintf(int x, int y, const Vector3& vector, const char* lavel);

void MatrixScreenPrintf(int x, int y, const Matrix4x4& matrix, const char* lavel);

Camera DebugCamera(Camera& camera, int& prePositionX, int& prePositionY);