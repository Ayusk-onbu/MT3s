#include <Novice.h>
#include "matrix4x4Calculation.h"
#include "debugView.h"

const char kWindowTitle[] = "LE2A_10_ハマダ_カズヤ_MT3";

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	Vector3 v1{ 1.2f,-3.9f,2.5f };
	Vector3 v2{ 2.8f,0.4f,-1.3f };
	Vector3 cross = CrossProduct(v1, v2);

	Vector3 scale = { 1.0f,1.0f,1.0f };
	Vector3 rotate = { 0.0f,0.0f,0.0f };
	Vector3 translate = { 0.0f,0.0f,0.0f };

	Vector3 kLocalVertices[3] = {
		{ 0.0f, 1.0f, 0.0f },
		{ 1.0f, -1.0f, 0.0f},
		{-1.0f, -1.0f, 0.0f},
	};

	Vector3 scaleCamera = { 1.0f,1.0f,1.0f };
	Vector3 rotateCamera = { 0.0f,0.0f,0.0f };
	Vector3 translateCamera = { 0.0f,0.0f,-5.0f };

	Vector3 screenVertices[3];
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

		if (keys[DIK_W]) {
			translate.z += 0.1f;
		}

		if (keys[DIK_S]) {
			translate.z -= 0.1f;
		}

		if (keys[DIK_A]) {
			translate.x -= 0.01f;
		}

		if (keys[DIK_D]) {
			translate.x += 0.01f;
		}

		//Y軸の回転
		rotate.y += 0.1f;

		for (uint32_t i = 0;i < 3;++i) {
			screenVertices[i] = RenderingPipelineVer2(scale, rotate, translate, scaleCamera, rotateCamera, translateCamera, kLocalVertices[i]);
		}

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///
		VectorScreenPrintf(0, 0, cross, "Cross");
		Novice::DrawTriangle(static_cast<int>(screenVertices[0].x),
			                 static_cast<int>(screenVertices[0].y),
			                 static_cast<int>(screenVertices[1].x),
			                 static_cast<int>(screenVertices[1].y),
			                 static_cast<int>(screenVertices[2].x),
			                 static_cast<int>(screenVertices[2].y),
			                 0xFF0000FF, kFillModeSolid
		);
		///
		/// ↑描画処理ここまで
		///

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
