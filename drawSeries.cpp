#include "drawSeries.h"
#include "matrix4x4Calculation.h"
#include "planeCalculation.h"
#include "vector3Calculation.h"
#include <Novice.h>
#include "imgui.h"

void DrawGrid(Vector3 scaleCamera, Vector3 rotateCamera, Vector3 translateCamera,
	Vector3 scale, Vector3 rotate, Vector3 translate,
	float width, float height, float fovY, float nearClip, float farClip,
	float left, float top, float minDepth, float maxDepth) {
	const float kGridHalfWidth = 2.0f;
	const uint32_t kSubdivision = 10;
	const float kGridEvery = (kGridHalfWidth * 2.0f) / static_cast<float>(kSubdivision);

	for (uint32_t xIndex = 0;xIndex <= kSubdivision;++xIndex) {
		Vector3 startVertex = { -kGridHalfWidth + kGridEvery * xIndex,0.0f,-kGridHalfWidth };
		Vector3 endVertex = { startVertex.x,0.0f,startVertex.z + kGridHalfWidth * 2.0f };
		startVertex = RenderingPipelineVer2(scale, rotate, translate, scaleCamera, rotateCamera, translateCamera, startVertex, width, height, fovY, nearClip, farClip, left, top, minDepth, maxDepth);
		endVertex = RenderingPipelineVer2(scale, rotate, translate, scaleCamera, rotateCamera, translateCamera, endVertex, width, height, fovY, nearClip, farClip, left, top, minDepth, maxDepth);
		Novice::DrawLine(static_cast<int>(startVertex.x),
			static_cast<int>(startVertex.y),
			static_cast<int>(endVertex.x),
			static_cast<int>(endVertex.y),
			0xAABBAAFF);
	}
	for (uint32_t zIndex = 0;zIndex <= kSubdivision;++zIndex) {
		Vector3 startVertex = { -kGridHalfWidth,0.0f,-kGridHalfWidth + kGridEvery * zIndex };
		Vector3 endVertex = { startVertex.x + kGridHalfWidth * 2.0f,0.0f,startVertex.z };
		startVertex = RenderingPipelineVer2(scale, rotate, translate, scaleCamera, rotateCamera, translateCamera, startVertex, width, height, fovY, nearClip, farClip, left, top, minDepth, maxDepth);
		endVertex = RenderingPipelineVer2(scale, rotate, translate, scaleCamera, rotateCamera, translateCamera, endVertex, width, height, fovY, nearClip, farClip, left, top, minDepth, maxDepth);
		Novice::DrawLine(static_cast<int>(startVertex.x),
			static_cast<int>(startVertex.y),
			static_cast<int>(endVertex.x),
			static_cast<int>(endVertex.y),
			0xAABBAAFF);
	}
}

void DrawSphere(const Sphere& sphere, Vector3 scaleCamera, Vector3 rotateCamera, Vector3 translateCamera, int color,
	Vector3 scale, Vector3 rotate, Vector3 translate, 
	float width, float height, float fovY, float nearClip, float farClip,
	float left, float top, float minDepth, float maxDepth) {
	const uint32_t kSubdivision = 10;
	const float kLonEvery = 1.0f / kSubdivision * PI;//緯度分割一つ分の角度
	const float kLatEvery = 2.0f / kSubdivision * PI;//軽度分割一つ分の角度
	//緯度の方向に分割 -π/2～π/2
	for (uint32_t latIndex = 0;latIndex < kSubdivision;++latIndex) {
		float lat = -PI / 2.0f + kLatEvery * latIndex;//現在の緯度
		//緯度の方向に分割 0～2π
		for (uint32_t lonIndex = 0;lonIndex < kSubdivision;++lonIndex) {
			float lon = lonIndex * kLonEvery;//現在の軽度
			//world座標系でのa,b,cを求める
			Vector3 a, b, c;
			a = { std::cos(lat) * std::cos(lon),std::sin(lat),std::cos(lat) * std::sin(lon) };
			b = { std::cos(lat + kLatEvery) * std::cos(lon),std::sin(lat + kLatEvery),std::cos(lat + kLatEvery) * std::sin(lon) };
			c = { std::cos(lat) * std::cos(lon + kLonEvery),std::sin(lat),std::cos(lat) * std::sin(lon + kLonEvery) };

			a = { a.x * sphere.radius + sphere.center.x,a.y * sphere.radius + sphere.center.y,a.z * sphere.radius + sphere.center.z };
			b = { b.x * sphere.radius + sphere.center.x,b.y * sphere.radius + sphere.center.y,b.z * sphere.radius + sphere.center.z };
			c = { c.x * sphere.radius + sphere.center.x,c.y * sphere.radius + sphere.center.y,c.z * sphere.radius + sphere.center.z };

			a = RenderingPipelineVer2(scale, rotate, translate, scaleCamera, rotateCamera, translateCamera, a, width, height, fovY, nearClip, farClip, left, top, minDepth, maxDepth);
			b = RenderingPipelineVer2(scale, rotate, translate, scaleCamera, rotateCamera, translateCamera, b, width, height, fovY, nearClip, farClip, left, top, minDepth, maxDepth);
			c = RenderingPipelineVer2(scale, rotate, translate, scaleCamera, rotateCamera, translateCamera, c, width, height, fovY, nearClip, farClip, left, top, minDepth, maxDepth);

			Novice::DrawLine(static_cast<int>(a.x),
				static_cast<int>(a.y),
				static_cast<int>(b.x),
				static_cast<int>(b.y),
				color);
			Novice::DrawLine(static_cast<int>(b.x),
				static_cast<int>(b.y),
				static_cast<int>(c.x),
				static_cast<int>(c.y),
				color);
		}
	}
}

