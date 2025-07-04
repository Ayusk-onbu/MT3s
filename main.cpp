#include <Novice.h>
#include "matrix4x4Calculation.h"
#include "vector3Calculation.h"
#include "OBBCalculation.h"
#include "sphereCalculation.h"
#include "planeCalculation.h"
#include "TriangleCalculation.h"
#include "AABBCalculation.h"
#include "drawSeries.h"
#include "Bezier.h"
#include "debugView.h"
#include "functions.h"

const char kWindowTitle[] = "LE2A_10_ハマダ_カズヤ_MT3";

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	Vector3 scaleCamera = { 1.0f,1.0f,0.250f };
	Vector3 rotateCamera = { 0.26f,0.0f,0.0f };
	Vector3 translateCamera = { 0.0f,1.9f,-6.49f };
	Camera debugCamera = { scaleCamera,rotateCamera,translateCamera };
	Matrix4x4 viewProjectionMatrix = MakeViewProjectionMatrix({ 1.0f,1.0f,1.0f }, { 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f }, scaleCamera, rotateCamera, translateCamera);
	Matrix4x4 viewportMatrix = MakeViewportMatrix(0.0f, 0.0f, 1280.0f, 720.0f, 0.0f, 0.0f);

	Segment segment{
		.origin = { -0.7f, 0.3f, 0.0f },
		.diff = { 2.0f, -0.5f, 0.0f }
	};
	Vector3 start = Transform(Transform(segment.origin, viewProjectionMatrix), viewportMatrix);
	Vector3 end = Transform(Transform(Add(segment.origin, segment.diff), viewProjectionMatrix), viewportMatrix);


	const int kSphereNum = 3;
	Sphere sphere[kSphereNum] = {};
	for (int i = 0;i < kSphereNum;++i) {
		sphere[i].center = Vector3(0.0f, 0.0f, 0.0f);
		sphere[i].radius = 0.1f * (1 + 0);
		sphere[i].color = 0x000000FF;
	}

	Vector3 a{ 0.2f, 1.0f, 0.0f };
	Vector3 b{ 2.4f,3.1f,1.2f };
	Vector3 c = a + b;
	Vector3 d = a - b;
	Vector3 e = a * 2.4f;
	Vector3 rotate{ 0.4f,1.43f,-0.8f };
	Matrix4x4 rotateXMatrix = MakeRotateXMatrix(rotate.x);
	Matrix4x4 rotateYMatrix = MakeRotateYMatrix(rotate.y);
	Matrix4x4 rotateZMatrix = MakeRotateZMatrix(rotate.z);
	Matrix4x4 rotateMatrix = rotateXMatrix * rotateYMatrix * rotateZMatrix;
	
	bool isViewSphere = true;
	bool isDebugCamera = false;
	int preCameraPosX = 0;
	int preCameraPosY = 0;

	//int color = 0x0000FFFF;

	// キー入力結果を受け取る箱
	char keys[256] = {0};
	char preKeys[256] = {0};

	// ウィンドウの×ボタンが押されるまでループ
	while (Novice::ProcessMessage() == 0) {
		// フレームの開始
		Novice::BeginFrame();

		// キー入力を受け取る
		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);

		///
		/// ↓更新処理ここから
		///



		///
		/// ↑更新処理ここまで
		///

		ImGui::Begin("Window");
		ImGui::Text("c:%f, %f, %f", c.x, c.y, c.z);
		ImGui::Text("d:%f, %f, %f", d.x, d.y, d.z);
		ImGui::Text("e:%f, %f, %f", e.x, e.y, e.z);
		ImGui::Text(
			"matrix:\n%f, %f, %f, %f\n%f, %f, %f, %f\n%f, %f, %f, %f\n%f, %f, %f, %f\n",
			rotateMatrix.m[0][0], rotateMatrix.m[0][1], rotateMatrix.m[0][2], rotateMatrix.m[0][3],
			rotateMatrix.m[1][0], rotateMatrix.m[1][1], rotateMatrix.m[1][2], rotateMatrix.m[1][3],
			rotateMatrix.m[2][0], rotateMatrix.m[2][1], rotateMatrix.m[2][2], rotateMatrix.m[2][3],
			rotateMatrix.m[3][0], rotateMatrix.m[3][1], rotateMatrix.m[3][2], rotateMatrix.m[3][3]
		);
		ImGui::End();

		///
		/// ↓描画処理ここから
		///
		if (isViewSphere) {
			for (int i = 0;i < kSphereNum;++i) {
				if (isDebugCamera) {
					//DrawSphere(sphere[i], debugCamera.scale, debugCamera.rotate, debugCamera.translate, sphere[i].color);
					//
					//DrawSphere(sphere[i], debugCamera.scale, debugCamera.rotate, debugCamera.translate, color);
				}
				else {
					//DrawSphere(sphere[i], scaleCamera, rotateCamera, translateCamera, sphere[i].color);
					//DrawSphere(sphere[i], scaleCamera, rotateCamera, translateCamera, color);
				}
			}
		}
		if (isDebugCamera) {
			DrawGrid(debugCamera.scale, debugCamera.rotate, debugCamera.translate);
			
			viewProjectionMatrix = MakeViewProjectionMatrix({ 1.0f,1.0f,1.0f }, { 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f }, debugCamera.scale, debugCamera.rotate, debugCamera.translate);
		}
		else {
			DrawGrid(scaleCamera, rotateCamera, translateCamera);

			viewProjectionMatrix = MakeViewProjectionMatrix({ 1.0f,1.0f,1.0f }, { 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f }, scaleCamera, rotateCamera, translateCamera);
		}

		if (isDebugCamera) {
			DebugCamera(debugCamera,preCameraPosX,preCameraPosY);
			if (keys[DIK_RETURN]&& preKeys[DIK_RETURN] == false) {
				isDebugCamera = false;
			}
		}
		else {
			if (keys[DIK_RETURN] && preKeys[DIK_RETURN] == false) {
				isDebugCamera = true;
			}
		}

