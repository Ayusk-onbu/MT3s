#include "matrix4x4Calculation.h"
#include "functions.h"

//行列の加法
Matrix4x4 Add(const Matrix4x4& m1, const Matrix4x4 m2) {
	float a11 = m1.m[0][0], a12 = m1.m[0][1], a13 = m1.m[0][2], a14 = m1.m[0][3],
		a21 = m1.m[1][0], a22 = m1.m[1][1], a23 = m1.m[1][2], a24 = m1.m[1][3],
		a31 = m1.m[2][0], a32 = m1.m[2][1], a33 = m1.m[2][2], a34 = m1.m[2][3],
		a41 = m1.m[3][0], a42 = m1.m[3][1], a43 = m1.m[3][2], a44 = m1.m[3][3];
	float b11 = m2.m[0][0], b12 = m2.m[0][1], b13 = m2.m[0][2], b14 = m2.m[0][3],
		b21 = m2.m[1][0], b22 = m2.m[1][1], b23 = m2.m[1][2], b24 = m2.m[1][3],
		b31 = m2.m[2][0], b32 = m2.m[2][1], b33 = m2.m[2][2], b34 = m2.m[2][3],
		b41 = m2.m[3][0], b42 = m2.m[3][1], b43 = m2.m[3][2], b44 = m2.m[3][3];
	Matrix4x4 ret = {
		a11 + b11,a12 + b12,a13 + b13,a14 + b14,
		a21 + b21,a22 + b22,a23 + b23,a24 + b24,
		a31 + b31,a32 + b32,a33 + b33,a34 + b34,
		a41 + b41,a42 + b42,a43 + b43,a44 + b44
	};
	return ret;
}
//行列の減法
Matrix4x4 Subtract(const Matrix4x4& m1, const Matrix4x4 m2) {
	float a11 = m1.m[0][0], a12 = m1.m[0][1], a13 = m1.m[0][2], a14 = m1.m[0][3],
		a21 = m1.m[1][0], a22 = m1.m[1][1], a23 = m1.m[1][2], a24 = m1.m[1][3],
		a31 = m1.m[2][0], a32 = m1.m[2][1], a33 = m1.m[2][2], a34 = m1.m[2][3],
		a41 = m1.m[3][0], a42 = m1.m[3][1], a43 = m1.m[3][2], a44 = m1.m[3][3];
	float b11 = m2.m[0][0], b12 = m2.m[0][1], b13 = m2.m[0][2], b14 = m2.m[0][3],
		b21 = m2.m[1][0], b22 = m2.m[1][1], b23 = m2.m[1][2], b24 = m2.m[1][3],
		b31 = m2.m[2][0], b32 = m2.m[2][1], b33 = m2.m[2][2], b34 = m2.m[2][3],
		b41 = m2.m[3][0], b42 = m2.m[3][1], b43 = m2.m[3][2], b44 = m2.m[3][3];
	Matrix4x4 ret = {
		a11 - b11,a12 - b12,a13 - b13,a14 - b14,
		a21 - b21,a22 - b22,a23 - b23,a24 - b24,
		a31 - b31,a32 - b32,a33 - b33,a34 - b34,
		a41 - b41,a42 - b42,a43 - b43,a44 - b44
	};
	return ret;
}
//行列の積
Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4 m2) {
	float a11 = m1.m[0][0], a12 = m1.m[0][1], a13 = m1.m[0][2], a14 = m1.m[0][3],
		a21 = m1.m[1][0], a22 = m1.m[1][1], a23 = m1.m[1][2], a24 = m1.m[1][3],
		a31 = m1.m[2][0], a32 = m1.m[2][1], a33 = m1.m[2][2], a34 = m1.m[2][3],
		a41 = m1.m[3][0], a42 = m1.m[3][1], a43 = m1.m[3][2], a44 = m1.m[3][3];
	float b11 = m2.m[0][0], b12 = m2.m[0][1], b13 = m2.m[0][2], b14 = m2.m[0][3],
		b21 = m2.m[1][0], b22 = m2.m[1][1], b23 = m2.m[1][2], b24 = m2.m[1][3],
		b31 = m2.m[2][0], b32 = m2.m[2][1], b33 = m2.m[2][2], b34 = m2.m[2][3],
		b41 = m2.m[3][0], b42 = m2.m[3][1], b43 = m2.m[3][2], b44 = m2.m[3][3];
	Matrix4x4 ret = {
		a11 * b11 + a12 * b21 + a13 * b31 + a14 * b41,  a11 * b12 + a12 * b22 + a13 * b32 + a14 * b42,  a11 * b13 + a12 * b23 + a13 * b33 + a14 * b43,  a11 * b14 + a12 * b24 + a13 * b34 + a14 * b44,
		a21 * b11 + a22 * b21 + a23 * b31 + a24 * b41,  a21 * b12 + a22 * b22 + a23 * b32 + a24 * b42,  a21 * b13 + a22 * b23 + a23 * b33 + a24 * b43,  a21 * b14 + a22 * b24 + a23 * b34 + a24 * b44,
		a31 * b11 + a32 * b21 + a33 * b31 + a34 * b41,  a31 * b12 + a32 * b22 + a33 * b32 + a34 * b42,  a31 * b13 + a32 * b23 + a33 * b33 + a34 * b43,  a31 * b14 + a32 * b24 + a33 * b34 + a34 * b44,
		a41 * b11 + a42 * b21 + a43 * b31 + a44 * b41,  a41 * b12 + a42 * b22 + a43 * b32 + a44 * b42,  a41 * b13 + a42 * b23 + a43 * b33 + a44 * b43,  a41 * b14 + a42 * b24 + a43 * b34 + a44 * b44
	};
	return ret;
}
//行列の逆
Matrix4x4 Inverse(const Matrix4x4& m) {
	float a11 = m.m[0][0], a12 = m.m[0][1], a13 = m.m[0][2], a14 = m.m[0][3],
		a21 = m.m[1][0], a22 = m.m[1][1], a23 = m.m[1][2], a24 = m.m[1][3],
		a31 = m.m[2][0], a32 = m.m[2][1], a33 = m.m[2][2], a34 = m.m[2][3],
		a41 = m.m[3][0], a42 = m.m[3][1], a43 = m.m[3][2], a44 = m.m[3][3];
	float A = a11 * a22 * a33 * a44 + a11 * a23 * a34 * a42 + a11 * a24 * a32 * a43
		- a11 * a24 * a33 * a42 - a11 * a23 * a32 * a44 - a11 * a22 * a34 * a43
		- a12 * a21 * a33 * a44 - a13 * a21 * a34 * a42 - a14 * a21 * a32 * a43
		+ a14 * a21 * a33 * a42 + a13 * a21 * a32 * a44 + a12 * a21 * a34 * a43
		+ a12 * a23 * a31 * a44 + a13 * a24 * a31 * a42 + a14 * a22 * a31 * a43
		- a14 * a23 * a31 * a42 - a13 * a22 * a31 * a44 - a12 * a24 * a31 * a43
		- a12 * a23 * a34 * a41 - a13 * a24 * a32 * a41 - a14 * a22 * a33 * a41
		+ a14 * a23 * a32 * a41 + a13 * a22 * a34 * a41 + a12 * a24 * a33 * a41;
	Matrix4x4 ret = {
		(a22 * a33 * a44 + a23 * a34 * a42 + a24 * a32 * a43 - a24 * a33 * a42 - a23 * a32 * a44 - a22 * a34 * a43) / A,
		(-a12 * a33 * a44 - a13 * a34 * a42 - a14 * a32 * a43 + a14 * a33 * a42 + a13 * a32 * a44 + a12 * a34 * a43) / A,
		(a12 * a23 * a44 + a13 * a24 * a42 + a14 * a22 * a43 - a14 * a23 * a42 - a13 * a22 * a44 - a12 * a24 * a43) / A,
		(-a12 * a23 * a34 - a13 * a24 * a32 - a14 * a22 * a33 + a14 * a23 * a32 + a13 * a22 * a34 + a12 * a24 * a33) / A,

		(-a21 * a33 * a44 - a23 * a34 * a41 - a24 * a31 * a43 + a24 * a33 * a41 + a23 * a31 * a44 + a21 * a34 * a43) / A,
		(a11 * a33 * a44 + a13 * a34 * a41 + a14 * a31 * a43 - a14 * a33 * a41 - a13 * a31 * a44 - a11 * a34 * a43) / A,
		(-a11 * a23 * a44 - a13 * a24 * a41 - a14 * a21 * a43 + a14 * a23 * a41 + a13 * a21 * a44 + a11 * a24 * a43) / A,
		(a11 * a23 * a34 + a13 * a24 * a31 + a14 * a21 * a33 - a14 * a23 * a31 - a13 * a21 * a34 - a11 * a24 * a33) / A,

		(a21 * a32 * a44 + a22 * a34 * a41 + a24 * a31 * a42 - a24 * a32 * a41 - a22 * a31 * a44 - a21 * a34 * a42) / A,
		(-a11 * a32 * a44 - a12 * a34 * a41 - a14 * a31 * a42 + a14 * a32 * a41 + a12 * a31 * a44 + a11 * a34 * a42) / A,
		(a11 * a22 * a44 + a12 * a24 * a41 + a14 * a21 * a42 - a14 * a22 * a41 - a12 * a21 * a44 - a11 * a24 * a42) / A,
		(-a11 * a22 * a34 - a12 * a24 * a31 - a14 * a21 * a32 + a14 * a22 * a31 + a12 * a21 * a34 + a11 * a24 * a32) / A,

		(-a21 * a32 * a43 - a22 * a33 * a41 - a23 * a31 * a42 + a23 * a32 * a41 + a22 * a31 * a43 + a21 * a33 * a42) / A,
		(a11 * a32 * a43 + a12 * a33 * a41 + a13 * a31 * a42 - a13 * a32 * a41 - a12 * a31 * a43 - a11 * a33 * a42) / A,
		(-a11 * a22 * a43 - a12 * a23 * a41 - a13 * a21 * a42 + a13 * a22 * a41 + a12 * a21 * a43 + a11 * a23 * a42) / A,
		(a11 * a22 * a33 + a12 * a23 * a31 + a13 * a21 * a32 - a13 * a22 * a31 - a12 * a21 * a33 - a11 * a23 * a32) / A
	};
	return ret;
}
//行列の転置
Matrix4x4 Transpose(const Matrix4x4& m) {
	float a11 = m.m[0][0], a12 = m.m[0][1], a13 = m.m[0][2], a14 = m.m[0][3],
		a21 = m.m[1][0], a22 = m.m[1][1], a23 = m.m[1][2], a24 = m.m[1][3],
		a31 = m.m[2][0], a32 = m.m[2][1], a33 = m.m[2][2], a34 = m.m[2][3],
		a41 = m.m[3][0], a42 = m.m[3][1], a43 = m.m[3][2], a44 = m.m[3][3];
	Matrix4x4 ret = {
		a11,a21,a31,a41,
		a12,a22,a32,a42,
		a13,a23,a33,a43,
		a14,a24,a34,a44
	};
	return ret;
}
//行列の単位
Matrix4x4 MakeIdentity4x4() {
	Matrix4x4 ret = {
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1
	};
	return ret;
}