void DrawPlane(const Plane& plane, Vector3 scaleCamera, Vector3 rotateCamera, Vector3 translateCamera, int color,
	Vector3 scale, Vector3 rotate, Vector3 translate,
	float width, float height, float fovY, float nearClip, float farClip,
	float left, float top, float minDepth, float maxDepth) 
{
	Matrix4x4 viewProjectionMatrix = MakeViewProjectionMatrix(scale, rotate, translate, scaleCamera, rotateCamera, translateCamera,width,height,fovY,nearClip,farClip);
	Matrix4x4 viewPortMatrix = MakeViewportMatrix(left, top, width, height, minDepth, maxDepth);
	std::vector<Vector3> pointData = GetPlaneVertex(plane,viewProjectionMatrix,viewPortMatrix);
	Vector3 points[4];
	for (int i = 0;i < 4;++i) {
		points[i] = pointData[i];
	}

	ImGui::Begin("DrawPlane");
	ImGui::SliderFloat3("points[0]", &points[0].x, -1.0f, 1.0f);
	ImGui::SliderFloat3("points[1]", &points[1].x, -1.0f, 1.0f);
	ImGui::SliderFloat3("points[2]", &points[2].x, -1.0f, 1.0f);
	ImGui::SliderFloat3("points[3]", &points[3].x, -1.0f, 1.0f);
	ImGui::End();

	Novice::DrawLine(int(points[0].x), int(points[0].y), int(points[2].x), int(points[2].y), color);
	Novice::DrawLine(int(points[1].x), int(points[1].y), int(points[3].x), int(points[3].y), color);
	Novice::DrawLine(int(points[2].x), int(points[2].y), int(points[1].x), int(points[1].y), color);
	Novice::DrawLine(int(points[3].x), int(points[3].y), int(points[0].x), int(points[0].y), color);
}

void DrawTriangle(const Triangle& triangle, Vector3 scaleCamera, Vector3 rotateCamera, Vector3 translateCamera, int color,
	Vector3 scale, Vector3 rotate, Vector3 translate,
	float width, float height, float fovY, float nearClip, float farClip,
	float left, float top, float minDepth, float maxDepth) {

	Vector3 a = RenderingPipelineVer2(scale, rotate, translate, scaleCamera, rotateCamera, translateCamera, triangle.vertices[0], width, height, fovY, nearClip, farClip, left, top, minDepth, maxDepth);
	Vector3 b = RenderingPipelineVer2(scale, rotate, translate, scaleCamera, rotateCamera, translateCamera, triangle.vertices[1], width, height, fovY, nearClip, farClip, left, top, minDepth, maxDepth);
	Vector3 c = RenderingPipelineVer2(scale, rotate, translate, scaleCamera, rotateCamera, translateCamera, triangle.vertices[2], width, height, fovY, nearClip, farClip, left, top, minDepth, maxDepth);

	Novice::DrawTriangle(static_cast<int>(a.x), static_cast<int>(a.y),
		static_cast<int>(b.x), static_cast<int>(b.y),
		static_cast<int>(c.x), static_cast<int>(c.y), color, kFillModeWireFrame);
}

//void DrawTriangle(const Triangle& triangle, Matrix4x4& viewProjectionMatrix, Matrix4x4& viewPortMatrix, int color) {
//	Vector3 ndcVertex = Transform(triangle.vertices[0], viewProjectionMatrix);
//	Vector3 screenVertex = Transform(ndcVertex, viewPortMatrix);
//	Vector3 a = { screenVertex.x,screenVertex.y,0.0f };
//	ndcVertex = Transform(triangle.vertices[1], viewProjectionMatrix);
//	screenVertex = Transform(ndcVertex, viewPortMatrix);
//	Vector3 b = { screenVertex.x,screenVertex.y,0.0f };
//	ndcVertex = Transform(triangle.vertices[2], viewProjectionMatrix);
//	screenVertex = Transform(ndcVertex, viewPortMatrix);
//	Vector3 c = { screenVertex.x,screenVertex.y,0.0f };
//
//	Novice::DrawTriangle(static_cast<int>(a.x), static_cast<int>(a.y),
//		static_cast<int>(b.x), static_cast<int>(b.y),
//		static_cast<int>(c.x), static_cast<int>(c.y), color, kFillModeSolid);
//}

void DrawAABB(const AABB& a, Vector3 scaleCamera, Vector3 rotateCamera, Vector3 translateCamera, int color,
	Vector3 scale, Vector3 rotate, Vector3 translate,
	float width, float height, float fovY, float nearClip, float farClip,
	float left, float top, float minDepth, float maxDepth) {
	Vector3 vertices[8] = {
		{ a.min.x, a.min.y, a.min.z },// 手前左下
		{ a.max.x, a.min.y, a.min.z },// 手前右下
		{ a.max.x, a.max.y, a.min.z },// 手前右上
		{ a.min.x, a.max.y, a.min.z },// 手前左上
		{ a.min.x, a.min.y, a.max.z },// 奥左下
		{ a.max.x, a.min.y, a.max.z },// 奥右下
		{ a.max.x, a.max.y, a.max.z },// 奥右上
		{ a.min.x, a.max.y, a.max.z } // 奥左上
	};
	for (int i = 0; i < 8; ++i) {
		vertices[i] = RenderingPipelineVer2(scale, rotate, translate, scaleCamera, rotateCamera, translateCamera, vertices[i], width, height, fovY, nearClip, farClip, left, top, minDepth, maxDepth);
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