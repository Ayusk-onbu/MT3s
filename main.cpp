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
#include <array>

const char kWindowTitle[] = "LE2A_10_ハマダ_カズヤ_MT3";
// WinterのGJKのコード
struct Collider {
	virtual Vector3 FindFurthestPoint(Vector3 direction) const = 0;
};
struct MeshCollider : Collider
{
public:
	std::vector<Vector3> m_vertices;

public:
	Vector3 FindFurthestPoint(Vector3 direction) const override
	{
		Vector3  maxPoint;
		float maxDistance = -FLT_MAX;

		for (Vector3 vertex : m_vertices) {
			float distance = Dot(vertex, direction);
			if (distance > maxDistance) {
				maxDistance = distance;
				maxPoint = vertex;
			}
		}

		return maxPoint;
	}
};
Vector3 Support(const Collider& colliderA, const Collider& colliderB, Vector3 direction)
{
	return colliderA.FindFurthestPoint(direction)
		- colliderB.FindFurthestPoint(-direction);
}
struct Simplex {
private:
	std::array<Vector3, 4> m_points;
	int m_size;

public:
	Simplex()
		: m_size(0)
	{
	}

	Simplex& operator=(std::initializer_list<Vector3> list)
	{
		m_size = 0;

		for (Vector3 point : list)
			m_points[m_size++] = point;

		return *this;
	}

	void push_front(Vector3 point)
	{
		m_points = { point, m_points[0], m_points[1], m_points[2] };
		m_size = (std::min)(m_size + 1, 4);
	}

	Vector3& operator[](int i) { return m_points[i]; }
	size_t size() const { return m_size; }

	auto begin() const { return m_points.begin(); }
	auto end() const { return m_points.end() - (4 - m_size); }
};

bool SameDirection(const Vector3& direction, const Vector3& ao)
{
	return Dot(direction, ao) > 0;
}

bool Line(Simplex& points, Vector3& direction)
{
	Vector3 a = points[0];
	Vector3 b = points[1];

	Vector3 ab = b - a;
	Vector3 ao = -a;

	if (SameDirection(ab, ao)) {
		direction = CrossProduct(CrossProduct(ab, ao), ab);
	}

	else {
		points = { a };
		direction = ao;
	}

	return false;
}

bool Triangle(Simplex& points, Vector3& direction)
{
	Vector3 a = points[0];
	Vector3 b = points[1];
	Vector3 c = points[2];

	Vector3 ab = b - a;
	Vector3 ac = c - a;
	Vector3 ao = -a;

	Vector3 abc = CrossProduct(ab, ac);

	if (SameDirection(CrossProduct(abc, ac), ao)) {
		if (SameDirection(ac, ao)) {
			points = { a, c };
			direction = CrossProduct(CrossProduct(ac, ao), ac);
		}

		else {
			return Line(points = { a, b }, direction);
		}
	}

	else {
		if (SameDirection(CrossProduct(ab, abc), ao)) {
			return Line(points = { a, b }, direction);
		}

		else {
			if (SameDirection(abc, ao)) {
				direction = abc;
			}

			else {
				points = { a, c, b };
				direction = -abc;
			}
		}
	}

	return false;
}

bool Tetrahedron(Simplex& points, Vector3& direction)
{
	Vector3 a = points[0];
	Vector3 b = points[1];
	Vector3 c = points[2];
	Vector3 d = points[3];

	Vector3 ab = b - a;
	Vector3 ac = c - a;
	Vector3 ad = d - a;
	Vector3 ao = -a;

	Vector3 abc = CrossProduct(ab, ac);
	Vector3 acd = CrossProduct(ac, ad);
	Vector3 adb = CrossProduct(ad, ab);

	//if (SameDirection(abc, ao)) {
	//	return Triangle(points = { a, b, c }, direction);
	//}

	//if (SameDirection(acd, ao)) {
	//	return Triangle(points = { a, c, d }, direction);
	//}

	//if (SameDirection(adb, ao)) {
	//	return Triangle(points = { a, d, b }, direction);
	//}

	// abc面の法線が「外」を向くように調整 (dと逆側ならそのままでOK)
	if (Dot(abc, ad) > 0) abc = -abc;
	if (SameDirection(abc, ao)) {
		points = { a, b, c };
		direction = abc;
		return false;
	}

	// acd面の法線が「外」を向くように調整
	if (Dot(acd, ab) > 0) acd = -acd;
	if (SameDirection(acd, ao)) {
		points = { a, c, d };
		direction = acd;
		return false;
	}

	// adb面の法線が「外」を向くように調整
	if (Dot(adb, ac) > 0) adb = -adb;
	if (SameDirection(adb, ao)) {
		points = { a, d, b };
		direction = adb;
		return false;
	}

	return true;
}

