#include <Novice.h>
#include "Bezier.h"
#include "functions.h"
#include "matrix4x4Calculation.h"


void Bezier::Draw(const Vector3& v0, const Vector3& v1, const Vector3& v2, float t,
	Vector3 scaleCamera, Vector3 rotateCamera, Vector3 translateCamera, int color,
	float width, float height, float fovY, float nearClip, float farClip,
	float left, float top, float minDepth, float maxDepth) {

	Vector3 p0p1;
	Vector3 p1p2;
	
	p0p1 = Lerp(v0, v1, t);
	p1p2 = Lerp(v1, v2, t);
	curPos_ = Lerp(p0p1, p1p2, t);
	if (t == 1.0f) {
		prePos_ = curPos_;
	}
	Vector3 startVertex;
	startVertex = RenderingPipelineVer2({ 1.0f,1.0f,1.0f }, { 0.0f,0.0f,0.0f }, {0.0f,0.0f,0.0f}, scaleCamera, rotateCamera, translateCamera, prePos_, width, height, fovY, nearClip, farClip, left, top, minDepth, maxDepth);
	Vector3 endVertex;
	endVertex = RenderingPipelineVer2({ 1.0f,1.0f,1.0f }, { 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f }, scaleCamera, rotateCamera, translateCamera, curPos_, width, height, fovY, nearClip, farClip, left, top, minDepth, maxDepth);

	Novice::DrawLine(
		static_cast<int>(startVertex.x), static_cast<int>(startVertex.y),
		static_cast<int>(endVertex.x), static_cast<int>(endVertex.y),
		color);
	if(t != 1.0f)
	prePos_ = curPos_;
}