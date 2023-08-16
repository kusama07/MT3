#include <Novice.h>
#include "Vector3.h"
#include "MyMath.h"

const char kWindowTitle[] = "LE2B_10_クサマリョウト";

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	const int kWindowWidth = 1280;
	const int kWindowHeight = 720;

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, kWindowWidth, kWindowHeight);

	// キー入力結果を受け取る箱
	char keys[256] = { 0 };
	char preKeys[256] = { 0 };

	Vector3 rotate = {};
	Vector3 translate = {};

	Vector3 v1{ 1.2f,-3.9f,2.5f };
	Vector3 v2{ 2.8f,0.4f,-1.3f };

	Vector3 cross = Cross(v1, v2);

	Vector3 cameraPosition = { 0,0,-1000 };

	Vector3 kLocalVertices[3];

	float speed = 8;

	kLocalVertices[0] = { 0, 100, 0 };
	kLocalVertices[1] = { 100, -100, 0 };
	kLocalVertices[2] = { -100, -100, 0 };

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

		rotate.y += 0.05f;


		if (keys[DIK_W])
		{
			translate.z += speed;
		}

		if (keys[DIK_S])
		{
			translate.z -= speed;
		}

		if (keys[DIK_A])
		{
			cameraPosition.x -= speed;
		}

		if (keys[DIK_D])
		{
			cameraPosition.x += speed;
		}

		Matrix4x4 worldMatrix = MakeAffineMatrix({ 1.0f,1.0f,1.0f }, rotate, translate);

		Matrix4x4 cameraMatrix = MakeAffineMatrix({ 1.0f,1.0f,1.0f }, { 0.0f,0.0f,0.0f }, cameraPosition);

		Matrix4x4 viewMatrix = Inverse(cameraMatrix);

		Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(kWindowWidth) / float(kWindowHeight), 0.1f, 100.0f);

		Matrix4x4 worldViewPrjectionMatrix = Multiply(worldMatrix, Multiply(viewMatrix, projectionMatrix));

		Matrix4x4 viewportMatrix = MakeViewportMatrix(0, 0, float(kWindowWidth), float(kWindowHeight), 0.0f, 1.0f);

		Vector3 screenVerices[3];
		for (uint32_t i = 0; i < 3; ++i)
		{
			Vector3 ndcVertex = Transform(kLocalVertices[i], worldViewPrjectionMatrix);
			screenVerices[i] = Transform(ndcVertex, viewportMatrix);
		}

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		Novice::DrawTriangle(int(screenVerices[0].x), int(screenVerices[0].y), int(screenVerices[1].x), int(screenVerices[1].y), int(screenVerices[2].x), int(screenVerices[2].y), RED, kFillModeSolid);

		VecterScreenPrintf(0, 0, cross, "Cross");

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