#include "debugView.h"
#include <Novice.h>

void VectorScreenPrintf(int x, int y, const Vector3& vector, const char* lavel) {
	Novice::ScreenPrintf(x, y, "%.02f", vector.x);
	Novice::ScreenPrintf(x + kColumnWidth, y, "%.02f", vector.y);
	Novice::ScreenPrintf(x + kColumnWidth * 2, y, "%.02f", vector.z);
	Novice::ScreenPrintf(x + kColumnWidth * 3, y, "%s", lavel);
}

void MatrixScreenPrintf(int x, int y, const Matrix4x4& matrix, const char* lavel) {
	Novice::ScreenPrintf(x, y, "%s", lavel);
	y += kRowHeight;
	for (int row = 0;row < 4;++row) {
		for (int column = 0;column < 4;++column) {
			Novice::ScreenPrintf(
				x + column * kColumnWidth, y + row * kRowHeight, "%6.03f", matrix.m[row][column]
			);
		}
	}
}

Camera DebugCamera(Camera& camera,int &prePositionX,int &prePositionY) {
	int positionX;
	int positionY;
	Novice::GetMousePosition(&positionX, &positionY);
	// マウスのホイールを押していたら回転
	if (Novice::IsPressMouse(2)) {
		camera.rotate.y -= float(prePositionX - positionX) / 180.0f;
		camera.rotate.x -= float(prePositionY - positionY) / 180.0f;
	}
	else {
		//マウスのホイールでカメラを引く
		camera.scale.z += float(Novice::GetWheel()) / 500.0f;
	}
	// マウスのクリック二つ押しで移動
	if (Novice::IsPressMouse(0) && Novice::IsPressMouse(1)) {
		camera.translate.x += float(prePositionX - positionX) / 10.0f;
		camera.translate.y += float(prePositionY - positionY) / 10.0f;
	}
	prePositionX = positionX;
	prePositionY = positionY;
	return camera;
}