bool NextSimplex(Simplex& points, Vector3& direction)
{
	switch (points.size()) {
	case 2: return Line(points, direction);
	case 3: return Triangle(points, direction);
	case 4: return Tetrahedron(points, direction);
	}

	// never should be here
	return false;
}

bool GJK(const Collider& colliderA, const Collider& colliderB)
{
	// Get initial support point in any direction
	Vector3 support = Support(colliderA, colliderB, Vector3(1, 0, 0));
	// Simplex is an array of points, max count is 4
	Simplex points;
	points.push_front(support);

	// New direction is towards the origin
	Vector3 direction = -support;
	while (true) {
		support = Support(colliderA, colliderB, direction);

		if (Dot(support, direction) <= 0) {
			return false; // no collision
		}

		points.push_front(support);
		if (NextSimplex(points, direction)) {
			return true;
		}
	}
}

// 追加: Polytope (faces) 拡張と法線計算のユーティリティ

// 面の法線と距離を格納する簡易構造体
struct FaceNormal {
	Vector3 normal;
	float distance;
	FaceNormal() : normal{ 0,0,0 }, distance(0.0f) {}
	FaceNormal(const Vector3& n, float d) : normal(n), distance(d) {}
};

// faces: 頂点インデックスのリスト ( tri0: faces[0..2], tri1: faces[3..5], ... )
// 戻り値: (faceNormals, 最小距離の面インデックス(三角形単位) )
static std::pair<std::vector<FaceNormal>, size_t> GetFaceNormals(
	const std::vector<Vector3>& polytope,
	const std::vector<size_t>& faces)
{
	std::vector<FaceNormal> normals;
	size_t minTriangle = 0;
	float minDistance = FLT_MAX;

	if (faces.empty()) return { normals, minTriangle };

	for (size_t i = 0; i + 2 < faces.size(); i += 3) {
		Vector3 a = polytope[faces[i]];
		Vector3 b = polytope[faces[i + 1]];
		Vector3 c = polytope[faces[i + 2]];

		Vector3 normal = CrossProduct(b - a, c - a);
		normal = Normalize(normal);

		float distance = Dot(normal, a);

		// 距離が負なら法線の向きを反転して正にする（法線は外側を向く）
		if (distance < 0.0f) {
			normal = -normal;
			distance = -distance;
		}

		normals.emplace_back(normal, distance);

		if (distance < minDistance) {
			minDistance = distance;
			minTriangle = i / 3;
		}
	}

	return { normals, minTriangle };
}

// faces と faces[a], faces[b] が示すインデックスの順で辺を edges に追加する。
// ただし逆向きの辺が既にある場合はそちらを削除する（内部で「一意」を保つ）
static void AddIfUniqueEdge(
	std::vector<std::pair<size_t, size_t>>& edges,
	const std::vector<size_t>& faces,
	size_t a, size_t b)
{
	// faces[a], faces[b] を実際の頂点インデックスとして扱う
	size_t va = faces[a];
	size_t vb = faces[b];
	auto reverseIt = std::find(edges.begin(), edges.end(), std::make_pair(vb, va));
	if (reverseIt != edges.end()) {
		// 逆向きの辺があれば消す（内部で共有辺はキャンセルされる）
		edges.erase(reverseIt);
	}
	else {
		edges.emplace_back(va, vb);
	}
}

