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

struct SupportPoint {
	Vector2 v;// 差分ベクトル(ShapeB - ShapeA)
	Vector2 supA;// 各形状のサポートポイント(後で衝突点計算に使う場合)
	Vector2 supB;
};

struct Contact {
	Vector2 direction;
	float depth;
};

Vector2 support(std::vector<Vector2>vertices,const Vector2& direction) {
	// 内積が最大の点を探す
	// 保存する変数
	float maxDot = -FLT_MAX;// 最小値からスタート
	Vector2 supportPoint = vertices[0];
	// 全頂点を調べる
	for (const Vector2& vertex : vertices) {
		float dot = vertex.x * direction.x + vertex.y * direction.y;
		if (dot > maxDot) {
			maxDot = dot;
			supportPoint = vertex;
		}
	}
	return supportPoint;
}

float Cross2d(const Vector2& v1, const Vector2& v2) {
	return v1.x * v2.y - v1.y * v2.x;
}

float Dot2d(const Vector2& v1, const Vector2& v2) {
	return v1.x * v2.x + v1.y * v2.y;
}

SupportPoint getSupport(std::vector<Vector2>shapeA, std::vector<Vector2>shapeB, const Vector2& direction) {
	SupportPoint p = {};
	p.supA = support(shapeA, direction);
	p.supB = support(shapeB, { -direction.x,-direction.y });
	p.v = { p.supA.x - p.supB.x, p.supA.y - p.supB.y };
	return p;
}

Vector2 getPerpendicularToOrigin(Vector2 edge, Vector2 toOrigin) {
	// 垂直なベクトルを作成
	Vector2 perp = { -edge.y, edge.x };

	// 原点方向を向いているかを内積で確認
	float dot = perp.x * toOrigin.x + perp.y * toOrigin.y;

	if (dot < 0) {
		// 反転
		perp.x = -perp.x;
		perp.y = -perp.y;
	}

	return perp;
}

Vector2 GetOriginProjection(const Vector2& v1, const Vector2& v2) {
	float a = v1.y - v2.y;
	float b = v2.x - v1.x;
	float c = v1.x * v2.y - v2.x * v1.y;

	// デノミネーター(分母)
	float denom = a * a + b * b;
	// 原点からの最短点を求める
	Vector2 projection = { -a * c / denom, -b * c / denom };
	return projection;
}

float GetDistanceToOrigin(const Vector2& point) {
	return sqrtf(point.x * point.x + point.y * point.y);
}

Vector2 Normalize(const Vector2& v) {
	float length = sqrtf(v.x * v.x + v.y * v.y);
	if (length > 0) {
		return { v.x / length, v.y / length };
	}
	return { 0.0f, 0.0f }; // 長さがゼロの場合はゼロベクトルを返す
}

bool UpdateSimplex(std::vector<SupportPoint>& simplex, Vector2& direction) {
	if (simplex.size() == 2) {
		// 直線のケース
		SupportPoint p = simplex[0];
		SupportPoint r = simplex[1];
		//// 線分PRの作成
		//Vector2 pr = { r.v.x - p.v.x, r.v.y - p.v.y };
		//Vector2 rO = { -r.v.x, -r.v.y };

		//// PRの垂線で原点方向を向くものを次の方向に設定
		//direction = getPerpendicularToOrigin(pr, rO);
		direction = GetOriginProjection(p.v, r.v);
		direction.x = -direction.x;
		direction.y = -direction.y;
		return false;
	}
	else {

		// 三角形の場合
		SupportPoint a = simplex[0];
		SupportPoint b = simplex[1];
		SupportPoint c = simplex[2];

		Vector2 ab = { b.v.x - a.v.x, b.v.y - a.v.y };
		Vector2 bc = { c.v.x - b.v.x, c.v.y - b.v.y };
		Vector2 ca = { a.v.x - c.v.x, a.v.y - c.v.y };

		Vector2 ao = { -a.v.x,-a.v.y };
		Vector2 bo = { -b.v.x,-b.v.y };
		Vector2 co = { -c.v.x,-c.v.y };

		float crossABAO = Cross2d(ab, ao);
		float crossBCBO = Cross2d(bc, bo);
		if (crossABAO * crossBCBO < 0) {
			// 符号が違うのでFalse
			// 最初の頂点を削除
			simplex.erase(simplex.begin());
			direction = GetOriginProjection(b.v, c.v);
			direction.x = -direction.x;
			direction.y = -direction.y;
			return false;
		}
		float crossCACO = Cross2d(ca, co);
		if (crossBCBO * crossCACO < 0) {
			// 符号が違うのでFalse
			// 最初の頂点を削除
			simplex.erase(simplex.begin());
			direction = GetOriginProjection(b.v, c.v);
			direction.x = -direction.x;
			direction.y = -direction.y;
			return false;
		}
		// ここまできたら原点は含まれているのでOK
		return true;
	}
}

