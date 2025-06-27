#include "OBBCalculation.h"
#include "matrix4x4Calculation.h"
#include "vector3Calculation.h"
#include "AABBCalculation.h"
#include <Novice.h>

void OBB::SetOrientations(Vector3& rotate) {
	Matrix4x4 rotateMatrix;
	rotateMatrix = MakeRotateXYZMatrix(rotate);
	orientations_[0] = { rotateMatrix.m[0][0],rotateMatrix.m[0][1] ,rotateMatrix.m[0][2] };
	orientations_[1] = { rotateMatrix.m[1][0],rotateMatrix.m[1][1] ,rotateMatrix.m[1][2] };
	orientations_[2] = { rotateMatrix.m[2][0],rotateMatrix.m[2][1] ,rotateMatrix.m[2][2] };
}

Matrix4x4 OBB::MakeScale() {
	Matrix4x4 ret = MakeScaleMatrix(size_);
	return ret;
}

Matrix4x4 OBB::MakeRotate() {
	Matrix4x4 ret{
		orientations_[0].x, orientations_[0].y, orientations_[0].z, 0,
		orientations_[1].x, orientations_[1].y, orientations_[1].z, 0,
		orientations_[2].x, orientations_[2].y, orientations_[2].z, 0,
						 0,                  0,                  0, 1
	};
	return ret;
}

Matrix4x4 OBB::MakeTranslate() {
	Matrix4x4 ret = MakeTranslateMatrix(center_);
	return ret;
}

Matrix4x4 OBB::MakeWorld() {
	Matrix4x4 ret;
	ret = MakeScale();
	ret = Multiply(ret, MakeRotate());
	ret = Multiply(ret, MakeTranslate());
	return ret;
}

Matrix4x4 OBB::InverseRotate() {
	Matrix4x4 ret = OBB::MakeRotate();
	ret = Inverse(ret);
	return ret;
}

Matrix4x4 OBB::InverseWorld() {
	Matrix4x4 ret = OBB::MakeWorld();
	ret = Inverse(ret);
	return ret;
}

AABB OBB::MakeAABB() {
	AABB aabb{
		.min = Multiply(-1.0f,size_),
		.max = size_
	};
	return aabb;
}

void OBB::Draw(Vector3 scaleCamera, Vector3 rotateCamera, Vector3 translateCamera, int color,
	float width, float height, float fovY, float nearClip, float farClip,
	float left, float top, float minDepth, float maxDepth) {
	Vector3 vertices[8] = {
		{ center_.x - size_.x, center_.y - size_.y, center_.z - size_.z },// 手前左下
		{ center_.x + size_.x, center_.y - size_.y, center_.z - size_.z },// 手前右下
		{ center_.x + size_.x, center_.y + size_.y, center_.z - size_.z },// 手前右上
		{ center_.x - size_.x, center_.y + size_.y, center_.z - size_.z },// 手前左上
		{ center_.x - size_.x, center_.y - size_.y, center_.z + size_.z },// 奥左下
		{ center_.x + size_.x, center_.y - size_.y, center_.z + size_.z },// 奥右下
		{ center_.x + size_.x, center_.y + size_.y, center_.z + size_.z },// 奥右上
		{ center_.x - size_.x, center_.y + size_.y, center_.z + size_.z } // 奥左上
	};
	for (int i = 0; i < 8; ++i) {
		
		Matrix4x4 cameraMatrix = MakeAffineMatrix(scaleCamera, rotateCamera, translateCamera);
		Matrix4x4 viewMatrix = Inverse(cameraMatrix);

		Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(fovY, width / height, nearClip, farClip);
		Matrix4x4 worldViewProjectionMatrix = Multiply(MakeWorld(), Multiply(viewMatrix, projectionMatrix));
		Matrix4x4 viewportMatrix = MakeViewportMatrix(left, top, width, height, minDepth, maxDepth);

		Vector3 ndcVertex = Transform(vertices[i], worldViewProjectionMatrix);
		Vector3 screenVertex = Transform(ndcVertex, viewportMatrix);
		vertices[i] = screenVertex;
	}
	Novice::DrawLine(static_cast<int>(vertices[0].x), static_cast<int>(vertices[0].y),
		static_cast<int>(vertices[1].x), static_cast<int>(vertices[1].y), color);
	Novice::DrawLine(static_cast<int>(vertices[1].x), static_cast<int>(vertices[1].y),
		static_cast<int>(vertices[2].x), static_cast<int>(vertices[2].y), color);
	Novice::DrawLine(static_cast<int>(vertices[2].x), static_cast<int>(vertices[2].y),
		static_cast<int>(vertices[3].x), static_cast<int>(vertices[3].y), color);
	Novice::DrawLine(static_cast<int>(vertices[3].x), static_cast<int>(vertices[3].y),
		static_cast<int>(vertices[0].x), static_cast<int>(vertices[0].y), color);

	Novice::DrawLine(static_cast<int>(vertices[4].x), static_cast<int>(vertices[4].y),
		static_cast<int>(vertices[5].x), static_cast<int>(vertices[5].y), color);
	Novice::DrawLine(static_cast<int>(vertices[5].x), static_cast<int>(vertices[5].y),
		static_cast<int>(vertices[6].x), static_cast<int>(vertices[6].y), color);
	Novice::DrawLine(static_cast<int>(vertices[6].x), static_cast<int>(vertices[6].y),
		static_cast<int>(vertices[7].x), static_cast<int>(vertices[7].y), color);
	Novice::DrawLine(static_cast<int>(vertices[7].x), static_cast<int>(vertices[7].y),
		static_cast<int>(vertices[4].x), static_cast<int>(vertices[4].y), color);

	Novice::DrawLine(static_cast<int>(vertices[0].x), static_cast<int>(vertices[0].y),
		static_cast<int>(vertices[4].x), static_cast<int>(vertices[4].y), color);
	Novice::DrawLine(static_cast<int>(vertices[1].x), static_cast<int>(vertices[1].y),
		static_cast<int>(vertices[5].x), static_cast<int>(vertices[5].y), color);
	Novice::DrawLine(static_cast<int>(vertices[2].x), static_cast<int>(vertices[2].y),
		static_cast<int>(vertices[6].x), static_cast<int>(vertices[6].y), color);
	Novice::DrawLine(static_cast<int>(vertices[3].x), static_cast<int>(vertices[3].y),
		static_cast<int>(vertices[7].x), static_cast<int>(vertices[7].y), color);
}

bool IsHitOBB2Sphere(OBB& obb, Sphere& sphere) {
	AABB aabb = obb.MakeAABB();
	Sphere localSphere = sphere;
	localSphere.center = Transform(sphere.center, obb.InverseWorld());
	if (IsHitAABB2Sphere(aabb, localSphere)) {
		return true;
	}
	return false;
}