#pragma once
#include "structures.h"

class OBB {
public:

	void Setsize(Vector3& scale) { size_ = scale; };

	void SetOrientations(Vector3& rotate);

	void SetCenter(Vector3& translate) { center_ = translate; };

	Matrix4x4 MakeRotate();

	Matrix4x4 InverseRotate();

	Matrix4x4 MakeScale();

	Matrix4x4 MakeTranslate();

	Matrix4x4 MakeWorld();

	Matrix4x4 InverseWorld();

	AABB MakeAABB();

	void Draw(Vector3 scaleCamera, Vector3 rotateCamera, Vector3 translateCamera, int color = 0xffffffff,
		float width = 1280.0f, float height = 720.0f, float fovY = 0.45f, float nearClip = 0.1f, float farClip = 100.0f,
		float left = 0.0f, float top = 0.0f, float minDepth = 0.0f, float maxDepth = 0.0f);

private:
	Vector3 size_;
	Vector3 orientations_[3];
	Vector3 center_;

};

bool IsHitOBB2Sphere(OBB& obb, Sphere& sphere);