#pragma region ImGui
		ImGui::Begin("Debug");

		

		ImGui::DragFloat3("CameraTranslate", &translateCamera.x, 0.01f);
		ImGui::DragFloat3("CameraRotate", &rotateCamera.x, 0.01f);
		ImGui::DragFloat3("sphereCenter", &sphere[0].center.x, 0.01f);
		ImGui::DragFloat("sphereRadius", &sphere[0].radius, 0.01f);
		ImGui::DragFloat3("segmentOrijin", &segment.origin.x, 0.01f);
		ImGui::DragFloat3("segmentDiff", &segment.diff.x, 0.01f);

		
		//ImGui::InputFloat3("Project", &project.x, "%.3f", ImGuiInputTextFlags_ReadOnly);
		ImGui::End();
#pragma endregion
		///
		/// ↑描画処理ここまで
		///
		

		start = Transform(Transform(segment.origin, viewProjectionMatrix), viewportMatrix);
		end = Transform(Transform(Add(segment.origin, segment.diff), viewProjectionMatrix), viewportMatrix);


		// フレームの終了
		Novice::EndFrame();

		// ESCキーが押されたらループを抜ける
		if (preKeys[DIK_ESCAPE] == 0 && keys[DIK_ESCAPE] != 0) {
			break;
		}
	}

	// ライブラリの終了
	Novice::Finalize();
	return 0;
}

