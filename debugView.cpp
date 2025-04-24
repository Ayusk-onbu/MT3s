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
				x + column * kColumnWidth, y + row * kRowHeight, "%6.02f", matrix.m[row][column]
			);
		}
	}
}