Contact EPA(std::vector<Vector2>shapeA, std::vector<Vector2>shapeB, std::vector<SupportPoint>& simplex) {
	const int MAX_ITERATIONS = 30;// 無限ループ防止用
	const float TOLERANCE = 0.0001f;// 許容範囲

	Contact contact = {};

	for (int iter = 0; iter < MAX_ITERATIONS; ++iter) {
		float minDepth = FLT_MAX;
		Vector2 minDirection = { 0.0f,0.0f };
		int insertIndex = 0;

		int numPoints = (int)simplex.size();

		// 全ての辺に対して原点からの距離を求める
		for (int i = 0; i < numPoints;++i) {
			int j = (i + 1) % numPoints;// 次の点のインデックス
			SupportPoint p1 = simplex[i];
			SupportPoint p2 = simplex[j];

			// [ 点を求める ]
			Vector2 closestPoint = GetOriginProjection(p1.v, p2.v);
			// [ 距離を求める ]
			float depth = GetDistanceToOrigin(closestPoint);

			// [ 一番近い距離を更新 ]
			if (depth < minDepth) {
				minDepth = depth;
				minDirection = closestPoint; // 原点から最も近い点へのベクトル
				insertIndex = j;             // 後で頂点を挿入する位置
			}
		}

		// 押し出し方向のベクトルを正規化（長さ1にする）
		// ※ ユーザー環境の正規化関数（Normalize等）を使用してください
		Vector2 normal = Normalize(minDirection);

		// 2. 求めたベクトルからサポートポイントを求める
		SupportPoint d = getSupport(shapeA, shapeB, normal);

		// 新しいサポートポイントが、原点から法線方向にどれだけ離れているか（内積）
		float newDepth = Dot2d(d.v, normal);

		// 3. 終了判定（似たような値が出たか）
		if (newDepth - minDepth < TOLERANCE) {
			// 境界に到達したと判定して結果を返す
			contact.depth = newDepth; // または minDepth
			contact.direction = normal;
			return contact;
		}

		// 4. 境界に達していなければ、求めたサポートポイントを辺の間に挿入して多角形を拡張 (Expand) する
		simplex.insert(simplex.begin() + insertIndex, d);

	}
	// 計算の結果を構造体にいれ、返す
	return contact;
}

bool collision(std::vector<Vector2> shapeA, std::vector<Vector2>shapeB, std::vector<SupportPoint>& simplex) {
	// 初期方向
	Vector2 direction = { 1.0f,0.0f };
	// 最初のサポートポイント
	SupportPoint p = getSupport(shapeA, shapeB, direction);

	// 単体(Simplex)を管理
	simplex.push_back(p);// 登録
	// 次の方向の取得
	direction = { -p.v.x,-p.v.y };
	int index = 0;// ループ制御変数
	while (index < 30) {
		SupportPoint r = getSupport(shapeA, shapeB, direction);

		// 新しい点が原点を超えていないなら衝突しない
		if ((r.v.x * direction.x + r.v.y * direction.y) < 0) {
			return false;
		}

		simplex.push_back(r);

		if (UpdateSimplex(simplex, direction)) {
			return true;
		}
		index++;
	}
	return false;
}

// --- ここから三次元のコード ---

struct Matrix3x3 {
	float m[3][3];

	// ベクトルとの積
	Vector3 multiply(const Vector3& v) const {
		return {
			m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z,
			m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z,
			m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z
		};
	}
};

