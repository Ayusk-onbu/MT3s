#pragma once
#include "structures.h"

void DrawGrid(Vector3 scaleCamera, Vector3 rotateCamera, Vector3 translateCamera,
	Vector3 scale = { 1.0f,1.0f,1.0f }, Vector3 rotate = { 0.0f,0.0f,0.0f }, Vector3 translate = { 0.0f,0.0f,0.0f },
	float width = 1280.0f, float height = 720.0f, float fovY = 0.45f, float nearClip = 0.1f, float farClip = 100.0f,
	float left = 0.0f, float top = 0.0f, float minDepth = 0.0f, float maxDepth = 0.0f);

void DrawSphere(const Sphere& sphere, Vector3 scaleCamera, Vector3 rotateCamera, Vector3 translateCamera,
	Vector3 scale = { 1.0f,1.0f,1.0f }, Vector3 rotate = { 0.0f,0.0f,0.0f }, Vector3 translate = { 0.0f,0.0f,0.0f },
	float width = 1280.0f, float height = 720.0f, float fovY = 0.45f, float nearClip = 0.1f, float farClip = 100.0f,
	float left = 0.0f, float top = 0.0f, float minDepth = 0.0f, float maxDepth = 0.0f);