Matrix4x4 MakeTranslateMatrix(const Vector3& translate) {
	Matrix4x4 ret = {
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		translate.x,translate.y,translate.z,1
	};
	return ret;
}

Matrix4x4 MakeScaleMatrix(const Vector3& scale) {
	Matrix4x4 ret = {
		scale.x,0,0,0,
		0,scale.y,0,0,
		0,0,scale.z,0,
		0,0,0,1
	};
	return ret;
}

Vector3 Transform(const Vector3& vector, const Matrix4x4& matrix) {
	Vector3 ret;
	ret.x = vector.x * matrix.m[0][0] + vector.y * matrix.m[1][0]
		+ vector.z * matrix.m[2][0] + 1.0f * matrix.m[3][0];
	ret.y = vector.x * matrix.m[0][1] + vector.y * matrix.m[1][1]
		+ vector.z * matrix.m[2][1] + 1.0f * matrix.m[3][1];
	ret.z = vector.x * matrix.m[0][2] + vector.y * matrix.m[1][2]
		+ vector.z * matrix.m[2][2] + 1.0f * matrix.m[3][2];
	float W = vector.x * matrix.m[0][3] + vector.y * matrix.m[1][3]
		+ vector.z * matrix.m[2][3] + 1.0f * matrix.m[3][3];
	assert(W != 0.0f);
	ret.x /= W;
	ret.y /= W;
	ret.z /= W;
	return ret;
}

