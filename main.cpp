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

	if (SameDirection(abc, ao)) {
		return Triangle(points = { a, b, c }, direction);
	}

	if (SameDirection(acd, ao)) {
		return Triangle(points = { a, c, d }, direction);
	}

	if (SameDirection(adb, ao)) {
		return Triangle(points = { a, d, b }, direction);
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


		// 位置変更があれば頂点を再生成（当たり判定・描画に反映）
		cubeA.m_vertices = CreateCube(cubeAPos, 1.0f);
		cubeB.m_vertices = CreateCube(cubeBPos, 1.0f);
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
		if (GJK(cubeA,cubeB)) {
			Novice::DrawBox(100, 100, 300, 300, 0.0f, WHITE, kFillModeWireFrame);
		}

		// base vertices を生成してオフセットを足す
		{
			auto baseA = CreateCube(cubeAPos, 1.0f);
			for (int i = 0; i < 8; ++i) {
				cubeA.m_vertices[i] = { baseA[i].x + vertexOffsetsA[i].x,
										baseA[i].y + vertexOffsetsA[i].y,
										baseA[i].z + vertexOffsetsA[i].z };
			}
			auto baseB = CreateCube(cubeBPos, 1.0f);
			for (int i = 0; i < 8; ++i) {
				cubeB.m_vertices[i] = { baseB[i].x + vertexOffsetsB[i].x,
										baseB[i].y + vertexOffsetsB[i].y,
										baseB[i].z + vertexOffsetsB[i].z };
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