// 3x3行列の逆行列を求める関数
Matrix3x3 Inverse(const Matrix3x3& M) {
	float det = M.m[0][0] * (M.m[1][1] * M.m[2][2] - M.m[1][2] * M.m[2][1]) -
		M.m[0][1] * (M.m[1][0] * M.m[2][2] - M.m[1][2] * M.m[2][0]) +
		M.m[0][2] * (M.m[1][0] * M.m[2][1] - M.m[1][1] * M.m[2][0]);

	float invDet = 1.0f / det;
	Matrix3x3 res;
	res.m[0][0] = (M.m[1][1] * M.m[2][2] - M.m[1][2] * M.m[2][1]) * invDet;
	res.m[0][1] = (M.m[0][2] * M.m[2][1] - M.m[0][1] * M.m[2][2]) * invDet;
	res.m[0][2] = (M.m[0][1] * M.m[1][2] - M.m[0][2] * M.m[1][1]) * invDet;
	res.m[1][0] = (M.m[1][2] * M.m[2][0] - M.m[1][0] * M.m[2][2]) * invDet;
	res.m[1][1] = (M.m[0][0] * M.m[2][2] - M.m[0][2] * M.m[2][0]) * invDet;
	res.m[1][2] = (M.m[1][0] * M.m[0][2] - M.m[0][0] * M.m[1][2]) * invDet;
	res.m[2][0] = (M.m[1][0] * M.m[2][1] - M.m[1][1] * M.m[2][0]) * invDet;
	res.m[2][1] = (M.m[2][0] * M.m[0][1] - M.m[0][0] * M.m[2][1]) * invDet;
	res.m[2][2] = (M.m[0][0] * M.m[1][1] - M.m[1][0] * M.m[0][1]) * invDet;
	return res;
}

bool PointInsideTetrahedron(Vector3 a, Vector3 b, Vector3 c, Vector3 d, Vector3 p) {
	Vector3 v1 = b - a;
	Vector3 v2 = c - a;
	Vector3 v3 = d - a;

	// 行列を作成 (各列に基底ベクトルを配置)
	Matrix3x3 mat = { {
		{v1.x, v2.x, v3.x},
		{v1.y, v2.y, v3.y},
		{v1.z, v2.z, v3.z}
	} };

	Matrix3x3 invMat = Inverse(mat);

	// 点 P を A 基準の座標系に変換
	Vector3 newP = invMat.multiply(p - a);

	// 判定条件
	return (newP.x >= 0 && newP.y >= 0 && newP.z >= 0 && (newP.x + newP.y + newP.z) <= 1.0f);
}

struct SupportPoint3D {
	Vector3 v;// 差分ベクトル(ShapeB - ShapeA)
	Vector3 supA;// 各形状のサポートポイント(後で衝突点計算に使う場合)
	Vector3 supB;
};

Vector3 support(std::vector<Vector3>vertices, const Vector3& direction) {
	// 内積が最大の点を探す
	// 保存する変数
	float maxDot = -FLT_MAX;// 最小値からスタート
	Vector3 supportPoint = vertices[0];
	// 全頂点を調べる
	for (const Vector3& vertex : vertices) {
		float dot = vertex.x * direction.x + vertex.y * direction.y + vertex.z * direction.z;
		if (dot > maxDot) {
			maxDot = dot;
			supportPoint = vertex;
		}
	}
	return supportPoint;
}

SupportPoint3D GetSupport3D(std::vector<Vector3>shapeA, std::vector<Vector3>shapeB, const Vector3& direction) {
	SupportPoint3D p = {};
	p.supA = support(shapeA, direction);
	p.supB = support(shapeB, { -direction.x,-direction.y,-direction.z });
	p.v = { p.supA.x - p.supB.x, p.supA.y - p.supB.y, p.supA.z - p.supB.z };
	return p;
}

Vector3 GetOriginProjection3D(const Vector3& v1, const Vector3& v2) {
	// 直線の方向ベクトル d を求める
	Vector3 d = { v2.x - v1.x, v2.y - v1.y, v2.z - v1.z };

	// 方向ベクトルの長さの2乗（分母）を計算
	float denom = d.x * d.x + d.y * d.y + d.z * d.z;

	// ゼロ除算チェック（v1 と v2 が同じ点の場合）
	if (denom < 1e-6f) return v1;

	// パラメータ t を求める
	float t = -(v1.x * d.x + v1.y * d.y + v1.z * d.z) / denom;

	// 直線の式 P = v1 + t * d に代入して座標を出す
	Vector3 projection = {
		v1.x + t * d.x,
		v1.y + t * d.y,
		v1.z + t * d.z
	};

	return projection;
}

