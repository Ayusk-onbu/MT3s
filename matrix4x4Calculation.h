#pragma once
#include "structures.h"

//行列の加法
Matrix4x4 Add(const Matrix4x4& m1, const Matrix4x4 m2);
//行列の減法
Matrix4x4 Subtract(const Matrix4x4& m1, const Matrix4x4 m2);
//行列の積
Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4 m2);
//行列の逆
Matrix4x4 Inverse(const Matrix4x4& m);
//行列の転置
Matrix4x4 Transpose(const Matrix4x4& m);
//行列の単位
Matrix4x4 MakeIdentity4x4();

//平行移動
Matrix4x4 MakeTranslateMatrix(const Vector3& translate);
//拡大縮小
Matrix4x4 MakeScaleMatrix(const Vector3& scale);
//座標変換
Vector3 Transform(const Vector3& vector, const Matrix4x4& matrix);

//ｘ軸の回転
Matrix4x4 MakeRotateXMatrix(float radian);
//ｙ軸の回転
Matrix4x4 MakeRotateYMatrix(float radian);
//ｚ軸の回転
Matrix4x4 MakeRotateZMatrix(float radian);

/// <summary>
/// アフィン変換行列の作成
/// </summary>
/// <param name="scale"></param>
/// <param name="rotate"></param>
/// <param name="translate"></param>
/// <returns></returns>
Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate);

//透視投影行列
Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip);
//正射影行列
Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip);
//ビューポート変換行列
Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth);

//レンダリングパイプライン
Vector3 RenderingPipelineVer2(Vector3 scale, Vector3 rotate, Vector3 translate, 
	Vector3 scaleCamera, Vector3 rotateCamera, Vector3 translateCamera, 
	Vector3 localVertex,float width = 1280.0f,float height = 720.0f, float fovY = 0.45f, float nearClip = 0.1f,float farClip = 100.0f,
	float left = 0.0f,float top = 0.0f,float minDepth = 0.0f, float maxDepth = 0.0f);

//クロス積（外積）
Vector3 CrossProduct(const Vector3& v1, const Vector3& v2);

//内積
float DotProduct(const Vector3& v1, const Vector3& v2);