//ｘ軸の回転
Matrix4x4 MakeRotateXMatrix(float radian) {
	Matrix4x4 ret = {
		1,0,0,0,
		0,std::cos(radian),std::sin(radian),0,
		0,-std::sin(radian),std::cos(radian),0,
		0,0,0,1
	};
	return ret;
}
//ｙ軸の回転
Matrix4x4 MakeRotateYMatrix(float radian) {
	Matrix4x4 ret = {
		std::cos(radian),0,-std::sin(radian),0,
		0,1,0,0,
		std::sin(radian),0,std::cos(radian),0,
		0,0,0,1
	};
	return ret;
}
//ｚ軸の回転
Matrix4x4 MakeRotateZMatrix(float radian) {
	Matrix4x4 ret = {
		std::cos(radian),std::sin(radian),0,0,
		-std::sin(radian),std::cos(radian),0,0,
		0,0,1,0,
		0,0,0,1
	};
	return ret;
}
Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate) {
	//SRT
	// - [ S ] - //
	Matrix4x4 SRTMatrix = Multiply(MakeScaleMatrix(scale), Multiply(MakeRotateXMatrix(rotate.x), Multiply(MakeRotateYMatrix(rotate.y), MakeRotateZMatrix(rotate.z))));
	SRTMatrix = Multiply(SRTMatrix, MakeTranslateMatrix(translate));
	return SRTMatrix;
}