bool UpdateSimplex3D(std::vector<SupportPoint3D>& simplex, Vector3& direction) {
	if (simplex.size() == 2) {
		// 直線の2点
		SupportPoint3D a = simplex[0];
		SupportPoint3D b = simplex[1];

		// ABの垂線で原点方向を向くものを次の方向に設定
		direction = GetOriginProjection3D(a.v, b.v);
		direction.x = -direction.x;
		direction.y = -direction.y;
		direction.z = -direction.z;
		return false;
	}
	else if (simplex.size() == 3) {
		// 三角形の三点
		SupportPoint3D a = simplex[0];
		SupportPoint3D b = simplex[1];
		SupportPoint3D c = simplex[2];

		Vector3 ab = { b.v.x - a.v.x, b.v.y - a.v.y, b.v.z - a.v.z };
		Vector3 ac = { c.v.x - a.v.x, c.v.y - a.v.y, c.v.z - a.v.z };
		direction = CrossProduct(ac, ab);

		if (Dot(direction, -a.v)) {
			// 面からの方向が原点を向くように修正
			direction = -direction;
		}

		return false;
	}
	else if (simplex.size() == 4) {
		// 四面体の四点
		SupportPoint3D a = simplex[3];
		SupportPoint3D b = simplex[2];
		SupportPoint3D c = simplex[1];
		SupportPoint3D d = simplex[0];

		Vector3 v1 = b.v - a.v;
		Vector3 v2 = c.v - a.v;
		Vector3 v3 = d.v - a.v;

		// 行列を作成 (各列に基底ベクトルを配置)
		Matrix3x3 mat = { {
			{v1.x, v2.x, v3.x},
			{v1.y, v2.y, v3.y},
			{v1.z, v2.z, v3.z}
		} };

		Matrix3x3 invMat = Inverse(mat);

		// 点 P を A 基準の座標系に変換
		Vector3 newP = invMat.multiply(Vector3{0.0f,0.0f,0.0f} - a.v);

		float u = newP.x; // Bの重み
		float v = newP.y; // Cの重み
		float w = newP.z; // Dの重み
		float alpha = 1.0f - (u + v + w); // Aの重み

		// 判定条件
		if (u >= 0 && v >= 0 && w >= 0 && alpha >= 0) {
			return true; // 衝突検出！
		}
		
		// X が頂点Bの重み、Y が頂点Cの重み、Z が頂点Dの重み
		// 頂点Aの重みは 1 - (X + Y + Z) で求められる
		// 2. 重みがマイナスの点を除外してシンプレックスを更新
		// ※ A（最新の点）は必ず残すのがGJKの鉄則
		if (u < 0) {
			// Bが不要。面ACDで再判定（実際にはさらに面か辺かの絞り込みが必要）
			simplex.erase(simplex.begin() + 2); // Bを削除
		}
		else if (v < 0) {
			// Cが不要
			simplex.erase(simplex.begin() + 1); // Cを削除
		}
		else if (w < 0) {
			// Dが不要
			simplex.erase(simplex.begin() + 0); // Dを削除
		}

		// ここから下の判定を変える
		Vector3 ab = { simplex[1].v.x - simplex[0].v.x, simplex[1].v.y - simplex[0].v.y, simplex[1].v.z - simplex[0].v.z };
		Vector3 ac = { simplex[2].v.x - simplex[0].v.x, simplex[2].v.y - simplex[0].v.y, simplex[2].v.z - simplex[0].v.z };
		direction = CrossProduct(ac, ab);

		if (Dot(direction, -simplex[0].v)) {
			// 面からの方向が原点を向くように修正
			direction = -direction;
		}

		return false;
	}
	else {
		return false;
	}
}