// polytope と faces を support 点で拡張する。
// 処理内容:
//  - faces の各面について face normal を計算し、support 方向を向いている面は除去してその辺を uniqueEdges に収集する。
//  - uniqueEdges を使って support を共有頂点とする新しい三角形を作成して faces に追加する。
// 戻り値: 追加された三角形のインデックス範囲を (startTriIndex, triCount) の pair で返す。
// 注意: faces は tri ごとのインデックス配列 (3*n)。
static std::pair<size_t, size_t> AddSupportToPolytope(
	std::vector<Vector3>& polytope,
	std::vector<size_t>& faces,
	const Vector3& support)
{
	// 1) 全面法線を取得
	auto [normals, minFace] = GetFaceNormals(polytope, faces);

	// 2) support に面が向いているか調べ、向いている面を削除してエッジを収集する
	std::vector<std::pair<size_t, size_t>> uniqueEdges;
	std::vector<size_t> newFaces; // 残すべき面のインデックスリスト（再構築用）

	for (size_t tri = 0; tri < normals.size(); ++tri) {
		const FaceNormal& fn = normals[tri];
		// 面の法線が support の方向を向いている (内積 > 0) なら削除対象
		if (Dot(fn.normal, support) > 0.0f) {
			// この三角形のフェイス頂点のインデックス
			size_t f = tri * 3;
			// 各辺を uniqueEdges に追加（逆向きがあれば消える）
			AddIfUniqueEdge(uniqueEdges, faces, f + 0, f + 1);
			AddIfUniqueEdge(uniqueEdges, faces, f + 1, f + 2);
			AddIfUniqueEdge(uniqueEdges, faces, f + 2, f + 0);
			// この三角形は保持しない（スキップ）
		}
		else {
			// 保持する三角形は newFaces にコピー
			size_t f = tri * 3;
			newFaces.push_back(faces[f + 0]);
			newFaces.push_back(faces[f + 1]);
			newFaces.push_back(faces[f + 2]);
		}
	}

	// 置き換え
	faces.swap(newFaces);

	// 3) uniqueEdges を使って新しい三角形を作る
	size_t supportIndex = polytope.size();
	polytope.push_back(support);

	size_t startTriIndex = faces.size() / 3; // 追加される三角形の開始インデックス
	for (auto& e : uniqueEdges) {
		// e.first, e.second は頂点インデックス
		faces.push_back(static_cast<size_t>(e.first));
		faces.push_back(static_cast<size_t>(e.second));
		faces.push_back(supportIndex);
	}
	size_t triCount = uniqueEdges.size();

	return { startTriIndex, triCount };
}

struct Contact3D {
	Vector3 normal;
	float depth;
	bool hasCollision;
	Contact3D() : normal{ 0,0,0 }, depth(0.0f), hasCollision(false) {}
	Contact3D(const Vector3& n, float d) : normal(n), depth(d), hasCollision(true) {}
};

// GJK を実行して、衝突があった場合に最終 Simplex の点群を outSimplex に格納して true を返す。
// 既存の GJK と同じアルゴリズムを使用（Simplex 構造体を利用）。
bool GJK_GetSimplex(const Collider& colliderA, const Collider& colliderB, std::vector<Vector3>& outSimplex) {
	// 初期サポート
	Vector3 support = Support(colliderA, colliderB, Vector3(1, 0, 0));
	Simplex points;
	points.push_front(support);

	Vector3 direction = -support;
	int iterations = 0;
	while (iterations++ < 64) {
		support = Support(colliderA, colliderB, direction);
		if (Dot(support, direction) <= 0.0f) {
			return false; // 衝突なし
		}
		points.push_front(support);
		if (NextSimplex(points, direction)) {
			// 衝突。points に残っている点を outSimplex にコピーする
			outSimplex.clear();
			for (size_t i = 0; i < points.size(); ++i) {
				outSimplex.push_back(points[(int)i]);
			}
			return true;
		}
	}
	return false;
}

