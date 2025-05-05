#include "drawSeries.h"
#include "matrix4x4Calculation.h"
#include <Novice.h>

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

void DrawSphere(const Sphere& sphere, Vector3 scaleCamera, Vector3 rotateCamera, Vector3 translateCamera,
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
				0x000000FF);
			Novice::DrawLine(static_cast<int>(b.x),
				static_cast<int>(b.y),
				static_cast<int>(c.x),
				static_cast<int>(c.y),
				0x000000FF);
		}
	}
}