bool Collision3D(std::vector<Vector3>shapeA, std::vector<Vector3>shapeB, std::vector<SupportPoint3D>& simplex) {
	// 初期方向の設定
	Vector3 direction = { 1.0f,0.0f,0.0f };

	// 最初のサポートポイントを取得
	SupportPoint3D p = GetSupport3D(shapeA, shapeB, direction);
	// 単体の管理
	simplex.push_back(p);
	// 次の方向の取得
	direction = { -p.v.x,-p.v.y,-p.v.z };
	int index = 0;// ループ制御変数
	while (index < 50) {
		SupportPoint3D r = GetSupport3D(shapeA, shapeB, direction);
		// 新しい点が原点を超えていないなら衝突しない
		if ((r.v.x * direction.x + r.v.y * direction.y + r.v.z * direction.z) < 0) {
			return false;
		}
		simplex.push_back(r);
		if (UpdateSimplex3D(simplex, direction)) {
			return true;
		}
		index++;
	}
	return false;
}

// WinterのGJKのコードはGJKの方に書いた


// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	//float deltaTime = 1.0f / 60.0f;

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	// カメラ
	Vector3 scaleCamera = { 1.0f,1.0f,0.250f };
	Vector3 rotateCamera = { 0.26f,0.0f,0.0f };
	Vector3 translateCamera = { 0.0f,1.9f,-6.49f };
	Camera debugCamera = { scaleCamera,rotateCamera,translateCamera };
	Matrix4x4 viewProjectionMatrix = MakeViewProjectionMatrix({ 1.0f,1.0f,1.0f }, { 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f }, scaleCamera, rotateCamera, translateCamera);
	Matrix4x4 viewportMatrix = MakeViewportMatrix(0.0f, 0.0f, 1280.0f, 720.0f, 0.0f, 0.0f);
	// カメラ

	Vector2 triangleTop = { 0.0f, 1.0f };
	Vector2 triangleLeft = { 0.0f, 0.0f };
	Vector2 triangleRight = { 1.0f, 0.0f };

	float quadMiddle = 0.5f;
	Vector2 quadTopLeft = { 2.0f - 2.0f + quadMiddle, 4.0f - 2.0f + quadMiddle };
	Vector2 quadTopRight = { 4.0f - 2.0f + quadMiddle, 4.0f - 2.0f + quadMiddle };
	Vector2 quadBottomLeft = { 2.0f - 2.0f + quadMiddle, 2.0f - 2.0f + quadMiddle };
	Vector2 quadBottomRight = { 4.0f - 2.0f + quadMiddle, 2.0f - 2.0f + quadMiddle };

	Vector2 quadTopLeft2 = { 2.0f - 2.0f + quadMiddle, 4.0f - 2.0f + quadMiddle };
	Vector2 quadTopRight2 = { 4.0f - 2.0f + quadMiddle, 4.0f - 2.0f + quadMiddle };
	Vector2 quadBottomLeft2 = { 2.0f - 2.0f + quadMiddle, 2.0f - 2.0f + quadMiddle };
	Vector2 quadBottomRight2 = { 4.0f - 2.0f + quadMiddle, 2.0f - 2.0f + quadMiddle };

	Vector3 triangleTop3D = { 0.5f, 1.0f, 0.5f };
	Vector3 triangleBackLeft3D = { 0.0f, 0.0f, -0.5f };
	Vector3 triangleBackRight3D = { 1.0f, 0.0f, -0.5f };
	Vector3 triangleFrontLeft3D = { 0.0f, 0.0f, 0.5f };
	Vector3 triangleFrontRight3D = { 1.0f, 0.0f, 0.5f };

	Vector3 quadTopLeft3D = { 2.0f - 2.0f + quadMiddle, 4.0f - 2.0f + quadMiddle, 0.5f };
	Vector3 quadTopRight3D = { 4.0f - 2.0f + quadMiddle, 4.0f - 2.0f + quadMiddle, 0.5f };
	Vector3 quadBottomLeft3D = { 2.0f - 2.0f + quadMiddle, 2.0f - 2.0f + quadMiddle, 0.5f };
	Vector3 quadBottomRight3D = { 4.0f - 2.0f + quadMiddle, 2.0f - 2.0f + quadMiddle, 0.5f };
	Vector3 quadBackTopLeft3D = { 2.0f - 2.0f + quadMiddle, 4.0f - 2.0f + quadMiddle, 2.5f };
	Vector3 quadBackTopRight3D = { 4.0f - 2.0f + quadMiddle, 4.0f - 2.0f + quadMiddle, 2.5f };
	Vector3 quadBackBottomLeft3D = { 2.0f - 2.0f + quadMiddle, 2.0f - 2.0f + quadMiddle, 2.5f };
	Vector3 quadBackBottomRight3D = { 4.0f - 2.0f + quadMiddle, 2.0f - 2.0f + quadMiddle, 2.5f };

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
	
		// 移動処理
		{
			if (keys[DIK_W]) {
				triangleLeft.y -= 1.0f;
				triangleRight.y -= 1.0f;
				triangleTop.y -= 1.0f;

				triangleTop3D.y -= 0.1f;
				triangleBackLeft3D.y -= 0.1f;
				triangleBackRight3D.y -= 0.1f;
				triangleFrontLeft3D.y -= 0.1f;
				triangleFrontRight3D.y -= 0.1f;
			}
			if (keys[DIK_D]) {
				triangleLeft.x += 1.0f;
				triangleRight.x += 1.0f;
				triangleTop.x += 1.0f;

				triangleTop3D.x += 0.1f;
				triangleBackLeft3D.x += 0.1f;
				triangleBackRight3D.x += 0.1f;
				triangleFrontLeft3D.x += 0.1f;
				triangleFrontRight3D.x += 0.1f;
			}
			if (keys[DIK_S]) {
				triangleLeft.y += 1.0f;
				triangleRight.y += 1.0f;
				triangleTop.y += 1.0f;

				triangleTop3D.y += 0.1f;
				triangleBackLeft3D.y += 0.1f;
				triangleBackRight3D.y += 0.1f;
				triangleFrontLeft3D.y += 0.1f;
				triangleFrontRight3D.y += 0.1f;
			}
			if (keys[DIK_A]) {
				triangleLeft.x -= 1.0f;
				triangleRight.x -= 1.0f;
				triangleTop.x -= 1.0f;

				triangleTop3D.x -= 0.1f;
				triangleBackLeft3D.x -= 0.1f;
				triangleBackRight3D.x -= 0.1f;
				triangleFrontLeft3D.x -= 0.1f;
				triangleFrontRight3D.x -= 0.1f;
			}
			if (keys[DIK_K]) {
				triangleTop3D.z += 0.1f;
				triangleBackLeft3D.z += 0.1f;
				triangleBackRight3D.z += 0.1f;
				triangleFrontLeft3D.z += 0.1f;
				triangleFrontRight3D.z += 0.1f;
			}

			if (keys[DIK_I]) {
				triangleTop3D.z -= 0.1f;
				triangleBackLeft3D.z -= 0.1f;
				triangleBackRight3D.z -= 0.1f;
				triangleFrontLeft3D.z -= 0.1f;
				triangleFrontRight3D.z -= 0.1f;
			}
		}
		// 当たり判定2D
		/*std::vector<SupportPoint>simplex;
		if (collision({triangleTop,triangleLeft,triangleRight}, {quadBottomLeft,quadBottomRight,quadTopLeft,quadTopRight},simplex)) {
			Novice::DrawBox(100, 100, 300, 300, 0.0f, WHITE, kFillModeWireFrame);
			Contact contact = EPA({ triangleTop,triangleLeft,triangleRight }, { quadBottomLeft,quadBottomRight,quadTopLeft,quadTopRight }, simplex);
			triangleLeft.x += -contact.direction.x * contact.depth;
			triangleLeft.y += -contact.direction.y * contact.depth;

			triangleTop.x += -contact.direction.x * contact.depth;
			triangleTop.y += -contact.direction.y * contact.depth;

			triangleRight.x += -contact.direction.x * contact.depth;
			triangleRight.y += -contact.direction.y * contact.depth;
			Novice::DrawLine(int(simplex[0].v.x * 50.0f), int(simplex[0].v.y * 50.0f), int(simplex[1].v.x * 50.0f), int(simplex[1].v.y * 50.0f), 0xFFFF00FF);
			Novice::DrawLine(int(simplex[1].v.x * 50.0f), int(simplex[1].v.y * 50.0f), int(simplex[2].v.x * 50.0f), int(simplex[2].v.y * 50.0f), 0xFFFF00FF);
			Novice::DrawLine(int(simplex[0].v.x * 50.0f), int(simplex[0].v.y * 50.0f), int(simplex[2].v.x * 50.0f), int(simplex[2].v.y * 50.0f), 0xFFFF00FF);

			ImGui::Begin("Debug");
			ImGui::DragFloat2("Simplex : 1", &simplex[0].v.x);
			ImGui::DragFloat2("Simplex : 2", &simplex[1].v.x);
			ImGui::DragFloat2("Simplex : 3", &simplex[2].v.x);
			ImGui::End();
		}

		std::vector<SupportPoint>simplex2;
		if (collision({ triangleTop,triangleLeft,triangleRight }, { quadBottomLeft2,quadBottomRight2,quadTopLeft2,quadTopRight2 }, simplex2)) {
			Novice::DrawBox(100, 100, 300, 300, 0.0f, WHITE, kFillModeWireFrame);
			Contact contact = EPA({ triangleTop,triangleLeft,triangleRight }, { quadBottomLeft2,quadBottomRight2,quadTopLeft2,quadTopRight2 }, simplex2);
			triangleLeft.x += -contact.direction.x * contact.depth;
			triangleLeft.y += -contact.direction.y * contact.depth;

			triangleTop.x += -contact.direction.x * contact.depth;
			triangleTop.y += -contact.direction.y * contact.depth;

			triangleRight.x += -contact.direction.x * contact.depth;
			triangleRight.y += -contact.direction.y * contact.depth;
			Novice::DrawLine(int(simplex2[0].v.x * 50.0f), int(simplex2[0].v.y * 50.0f), int(simplex2[1].v.x * 50.0f), int(simplex2[1].v.y * 50.0f), 0xFFFF00FF);
			Novice::DrawLine(int(simplex2[1].v.x * 50.0f), int(simplex2[1].v.y * 50.0f), int(simplex2[2].v.x * 50.0f), int(simplex2[2].v.y * 50.0f), 0xFFFF00FF);
			Novice::DrawLine(int(simplex2[0].v.x * 50.0f), int(simplex2[0].v.y * 50.0f), int(simplex2[2].v.x * 50.0f), int(simplex2[2].v.y * 50.0f), 0xFFFF00FF);

			ImGui::Begin("Debug");
			ImGui::DragFloat2("Simplex2 : 1", &simplex2[0].v.x);
			ImGui::DragFloat2("Simplex2 : 2", &simplex2[1].v.x);
			ImGui::DragFloat2("Simplex2 : 3", &simplex2[2].v.x);
			ImGui::End();
		}*/

		// 当たり判定3D
		std::vector<SupportPoint3D>simplex3D;
		if (Collision3D({ triangleTop3D,triangleBackLeft3D,triangleBackRight3D,triangleFrontLeft3D,triangleFrontRight3D }, { quadBottomLeft3D,quadBottomRight3D,quadTopLeft3D,quadTopRight3D,quadBackBottomLeft3D,quadBackBottomRight3D,quadBackTopLeft3D,quadBackTopRight3D }, simplex3D)) {
			Novice::DrawBox(100, 100, 300, 300, 0.0f, WHITE, kFillModeWireFrame);
			ImGui::Begin("Debug");
			ImGui::DragFloat3("Simplex : 1", &simplex3D[0].v.x);
			ImGui::DragFloat3("Simplex : 2", &simplex3D[1].v.x);
			ImGui::DragFloat3("Simplex : 3", &simplex3D[2].v.x);
			ImGui::DragFloat3("Simplex : 4", &simplex3D[3].v.x);
			ImGui::End();
		}

		///
		/// ↑更新処理ここまで
		///

		

		///
		/// ↓描画処理ここから
		///
		