// EPA 用ユーティリティ (GetFaceNormals, AddIfUniqueEdge, AddSupportToPolytope) は
// 既に main.cpp にある実装をそのまま使える前提。
// ここでは EPA のループを実装する。

// EPA: 初期多面体(単体) から押し出し法線と深さを求める
Contact3D EPA3D(const Collider& colliderA, const Collider& colliderB, const std::vector<Vector3>& initialSimplex) {
	// 初期多面体の頂点リストを作る
	std::vector<Vector3> polytope = initialSimplex;

	// simplex は通常 4 点になるはず (A,B,C,D)。faces の初期化は順序に注意
	// 初期 simplex の頂点順に依存するが、代表的な四面体の faces を作成
	std::vector<size_t> faces;
	if (polytope.size() == 4) {
		// faces: triangles (0,1,2), (0,3,1), (0,2,3), (1,3,2)
		faces = { 0,1,2, 0,3,1, 0,2,3, 1,3,2 };
	}
	else if (polytope.size() == 3) {
		// 三角形しかない場合は背面に一点を追加して四面体にする（原点方向に少し押し戻す）
		// 安全処理: 単純に原点方向に小さな点を追加
		Vector3 a = polytope[0], b = polytope[1], c = polytope[2];
		Vector3 d = (a + b + c) / 3.0f + Vector3{ 0.001f,0.001f,0.001f };
		polytope.push_back(d);
		faces = { 0,1,2, 0,3,1, 0,2,3, 1,3,2 };
	}
	else {
		// 想定外: 返却
		return Contact3D();
	}

	constexpr int kMaxEPAIterations = 64;
	constexpr float kTolerance = 0.0001f;

	for (int iter = 0; iter < kMaxEPAIterations; ++iter) {
		// faces -> 法線と距離の列を取得（GetFaceNormals は既実装）
		auto [normals, minFace] = GetFaceNormals(polytope, faces);
		if (normals.empty()) break;

		Vector3 minNormal = normals[minFace].normal;
		float minDistance = normals[minFace].distance;

		// サポートポイントを取る
		Vector3 support = Support(colliderA, colliderB, minNormal);
		float sDistance = Dot(minNormal, support);

		// 終了条件
		if (sDistance - minDistance < kTolerance) {
			// minNormal は既に外向き; penetration depth を返す
			return Contact3D(minNormal, minDistance);
		}

		// 多面体を拡張（AddSupportToPolytope は既実装）
		auto added = AddSupportToPolytope(polytope, faces, support);
		// AddSupportToPolytope は polytope と faces を更新する（上で実装済み）
		// 次ループで normals を再計算して続行
	}
	// 収束しなかった場合は失敗とみなす
	return Contact3D();
}

// 立方体の頂点 (中心 offset, サイズ size) を生成する関数
std::vector<Vector3> CreateCube(const Vector3& offset, float size) {
	float s = size * 0.5f;
	return {
		{offset.x - s, offset.y - s, offset.z - s},
		{offset.x + s, offset.y - s, offset.z - s},
		{offset.x - s, offset.y + s, offset.z - s},
		{offset.x + s, offset.y + s, offset.z - s},
		{offset.x - s, offset.y - s, offset.z + s},
		{offset.x + s, offset.y - s, offset.z + s},
		{offset.x - s, offset.y + s, offset.z + s},
		{offset.x + s, offset.y + s, offset.z + s}
	};
}

struct GJKEPATestResult {
	Vector3 bPos;
	bool gjk = false;
	bool epa = false;
	float depth = 0.0f;
	Vector3 normal = { 0.0f,0.0f,0.0f };
};

