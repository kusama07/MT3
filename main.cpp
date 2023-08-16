#include <Novice.h>
#include <imgui.h>
#include <cassert>
#include <cmath>
#include <numbers>
#include "Vector3.h"
#include "Matrix4x4.h"
#include "MyMath.h"

const char kWindowTitle[] = "LE2B_10_クサマリョウト";

const int kWindowWidth = 1280;
const int kWindowHeight = 720;

static const int kColumWidth = 60;

struct Sphere
{
	Vector3 center;
	float radius;
};

void VectorScreenPrintf(int x, int y, const Vector3& vector, const char* label)
{
	Novice::ScreenPrintf(x, y, "%0.2f", vector.x);
	Novice::ScreenPrintf(x + kColumWidth, y, "%0.2f", vector.y);
	Novice::ScreenPrintf(x + kColumWidth * 2, y, "%0.2f", vector.z);
	Novice::ScreenPrintf(x + kColumWidth * 3, y, "%s", label);

}

void DrawGrid(const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix)
{

	const float kGridHalfWidth = 2.0f;
	const uint32_t kSubdivision = 10;
	const float kGridEvery = (kGridHalfWidth * 2.0f) / float(kSubdivision);

	Vector3 worldVertex[2];

	Vector3 screenVertex[2];

	for (uint32_t xIndex = 0; xIndex <= kSubdivision; xIndex++)
	{
		worldVertex[0] = { (float)xIndex * kGridEvery - kGridHalfWidth, 0.0f, -kGridHalfWidth };
		worldVertex[1] = { (float)xIndex * kGridEvery - kGridHalfWidth, 0.0f, kGridHalfWidth };


		screenVertex[0] = Transform(worldVertex[0], viewProjectionMatrix);
		screenVertex[0] = Transform(screenVertex[0], viewportMatrix);

		screenVertex[1] = Transform(worldVertex[1], viewProjectionMatrix);
		screenVertex[1] = Transform(screenVertex[1], viewportMatrix);


		Novice::DrawLine((int)screenVertex[0].x, (int)screenVertex[0].y, (int)screenVertex[1].x, (int)screenVertex[1].y, 0xAAAAAAFF);

	}


	for (uint32_t zIndex = 0; zIndex <= kSubdivision; zIndex++)
	{
		worldVertex[0] = { -kGridHalfWidth, 0.0f,  (float)zIndex * kGridEvery - kGridHalfWidth };
		worldVertex[1] = { kGridHalfWidth, 0.0f, (float)zIndex * kGridEvery - kGridHalfWidth };


		screenVertex[0] = Transform(worldVertex[0], viewProjectionMatrix);
		screenVertex[0] = Transform(screenVertex[0], viewportMatrix);

		screenVertex[1] = Transform(worldVertex[1], viewProjectionMatrix);
		screenVertex[1] = Transform(screenVertex[1], viewportMatrix);


		Novice::DrawLine((int)screenVertex[0].x, (int)screenVertex[0].y, (int)screenVertex[1].x, (int)screenVertex[1].y, 0xAAAAAAFF);

	}

}

void DrawSphere(const Sphere& sphere, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewPortMatrix, uint32_t color)
{

	const uint32_t kSubdivison = 30;
	const float kLonEvery = 2.0f * float(std::numbers::pi) / float(kSubdivison);
	const float kLatEvery = float(std::numbers::pi) / float(kSubdivison);


	for (uint32_t latIndex = 0; latIndex < kSubdivison; latIndex++)
	{
		float lat = float(-std::numbers::pi) / 2.0f + kLatEvery * latIndex;

		for (uint32_t lonIndex = 0; lonIndex < kSubdivison; lonIndex++)
		{
			float lon = lonIndex * kLonEvery;


			Vector3 a, b, c;
			a = { sphere.radius * std::cosf(lat) * std::cosf(lon),sphere.radius * std::sinf(lat), sphere.radius * std::cosf(lat) * std::sinf(lon) };
			a = Add(a, sphere.center);
			b = { sphere.radius * std::cosf(lat + kLatEvery) * std::cosf(lon), sphere.radius * std::sinf(lat + kLatEvery), sphere.radius * std::cosf(lat + kLatEvery) * std::sinf(lon) };
			b = Add(b, sphere.center);
			c = { sphere.radius * std::cosf(lat) * cosf(lon + kLonEvery), sphere.radius * std::sinf(lat), sphere.radius * std::cosf(lat) * std::sinf(lon + kLonEvery) };
			c = Add(c, sphere.center);


			a = Transform(a, viewProjectionMatrix);
			a = Transform(a, viewPortMatrix);
			b = Transform(b, viewProjectionMatrix);
			b = Transform(b, viewPortMatrix);
			c = Transform(c, viewProjectionMatrix);
			c = Transform(c, viewPortMatrix);


			Novice::DrawLine(int(a.x), int(a.y), int(b.x), int(b.y), color);
			Novice::DrawLine(int(a.x), int(a.y), int(c.x), int(c.y), color);

		}
	}

}

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, kWindowWidth, kWindowHeight);

	// キー入力結果を受け取る箱
	char keys[256] = { 0 };
	char preKeys[256] = { 0 };


	Vector3 rotate{};

	Vector3 translate{};

	Vector3 cameraTranslate{ 0.0f, 1.9f, -6.49f };

	Vector3 cameraRotate{ 0.26f, 0.0f, 0.0f };

	Sphere sphere = { 0.0f, 0.0f, 0.0f, 1.0f };

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


		Matrix4x4 worldMatrix = MakeAffineMatrix({ 1.0f, 1.0f, 1.0f }, rotate, translate);


		Matrix4x4 cameraMatrix = MakeAffineMatrix({ 1.0f, 1.0f, 1.0f }, cameraRotate, cameraTranslate);


		Matrix4x4 viewMatrix = Inverse(cameraMatrix);
		Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(kWindowWidth) / float(kWindowHeight), 0.1f, 100.0f);
		Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(viewMatrix, projectionMatrix));


		Matrix4x4 viewPortmatrix = MakeViewPortMatrix(0, 0, float(kWindowWidth), float(kWindowHeight), 0.0f, 1.0f);

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		// グリッドの描画
		DrawGrid(worldViewProjectionMatrix, viewPortmatrix);

		// 球の描画
		DrawSphere(sphere, worldViewProjectionMatrix, viewPortmatrix, 0x000000FF);

		///
		/// ↑描画処理ここまで
		///

		///
		/// ↓デバック処理ここから
		///

		ImGui::Begin("Debug");
		ImGui::DragFloat3("cameraTranslate", &cameraTranslate.x, 0.01f);
		ImGui::DragFloat3("cameraRotate", &cameraRotate.x, 0.01f);
		ImGui::DragFloat3("SphereCenter", &sphere.center.x, 0.01f);
		ImGui::DragFloat("SphereRadius", &sphere.radius, 0.01f);
		ImGui::End();

		///
		/// ↑デバック処理ここまで
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