#pragma region ImGui
		ImGui::Begin("Debug2D");
		ImGui::DragFloat2("TriPosTop", &triangleTop.x);
		ImGui::DragFloat2("TriPosLeft", &triangleLeft.x);
		ImGui::DragFloat2("TriPosRight", &triangleRight.x);

		ImGui::DragFloat2("quadPosBottomLeft", &quadBottomLeft.x);
		ImGui::DragFloat2("quadPosBottomRight", &quadBottomRight.x);
		ImGui::DragFloat2("quadPosTopLeft", &quadTopLeft.x);
		ImGui::DragFloat2("quadPosTopRight", &quadTopRight.x);

		ImGui::DragFloat2("quadPosBottomLeft2", &quadBottomLeft2.x);
		ImGui::DragFloat2("quadPosBottomRight2", &quadBottomRight2.x);
		ImGui::DragFloat2("quadPosTopLeft2", &quadTopLeft2.x);
		ImGui::DragFloat2("quadPosTopRight2", &quadTopRight2.x);
		
		ImGui::End();


		ImGui::Begin("Debug3D");

		ImGui::DragFloat3("CameraScale", &scaleCamera.x);
		ImGui::DragFloat3("CameraRotate", &rotateCamera.x,0.01f);
		ImGui::DragFloat3("CameraTranslate", &translateCamera.x);

		ImGui::DragFloat3("TriPosTop3D", &triangleTop3D.x);
		ImGui::DragFloat3("TriPosLeft3D", &triangleBackLeft3D.x);
		ImGui::DragFloat3("TriPosRight3D", &triangleBackRight3D.x);
		ImGui::DragFloat3("TriPosFrontLeft3D", &triangleFrontLeft3D.x);
		ImGui::DragFloat3("TriPosFrontRight3D", &triangleFrontRight3D.x);
		ImGui::DragFloat3("quadPosTopLeft3D", &quadTopLeft3D.x);
		ImGui::DragFloat3("quadPosTopRight3D", &quadTopRight3D.x);
		ImGui::DragFloat3("quadPosBottomLeft3D", &quadBottomLeft3D.x);
		ImGui::DragFloat3("quadPosBottomRight3D", &quadBottomRight3D.x);
		ImGui::DragFloat3("quadPosBackTopLeft3D", &quadBackTopLeft3D.x);
		ImGui::DragFloat3("quadPosBackTopRight3D", &quadBackTopRight3D.x);
		ImGui::DragFloat3("quadPosBackBottomLeft3D", &quadBackBottomLeft3D.x);
		ImGui::DragFloat3("quadPosBackBottomRight3D", &quadBackBottomRight3D.x);

		ImGui::End();