static std::vector<GJKEPATestResult> RunGJKEPATests() {
	std::vector<GJKEPATestResult> results;

	// テストケース: cubeB の位置リスト (必要に応じて追加・調整する)
	std::vector<Vector3> testPositions = {
		{0.4f, 0.0f, 0.0f}, // 深く交差
		{0.8f, 0.0f, 0.0f}, // 軽い交差
		{1.0f, 0.0f, 0.0f}, // 端接触(ケース依存)
		{1.2f, 0.0f, 0.0f}, // 離れている
		{0.0f, 0.9f, 0.0f}, // Y 軸方向に近接
		{0.0f, 0.0f, 1.8f}  // Z 軸方向に離れている
	};

	for (auto pos : testPositions) {
		// 検査用メッシュを作成
		MeshCollider A, B;
		A.m_vertices = CreateCube({ 0.0f, 0.0f, 0.0f }, 1.0f);
		B.m_vertices = CreateCube(pos, 1.0f);

		GJKEPATestResult r;
		r.bPos = pos;

		// GJK で simplex を取得
		std::vector<Vector3> simplex;
		if (GJK_GetSimplex(A, B, simplex)) {
			r.gjk = true;
			// EPA で押し出し法線と深さを取得
			Contact3D contact = EPA3D(A, B, simplex);
			r.epa = contact.hasCollision;
			r.depth = contact.depth;
			r.normal = contact.normal;
		}
		else {
			r.gjk = false;
			r.epa = false;
			r.depth = 0.0f;
			r.normal = { 0.0f,0.0f,0.0f };
		}

		results.push_back(r);
	}

	return results;
}

