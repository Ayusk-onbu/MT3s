#pragma once
#include "structures.h"
#include "OBBCalculation.h"

void DrawGrid(Vector3 scaleCamera, Vector3 rotateCamera, Vector3 translateCamera,
	Vector3 scale = { 1.0f,1.0f,1.0f }, Vector3 rotate = { 0.0f,0.0f,0.0f }, Vector3 translate = { 0.0f,0.0f,0.0f },
	float width = 1280.0f, float height = 720.0f, float fovY = 0.45f, float nearClip = 0.1f, float farClip = 100.0f,
	float left = 0.0f, float top = 0.0f, float minDepth = 0.0f, float maxDepth = 0.0f);

void DrawSphere(const Sphere& sphere, Vector3 scaleCamera, Vector3 rotateCamera, Vector3 translateCamera, int color = 0xffffffff,
	Vector3 scale = { 1.0f,1.0f,1.0f }, Vector3 rotate = { 0.0f,0.0f,0.0f }, Vector3 translate = { 0.0f,0.0f,0.0f },
	float width = 1280.0f, float height = 720.0f, float fovY = 0.45f, float nearClip = 0.1f, float farClip = 100.0f,
	float left = 0.0f, float top = 0.0f, float minDepth = 0.0f, float maxDepth = 0.0f);

void DrawSphere(const Sphere& sphere, Vector3 scaleCamera, Vector3 rotateCamera, Vector3 translateCamera, int color,
	Matrix4x4 world,
	float width = 1280.0f, float height = 720.0f, float fovY = 0.45f, float nearClip = 0.1f, float farClip = 100.0f,
	float left = 0.0f, float top = 0.0f, float minDepth = 0.0f, float maxDepth = 0.0f);

void DrawPlane(const Plane& plane, Vector3 scaleCamera, Vector3 rotateCamera, Vector3 translateCamera, int color = 0xffffffff,
	Vector3 scale = { 1.0f,1.0f,1.0f }, Vector3 rotate = { 0.0f,0.0f,0.0f }, Vector3 translate = { 0.0f,0.0f,0.0f },
	float width = 1280.0f, float height = 720.0f, float fovY = 0.45f, float nearClip = 0.1f, float farClip = 100.0f,
	float left = 0.0f, float top = 0.0f, float minDepth = 0.0f, float maxDepth = 0.0f);

void DrawTriangle(const Triangle& triangle, Vector3 scaleCamera, Vector3 rotateCamera, Vector3 translateCamera, int color = 0xffffffff,
	Vector3 scale = { 1.0f,1.0f,1.0f }, Vector3 rotate = { 0.0f,0.0f,0.0f }, Vector3 translate = { 0.0f,0.0f,0.0f },
	float width = 1280.0f, float height = 720.0f, float fovY = 0.45f, float nearClip = 0.1f, float farClip = 100.0f,
	float left = 0.0f, float top = 0.0f, float minDepth = 0.0f, float maxDepth = 0.0f);

//void DrawTriangle(const Triangle& triangle, Matrix4x4 &viewProjectionMatrix,Matrix4x4&viewPortMatrix,int color);

void DrawAABB(const AABB& a, Vector3 scaleCamera, Vector3 rotateCamera, Vector3 translateCamera, int color = 0xffffffff,
	Vector3 scale = { 1.0f,1.0f,1.0f }, Vector3 rotate = { 0.0f,0.0f,0.0f }, Vector3 translate = { 0.0f,0.0f,0.0f },
	float width = 1280.0f, float height = 720.0f, float fovY = 0.45f, float nearClip = 0.1f, float farClip = 100.0f,
	float left = 0.0f, float top = 0.0f, float minDepth = 0.0f, float maxDepth = 0.0f);

void DrawSpecialTriangle3D(const Vector3& top,
	const Vector3& backLeft, const Vector3& backRight,
	const Vector3& frontLeft, const Vector3& frontRight,
	Vector3 scaleCamera, Vector3 rotateCamera, Vector3 translateCamera,
	int color = 0xffffffff,
	Vector3 scale = { 1.0f,1.0f,1.0f }, Vector3 rotate = { 0.0f,0.0f,0.0f }, Vector3 translate = { 0.0f,0.0f,0.0f },
	float width = 1280.0f, float height = 720.0f, float fovY = 0.45f, float nearClip = 0.1f, float farClip = 100.0f,
	float left = 0.0f, float topOffset = 0.0f, float minDepth = 0.0f, float maxDepth = 0.0f);

void DrawCustomCuboid(const Vector3& frontTL, const Vector3& frontTR, const Vector3& frontBR, const Vector3& frontBL,
	const Vector3& backTL, const Vector3& backTR, const Vector3& backBR, const Vector3& backBL,
	Vector3 scaleCamera, Vector3 rotateCamera, Vector3 translateCamera,
	int color = 0xffffffff,
	Vector3 scale = { 1.0f,1.0f,1.0f }, Vector3 rotate = { 0.0f,0.0f,0.0f }, Vector3 translate = { 0.0f,0.0f,0.0f },
	float width = 1280.0f, float height = 720.0f, float fovY = 0.45f, float nearClip = 0.1f, float farClip = 100.0f,
	float left = 0.0f, float top = 0.0f, float minDepth = 0.0f, float maxDepth = 0.0f);