#pragma endregion

		
		Novice::DrawQuad((int)quadBottomLeft.x * 1, (int)quadBottomLeft.y * 1, (int)quadBottomRight.x * 1, (int)quadBottomRight.y * 1, (int)quadTopLeft.x * 1, (int)quadTopLeft.y * 1, (int)quadTopRight.x * 1, (int)quadTopRight.y * 1, 0, 0, 0, 0, 0, 0x0000FF44);
		Novice::DrawQuad((int)quadBottomLeft2.x * 1, (int)quadBottomLeft2.y * 1, (int)quadBottomRight2.x * 1, (int)quadBottomRight2.y * 1, (int)quadTopLeft2.x * 1, (int)quadTopLeft2.y * 1, (int)quadTopRight2.x * 1, (int)quadTopRight2.y * 1, 0, 0, 0, 0, 0, 0x0000FF44);
		Novice::DrawTriangle((int)triangleTop.x * 1, (int)triangleTop.y * 1, (int)triangleLeft.x * 1, (int)triangleLeft.y * 1, (int)triangleRight.x * 1, (int)triangleRight.y * 1, RED, kFillModeWireFrame);

		DrawSpecialTriangle3D(triangleTop3D,
			triangleBackLeft3D, triangleBackRight3D,
			triangleFrontLeft3D, triangleFrontRight3D,
			scaleCamera, rotateCamera, translateCamera,
			0xFFFFFFFF);

		DrawCustomCuboid(quadTopLeft3D, quadTopRight3D, quadBottomRight3D, quadBottomLeft3D, quadBackTopLeft3D, quadBackTopRight3D, quadBackBottomRight3D, quadBackBottomLeft3D, scaleCamera, rotateCamera, translateCamera, 0x00FF00FF);

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