// グローバル変数に結果を保持（WinMain 内で初期化して使用）
static std::vector<GJKEPATestResult> g_gjkEpaResults;

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	//float deltaTime = 1.0f / 60.0f;

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	g_gjkEpaResults = RunGJKEPATests();

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

	// cube の位置を保持
	Vector3 cubeAPos = { 0.0f, 0.0f, 0.0f };
	Vector3 cubeBPos = { 0.8f, 0.0f, 0.0f };

	// 値 1: 原点にある 1x1x1 の立方体
	MeshCollider cubeA = {};
	cubeA.m_vertices = CreateCube(cubeAPos, 1.0f);

	// 値 2: 少し離れた場所にある 1x1x1 の立方体
	// (0.8, 0, 0) なら重なる、(1.5, 0, 0) なら離れる
	MeshCollider cubeB = {};
	cubeB.m_vertices = CreateCube(cubeBPos, 1.0f);

	// 頂点オフセット（ImGui で編集するため）
	std::array<Vector3, 8> vertexOffsetsA = {};
	std::array<Vector3, 8> vertexOffsetsB = {};
	// 初期化（省略可）
	for (int i = 0; i < 8; ++i) {
		vertexOffsetsA[i] = { 0.0f, 0.0f, 0.0f };
		vertexOffsetsB[i] = { 0.0f, 0.0f, 0.0f };
	}

	

	// 立方体（MeshCollider）を描画する簡易ユーティリティ（ローカルラムダ）
	// 注意: mesh.m_vertices は本コード内で CreateCube により 8 頂点で構成されることを想定
	auto DrawMeshCollider = [&](const MeshCollider& mesh, int color) {
		if (mesh.m_vertices.size() < 8) return;
		Vector3 screen[8];
		for (int i = 0; i < 8; ++i) {
			// world 側は頂点が既にワールド座標にある想定なので identity を渡す
			screen[i] = RenderingPipelineVer2({ 1.0f,1.0f,1.0f }, { 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f },
				scaleCamera, rotateCamera, translateCamera,
				mesh.m_vertices[i],
				1280.0f, 720.0f, 0.45f, 0.1f, 100.0f,
				0.0f, 0.0f, 0.0f, 0.0f);
		}

		// 前面（0-1-2-3）
		Novice::DrawLine(static_cast<int>(screen[0].x), static_cast<int>(screen[0].y), static_cast<int>(screen[1].x), static_cast<int>(screen[1].y), color);
		Novice::DrawLine(static_cast<int>(screen[1].x), static_cast<int>(screen[1].y), static_cast<int>(screen[2].x), static_cast<int>(screen[2].y), color);
		Novice::DrawLine(static_cast<int>(screen[2].x), static_cast<int>(screen[2].y), static_cast<int>(screen[3].x), static_cast<int>(screen[3].y), color);
		Novice::DrawLine(static_cast<int>(screen[3].x), static_cast<int>(screen[3].y), static_cast<int>(screen[0].x), static_cast<int>(screen[0].y), color);

		// 背面（4-5-6-7）
		Novice::DrawLine(static_cast<int>(screen[4].x), static_cast<int>(screen[4].y), static_cast<int>(screen[5].x), static_cast<int>(screen[5].y), color);
		Novice::DrawLine(static_cast<int>(screen[5].x), static_cast<int>(screen[5].y), static_cast<int>(screen[6].x), static_cast<int>(screen[6].y), color);
		Novice::DrawLine(static_cast<int>(screen[6].x), static_cast<int>(screen[6].y), static_cast<int>(screen[7].x), static_cast<int>(screen[7].y), color);
		Novice::DrawLine(static_cast<int>(screen[7].x), static_cast<int>(screen[7].y), static_cast<int>(screen[4].x), static_cast<int>(screen[4].y), color);

		// 接続（0-4,1-5,2-6,3-7）
		Novice::DrawLine(static_cast<int>(screen[0].x), static_cast<int>(screen[0].y), static_cast<int>(screen[4].x), static_cast<int>(screen[4].y), color);
		Novice::DrawLine(static_cast<int>(screen[1].x), static_cast<int>(screen[1].y), static_cast<int>(screen[5].x), static_cast<int>(screen[5].y), color);
		Novice::DrawLine(static_cast<int>(screen[2].x), static_cast<int>(screen[2].y), static_cast<int>(screen[6].x), static_cast<int>(screen[6].y), color);
		Novice::DrawLine(static_cast<int>(screen[3].x), static_cast<int>(screen[3].y), static_cast<int>(screen[7].x), static_cast<int>(screen[7].y), color);
	};

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

				triangleTop3D.y -= 1.0f;
				triangleBackLeft3D.y -= 1.0f;
				triangleBackRight3D.y -= 1.0f;
				triangleFrontLeft3D.y -= 1.0f;
				triangleFrontRight3D.y -= 1.0f;
			}
			if (keys[DIK_D]) {
				triangleLeft.x += 1.0f;
				triangleRight.x += 1.0f;
				triangleTop.x += 1.0f;

				triangleTop3D.x += 1.0f;
				triangleBackLeft3D.x += 1.0f;
				triangleBackRight3D.x += 1.0f;
				triangleFrontLeft3D.x += 1.0f;
				triangleFrontRight3D.x += 1.0f;
			}
			if (keys[DIK_S]) {
				triangleLeft.y += 1.0f;
				triangleRight.y += 1.0f;
				triangleTop.y += 1.0f;

				triangleTop3D.y += 1.0f;
				triangleBackLeft3D.y += 1.0f;
				triangleBackRight3D.y += 1.0f;
				triangleFrontLeft3D.y += 1.0f;
				triangleFrontRight3D.y += 1.0f;
			}
			if (keys[DIK_A]) {
				triangleLeft.x -= 1.0f;
				triangleRight.x -= 1.0f;
				triangleTop.x -= 1.0f;

				triangleTop3D.x -= 1.0f;
				triangleBackLeft3D.x -= 1.0f;
				triangleBackRight3D.x -= 1.0f;
				triangleFrontLeft3D.x -= 1.0f;
				triangleFrontRight3D.x -= 1.0f;
			}
		}

		// --- 追加: 矢印キーで cubeB を動かす ---
		const float kCubeMoveSpeed = 0.05f; // 1フレームあたりの移動量を調整
		if (keys[DIK_LEFT])  cubeBPos.x -= kCubeMoveSpeed;
		if (keys[DIK_RIGHT]) cubeBPos.x += kCubeMoveSpeed;
		if (keys[DIK_UP])    cubeBPos.z += kCubeMoveSpeed; // 前方に移動
		if (keys[DIK_DOWN])  cubeBPos.z -= kCubeMoveSpeed; // 後方に移動
		if (keys[DIK_I])  cubeBPos.y += kCubeMoveSpeed; // 上方に移動
		if (keys[DIK_K])  cubeBPos.y -= kCubeMoveSpeed; // 上方に移動

		if (preKeys[DIK_T] == 0 && keys[DIK_T] != 0) {
			g_gjkEpaResults = RunGJKEPATests();
		}

		// 位置変更があれば頂点を再生成（当たり判定・描画に反映）
		// 位置変更があれば頂点を再生成し、ImGui で編集したオフセットを反映する
		{
			auto baseA = CreateCube(cubeAPos, 1.0f);
			auto baseB = CreateCube(cubeBPos, 1.0f);

			// ensure vectors have size 8
			cubeA.m_vertices = baseA;
			cubeB.m_vertices = baseB;

			for (int i = 0; i < 8; ++i) {
				cubeA.m_vertices[i].x += vertexOffsetsA[i].x;
				cubeA.m_vertices[i].y += vertexOffsetsA[i].y;
				cubeA.m_vertices[i].z += vertexOffsetsA[i].z;

				cubeB.m_vertices[i].x += vertexOffsetsB[i].x;
				cubeB.m_vertices[i].y += vertexOffsetsB[i].y;
				cubeB.m_vertices[i].z += vertexOffsetsB[i].z;
			}
		}
		// --- ここまで追加 ---

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
		}*/

		// 当たり判定3D
		/*if (GJK(cubeA,cubeB)) {
			Novice::DrawBox(100, 100, 300, 300, 0.0f, WHITE, kFillModeWireFrame);
		}*/

		{
			// GJK で simplex を取得
			std::vector<Vector3> simplexForEPA;
			if (GJK_GetSimplex(cubeA, cubeB, simplexForEPA)) {
				Contact3D contact = EPA3D(cubeA, cubeB, simplexForEPA);
				if (contact.hasCollision) {
					// 小さな余裕を加えて突き抜け判定の反復を防ぐ
					const float kEpsilon = 0.001f;

					// contact.normal は多面体の外向き法線（Minkowski空間の法線）
					// これをそのまま使うと cubeB を押し出す方向になる（実装により符号が逆の場合は -normal を使ってください）
					// depth=contact.depth（minDistance）分だけ移動
					cubeBPos = cubeBPos + contact.normal * (contact.depth + kEpsilon);

					// 位置を変えたら頂点も更新して描画と次フレームの当たり判定に反映
					auto baseB = CreateCube(cubeBPos, 1.0f);
					for (int i = 0; i < 8; ++i) {
						cubeB.m_vertices[i] = { baseB[i].x + vertexOffsetsB[i].x,
												baseB[i].y + vertexOffsetsB[i].y,
												baseB[i].z + vertexOffsetsB[i].z };
					}

					// デバッグ表示（任意）
					ImGui::Begin("EPA Debug");
					ImGui::Text("Penetration depth: %.6f", contact.depth);
					ImGui::Text("Normal: %.3f, %.3f, %.3f", contact.normal.x, contact.normal.y, contact.normal.z);
					ImGui::End();
				}
			}
			else {
				// 衝突していなければ何もしない（或いは視覚化だけ）
			}
		}

		///
		/// ↑更新処理ここまで
		///

		

		///
		/// ↓描画処理ここから
		///
		