//透視投影行列
Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip) {
	Matrix4x4 ret = {
		cot(fovY / 2.0f) / aspectRatio, 0, 0, 0,
		0, cot(fovY / 2.0f), 0, 0,
		0, 0, farClip / (farClip - nearClip), 1,
		0, 0, (-nearClip * farClip) / (farClip - nearClip), 0
	};
	return ret;
}

//正射影行列
Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip) {
	Matrix4x4 ret = {
		2.0f / (right - left), 0, 0, 0,
		0, 2.0f / (top - bottom), 0, 0,
		0, 0, 1.0f / (farClip - nearClip), 0,
		(left + right) / (left - right), (top + bottom) / (bottom - top), nearClip / (nearClip - farClip),1
	};
	return ret;
}

//ビューポート変換行列
Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth) {
	Matrix4x4 ret = {
		width / 2.0f, 0, 0, 0,
		0, -height / 2.0f, 0, 0,
		0, 0, maxDepth - minDepth, 0,
		left + width / 2.0f, top + height / 2.0f, minDepth, 1
	};
	return ret;
}

//クロス積（外積）
Vector3 CrossProduct(const Vector3& v1, const Vector3& v2) {
	Vector3 ret;
	ret.x = v1.y * v2.z - v1.z * v2.y;
	ret.y = v1.z * v2.x - v1.x * v2.z;
	ret.z = v1.x * v2.y - v1.y * v2.x;
	return ret;
}

//内積
float DotProduct(const Vector3& v1, const Vector3& v2) {
	float ret = v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
	return ret;
}
