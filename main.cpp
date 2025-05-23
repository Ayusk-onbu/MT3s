#include <Novice.h>
#include "matrix4x4Calculation.h"
#include "vector3Calculation.h"
#include "sphereCalculation.h"
#include "planeCalculation.h"
#include "drawSeries.h"
#include "debugView.h"

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

	const int kPlaneNum = 1;
	Plane plane[kPlaneNum];
	for (int i = 0;i < kPlaneNum;++i) {
		plane[i].normal = { 0.0f,-1.0f,0.0f };
		plane[i].distance = -3;
	}
	int planeColor = 0xFFFFFFFF;
	const int kSphereNum = 2;
	Sphere sphere[kSphereNum] = {};
	for (int i = 0;i < kSphereNum;++i) {
		sphere[i].center = {0.0f + i,0.0f + i,0.0f + i};
		sphere[i].radius = 1.0f * (1 + i);
		sphere[i].color = 0xFFFFFFFF;
	}

	bool isViewSphere = true;
	bool isDebugCamera = false;
	int preCameraPosX = 0;
	int preCameraPosY = 0;
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
#pragma region CameraControl
#pragma endregion
		///
		/// ↑更新処理ここまで
		///

		/*if (SphereHitSphere(sphere[0], sphere[1])) {
			sphere[0].color = 0xFF0000Ff;
			sphere[1].color = 0xFF0000Ff;
		}*/
		if (IsPlaneHitPoint(plane[0],sphere[0].center,sphere[0].radius)) {
			sphere[0].color = 0x00FF00FF;
			planeColor = 0x00FF00FF;
		}
		else {
			sphere[0].color = 0xFFFFFFFF;
			planeColor = 0xFFFFFFFF;
		}

		///
		/// ↓描画処理ここから
		///
		if (isViewSphere) {
			for (int i = 0;i < kSphereNum - 1;++i) {
				if (isDebugCamera) {
					DrawSphere(sphere[i], debugCamera.scale, debugCamera.rotate, debugCamera.translate, sphere[i].color);
				}
				else {
					DrawSphere(sphere[i], scaleCamera, rotateCamera, translateCamera, sphere[i].color);
				}
			}
		}
		if (isDebugCamera) {
			DrawPlane(plane[0], debugCamera.scale, debugCamera.rotate, debugCamera.translate,planeColor);
			DrawGrid(debugCamera.scale, debugCamera.rotate, debugCamera.translate);
		}
		else {
			DrawPlane(plane[0], scaleCamera, rotateCamera, translateCamera,planeColor);
			DrawGrid(scaleCamera, rotateCamera, translateCamera);
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
		ImGui::DragFloat3("PlaneNormal", &plane[0].normal.x, 0.1f,-5.0f, 5.0f);
		ImGui::DragFloat("PlaneDistance", &plane[0].distance, 0.1f, -10, 10);
		//ImGui::DragFloat3("Point", &point.x, 0.01f);
		//ImGui::DragFloat3("Segment.origin", &segment.origin.x, 0.01f);
		//ImGui::DragFloat3("Segment.diff", &segment.diff.x, 0.01f);

		//ImGui::InputFloat3("Project", &project.x, "%.3f", ImGuiInputTextFlags_ReadOnly);
		ImGui::End();
#pragma endregion
		///
		/// ↑描画処理ここまで
		///
		Novice::ScreenPrintf(10, 70, "normal.radius %f", sphere[0].radius);

		for (int i = 0;i < kPlaneNum;++i) {
			plane[i].normal = Normalize(plane[i].normal);
		}

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