#pragma region ImGui
		ImGui::Begin("Debug");
		ImGui::DragFloat3("Camera Scale", &scaleCamera.x);
		ImGui::DragFloat3("Camera Rotate", &rotateCamera.x,0.01f);
		ImGui::DragFloat3("Camera Translate", &translateCamera.x);

		ImGui::DragFloat2("TriPosTop", &triangleTop.x);
		ImGui::DragFloat2("TriPosLeft", &triangleLeft.x);
		ImGui::DragFloat2("TriPosRight", &triangleRight.x);

		ImGui::DragFloat2("quadPosBottomLeft", &quadBottomLeft.x);
		ImGui::DragFloat2("quadPosBottomRight", &quadBottomRight.x);
		ImGui::DragFloat2("quadPosTopLeft", &quadTopLeft.x);
		ImGui::DragFloat2("quadPosTopRight", &quadTopRight.x);

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

		ImGui::Begin("Mesh Editor");

		// cubeA 頂点編集
		if (ImGui::CollapsingHeader("Cube A Vertices", ImGuiTreeNodeFlags_DefaultOpen)) {
			for (int i = 0; i < 8; ++i) {
				char label[64];
				snprintf(label, sizeof(label), "A Vertex %d (offset)", i);
				ImGui::DragFloat3(label, &vertexOffsetsA[i].x, 0.01f, -5.0f, 5.0f);
			}
			// リセットボタン
			if (ImGui::Button("Reset A Offsets")) {
				for (auto& v : vertexOffsetsA) v = { 0.0f,0.0f,0.0f };
			}
		}

		// cubeB 頂点編集
		if (ImGui::CollapsingHeader("Cube B Vertices", ImGuiTreeNodeFlags_DefaultOpen)) {
			for (int i = 0; i < 8; ++i) {
				char label[64];
				snprintf(label, sizeof(label), "B Vertex %d (offset)", i);
				ImGui::DragFloat3(label, &vertexOffsetsB[i].x, 0.01f, -5.0f, 5.0f);
			}
			if (ImGui::Button("Reset B Offsets")) {
				for (auto& v : vertexOffsetsB) v = { 0.0f,0.0f,0.0f };
			}
		}

		// 頂点を絶対位置で編集したい場合の補助：ベース頂点を表示（読み取り専用）
		if (ImGui::CollapsingHeader("Base Vertices (read only)")) {
			auto baseA = CreateCube(cubeAPos, 1.0f);
			auto baseB = CreateCube(cubeBPos, 1.0f);
			for (int i = 0; i < 8; ++i) {
				char labA[64]; snprintf(labA, sizeof(labA), "A base %d", i);
				ImGui::Text("%s: %.3f, %.3f, %.3f", labA, baseA[i].x, baseA[i].y, baseA[i].z);
				char labB[64]; snprintf(labB, sizeof(labB), "B base %d", i);
				ImGui::Text("%s: %.3f, %.3f, %.3f", labB, baseB[i].x, baseB[i].y, baseB[i].z);
			}
		}

		ImGui::End();
