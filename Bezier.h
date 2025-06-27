#pragma once
#include "structures.h"
#include "vector3Calculation.h"

class Bezier
{
public:
	void Draw(const Vector3& v0, const Vector3& v1, const Vector3& v2, float t,
		Vector3 scaleCamera, Vector3 rotateCamera, Vector3 translateCamera, int color = 0xFFFFFFFF,
		float width = 1280.0f, float height = 720.0f, float fovY = 0.45f, float nearClip = 0.1f, float farClip = 100.0f,
		float left = 0.0f, float top = 0.0f, float minDepth = 0.0f, float maxDepth = 0.0f);

	void SetPrePos(const Vector3& prePos) {
		prePos_ = prePos;
	}

private:
	Vector3 prePos_ = {0.0f,0.0f,0.0f};
	Vector3 curPos_;
};