void ooTwo() {
	/*Segment segment = { {-2.0f,-1.0f,0.0f},{3.0f,2.0f,2.0f} };
	Vector3 point = { -1.5f,0.6f,0.6f };
	Vector3 project = Project(Subtract(point, segment.origin), segment.diff);
	Vector3 closestPoint = ClosestPoint(point, segment);
	Sphere pointSphere = { point, 0.01f };
	Sphere closestPointSphere = { closestPoint, 0.01f };
	Vector3 start = Transform(Transform(segment.origin, viewProjectionMatrix), viewportMatrix);
	Vector3 end = Transform(Transform(Add(segment.origin, segment.diff), viewProjectionMatrix), viewportMatrix);

	DrawSphere(pointSphere, scaleCamera, rotateCamera, translateCamera, RED);
	DrawSphere(closestPointSphere, scaleCamera, rotateCamera, translateCamera, BLACK);
	Novice::DrawLine(static_cast<int>(start.x), static_cast<int>(start.y),
		static_cast<int>(end.x), static_cast<int>(end.y), 0xFFffffFF);*/
}
void ooThree() {
	/*const int kPlaneNum = 1;
	Plane plane[kPlaneNum];
	for (int i = 0;i < kPlaneNum;++i) {
		plane[i].normal = { 0.0f,-1.0f,0.0f };
		plane[i].distance = -3;
	}
	int planeColor = 0xFFFFFFFF;

	if (isViewSphere) {
		if (IsPlaneHitPoint(plane[0], sphere[0].center, sphere[0].radius)) {
			sphere[0].color = 0x00FF00FF;
			planeColor = 0x00FF00FF;
		}
		else {
			sphere[0].color = 0xFFFFFFFF;
			planeColor = 0xFFFFFFFF;
		}
	}
	if (IsHitPlane2Segment(plane[0], segment, viewProjectionMatrix, viewportMatrix)) {
		color = 0x0000FFFF;
	}
	else {
		color = 0xffffffff;
	}

	DrawPlane(plane[0], debugCamera.scale, debugCamera.rotate, debugCamera.translate, planeColor);
	DrawPlane(plane[0], scaleCamera, rotateCamera, translateCamera, planeColor);

	for (int i = 0;i < kPlaneNum;++i) {
		plane[i].normal = Normalize(plane[i].normal);
	}*/
}
void ooFour() {
	/*Triangle triangle = {
		{ { 0.0f, 1.73f, 0.0f },{ 1.0f, 0.0f, 0.0f },{ -1.0f, 0.0f, 0.0f }  }
	};
	if (IsHitTriangle2Segment(triangle, segment)) {
		color = 0x0000FFFF;
	}
	else {
		color = 0xFFFFFFFF;
	}
	DrawTriangle(triangle, debugCamera.scale, debugCamera.rotate, debugCamera.translate, color);
	DrawTriangle(triangle, scaleCamera, rotateCamera, translateCamera, color);
	ImGui::DragFloat3("TriangleTranslation[0]", &triangle.vertices[0].x, -1.0f, 1.0f);
	ImGui::DragFloat3("TriangleTranslation[1]", &triangle.vertices[1].x, -1.0f, 1.0f);
	ImGui::DragFloat3("TriangleTranslation[2]", &triangle.vertices[2].x, -1.0f, 1.0f);*/

}
void ooFive() {
	/*AABB aabb1{
		.min = {-0.5f,-0.5f,-0.5f},
		.max = {0.5f,0.5f,0.5f}
	};
	if (IsHitAABB2Segment(aabb1, segment)) {
		color = 0xFF0000FF;
	}
	else {
		color = 0xFFFFFFFF;
	}
	DrawAABB(aabb1, debugCamera.scale, debugCamera.rotate, debugCamera.translate, color);
	DrawAABB(aabb1, scaleCamera, rotateCamera, translateCamera, color);
	ImGui::SliderFloat3("AABBmin", &aabb1.min.x, -5, 5);
	ImGui::SliderFloat3("AABBmax", &aabb1.max.x, -5, 5);
	aabb1.min.x = (std::min)(aabb1.min.x, aabb1.max.x);
	aabb1.max.x = (std::max)(aabb1.min.x, aabb1.max.x);
	aabb1.min.y = (std::min)(aabb1.min.y, aabb1.max.y);
	aabb1.max.y = (std::max)(aabb1.min.y, aabb1.max.y);
	aabb1.min.z = (std::min)(aabb1.min.z, aabb1.max.z);
	aabb1.max.z = (std::max)(aabb1.min.z, aabb1.max.z);*/
}