#pragma endregion
		// ImGui 内にテスト結果ウィンドウを表示
		ImGui::Begin("GJK-EPA Tests");
		if (ImGui::Button("Run Tests (T)")) {
			g_gjkEpaResults = RunGJKEPATests();
		}
		ImGui::Separator();
		ImGui::Text("Index | B Pos (x,y,z) | GJK | EPA | Depth | Normal(x,y,z)");
		for (size_t i = 0; i < g_gjkEpaResults.size(); ++i) {
			const auto& r = g_gjkEpaResults[i];
			ImGui::Text("%02zu: (%.2f, %.2f, %.2f) | %s | %s | %.5f | (%.3f, %.3f, %.3f)",
				i,
				r.bPos.x, r.bPos.y, r.bPos.z,
				r.gjk ? "YES" : "NO",
				r.epa ? "YES" : "NO",
				r.depth,
				r.normal.x, r.normal.y, r.normal.z
			);
		}
		ImGui::End();
		
		Novice::DrawQuad((int)quadBottomLeft.x * 1, (int)quadBottomLeft.y * 1, (int)quadBottomRight.x * 1, (int)quadBottomRight.y * 1, (int)quadTopLeft.x * 1, (int)quadTopLeft.y * 1, (int)quadTopRight.x * 1, (int)quadTopRight.y * 1, 0, 0, 0, 0, 0, 0x0000FF44);
		Novice::DrawTriangle((int)triangleTop.x * 1, (int)triangleTop.y * 1, (int)triangleLeft.x * 1, (int)triangleLeft.y * 1, (int)triangleRight.x * 1, (int)triangleRight.y * 1, RED, kFillModeWireFrame);

		// 作成した MeshCollider をワイヤーフレームで描画
		DrawMeshCollider(cubeA, 0x00FF00FF); // 緑
		DrawMeshCollider(cubeB, 0xFF0000FF); // 赤っぽい青

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
