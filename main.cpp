#include <Novice.h>
#include "MyMath.h"
#include "ImGuiManager.h"

const char kWindowTitle[] = "LE2B_10_クサマリョウト";

const int kWindowWidth = 1280;
const int kWindowHeight = 720;


// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, kWindowWidth, kWindowHeight);

	Vector3 cameraTranslate{ 0.0f, 1.9f, -6.49f };
	Vector3 cameraRotate{ 0.26f, 0.0f, 0.0f };

	Sphere sphere
	{
		{0.0f, 0.0f, 0.0f},
		1.0f
	};

	Plane plane
	{
		{1.0f, 1.0f, 1.0f},
		1.0f
	};

	uint32_t colorS1 = WHITE;
	uint32_t colorS2 = WHITE;


	// キー入力結果を受け取る箱
	char keys[256] = { 0 };
	char preKeys[256] = { 0 };

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


		Matrix4x4 worldMatrix = MakeAffineMatrix({ 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f });
		Matrix4x4 cameraMatrix = MakeAffineMatrix({ 1.0f, 1.0f, 1.0f }, cameraRotate, cameraTranslate);
		Matrix4x4 viewMatrix = Inverse(cameraMatrix);
		Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(kWindowWidth) / float(kWindowHeight), 0.1f, 100.0f);
		Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(viewMatrix, projectionMatrix));
		Matrix4x4 viewportMatrix = MakeViewPortMatrix(0, 0, float(kWindowWidth), float(kWindowHeight), 0.0f, 1.0f);

		if (IsCollision(sphere, plane))
		{
			colorS1 = RED;
		}
		else
		{
			colorS1 = WHITE;
		}

		///
		/// ↑更新処理ここまで
		///


		DrawShere(sphere, worldViewProjectionMatrix, viewportMatrix, colorS1);
		DrawPlane(plane, worldViewProjectionMatrix, viewportMatrix, colorS2);


		///
		/// ↓描画処理ここから
		///

		DrawGrid(worldViewProjectionMatrix, viewportMatrix);

		ImGui::Begin("Debug");
		ImGui::DragFloat3("cameraTRa", &cameraTranslate.x, 0.1f, -50.0f, 50.0f);
		ImGui::DragFloat3("cameraRot", &cameraRotate.x, 0.1f, -50.0f, 50.0f);
		ImGui::DragFloat3("sphereCenter", &sphere.center.x, 0.1f, -1.0f, 1.0f);
		ImGui::DragFloat("sphereRadius", &sphere.radius, 0.1f, -1.0f, 1.0f);
		ImGui::DragFloat("planeDistance", &plane.distance, 0.1f, -1.0f, 5.0f);
		ImGui::DragFloat3("planeNormal", &plane.normal.x, 0.1f, -1.0f, 1.0f);
		plane.normal = Normalize(plane.normal);
		ImGui::End();

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
