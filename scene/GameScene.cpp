#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>
#include <random>

using namespace DirectX;

GameScene::GameScene() {}

GameScene::~GameScene() {
	//delete sprite_;
	delete model_;
}

void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();
	debugText_ = DebugText::GetInstance();

	//ここから記述
	//乱数シード生成器
	std::random_device seed_gen;
	//メルセンヌツイスター
	std::mt19937_64 engine(seed_gen());
	//乱数範囲(回転角度)
	std::uniform_real_distribution<float>rotDist(0.0f, XM_2PI);
	//乱数範囲(座標用)
	std::uniform_real_distribution<float>posDist(-10.0f, 10.0f);

	//X,Y,Z軸回りの平行移動の設定
	worldTransform_->translation_ = { 0.0f,10.0f,0.0f };


	textureHandle_ = TextureManager::Load("mario.jpg");
	//sprite_ = Sprite::Create(textureHandle_, { 100,50 });
	model_ = Model::Create();

	soundDataHandle_ = audio_->LoadWave("sarunonakigoe.wav");
	//voiceHandle_ = audio_->PlayWave(soundDataHandle_, true);

	//大元から脊椎
	//親
	worldTransform_[PartId::Root].Initialize();
	//子
	worldTransform_[PartId::Spine].translation_ = { 0,4.5,0 };
	worldTransform_[PartId::Spine].parent_ = &worldTransform_[PartId::Root];
	worldTransform_[PartId::Spine].Initialize();

	//上半身
	worldTransform_[PartId::Chest].parent_ = &worldTransform_[PartId::Spine];
	worldTransform_[PartId::Chest].translation_ = { 0,0,0 };
	worldTransform_[PartId::Chest].Initialize();


	worldTransform_[PartId::Head].parent_ = &worldTransform_[PartId::Chest];
	worldTransform_[PartId::Head].translation_ = { 0,3,0 };
	worldTransform_[PartId::Head].Initialize();

	worldTransform_[PartId::ArmL].parent_ = &worldTransform_[PartId::Chest];
	worldTransform_[PartId::ArmL].translation_ = { 3,0,0 };
	worldTransform_[PartId::ArmL].Initialize();

	worldTransform_[PartId::ArmR].parent_ = &worldTransform_[PartId::Chest];
	worldTransform_[PartId::ArmR].translation_ = { -3,0,0 };
	worldTransform_[PartId::ArmR].Initialize();

	worldTransform_[PartId::Hip].parent_ = &worldTransform_[PartId::Spine];
	worldTransform_[PartId::Hip].translation_ = { 0,-4,0 };
	worldTransform_[PartId::Hip].Initialize();

	worldTransform_[PartId::LegL].parent_ = &worldTransform_[PartId::Hip];
	worldTransform_[PartId::LegL].translation_ = { -3,-3,0 };
	worldTransform_[PartId::LegL].Initialize();

	worldTransform_[PartId::LegR].parent_ = &worldTransform_[PartId::Hip];
	worldTransform_[PartId::LegR].translation_ = { 3,-3,0 };
	worldTransform_[PartId::LegR].Initialize();




	//worldTransform_[PartId::Head].parent_ = &worldTransform_[PartId::Spine];

	//worldTransform_[PartId::Spine].Initialize();
	



	//100個生成
	//for (size_t i = 0; i < _countof(worldTransform_); i++) {
	//	//X,Y,Z方向のスケーリング
	//	worldTransform_[i].scale_ = { 1.0f,1.0f,1.0f };
	//	//回転角設定
	//	worldTransform_[i].rotation_ = { rotDist(engine),rotDist(engine),rotDist(engine) };
	//	//平行移動
	//	worldTransform_[i].translation_ = { posDist(engine),posDist(engine),posDist(engine) };

	//	//ワールドトランスフォーム初期化
	//	worldTransform_[i].Initialize();
	//}

	//カメラ上方向ベクトルを設定(右上45度指定)
	//viewProjection_.up = { cosf(XM_PI / 4.0f),sinf(XM_PI / 4.0f),0.0f };

	//カメラ視点座標を設定
	//viewProjection_.eye = {0,0,-10};

	//カメラ中視点座標を設定
	//viewProjection_.target = { 10,0,0 };

	////カメラ垂直方向視野角を設定
	//viewProjection_.fovAngleY = XMConvertToRadians(10.0f);

	////アスペクト比設定
	//viewProjection_.aspectRatio = 1.0f;

	////ニアクリップ距離を設定	
	//viewProjection_.nearZ = 52.0f;

	////ファークリップの距離を設定
	//viewProjection_.farZ = 58.0f;

	//ビュープロジェクションの初期化
	viewProjection_.Initialize();


	//音声再生
	//audio_->PlayWave(soundDataHandle_);
}

void GameScene::Update() {
	//スプライトの今の座標を取得
	//XMFLOAT2 position = sprite_->GetPosition();
	//座標を｛２，０｝移動
	//position.x += 2.0f;
	//position.y += 1.0f;

	//視点移動処理
	//クリップ距離変更処理
	{
		//上下キーでニアクリップ距離を増減
		if (input_->PushKey(DIK_UP)) {
			viewProjection_.nearZ += 0.1f;
		}
		else if (input_->PushKey(DIK_DOWN)) {
			viewProjection_.nearZ -= 0.1f;
		}

		//行列の再計算
		viewProjection_.UpdateMatrix();

		//デバッグ用表示
		debugText_->SetPos(50, 130);
		debugText_->Printf("nearZ:%f", viewProjection_.nearZ);
	}
	//FOV偏光処理
	{
		//上キーでfovが広がる
		if (input_->PushKey(DIK_W)) {
			viewProjection_.fovAngleY += 0.01f;
			viewProjection_.fovAngleY = min(viewProjection_.fovAngleY, XM_PI);
			//下キーでし赤くが狭まる
		}
		else if (input_->PushKey(DIK_S)) {
			viewProjection_.fovAngleY -= 0.01f;
			viewProjection_.fovAngleY = max(viewProjection_.fovAngleY, 0.01f);

		}

		//行列の再計算
		viewProjection_.UpdateMatrix();

		//デバッグ用表示
		debugText_->SetPos(50, 110);
		debugText_->Printf("fovAngleY(Degree):%f", XMConvertToDegrees(viewProjection_.fovAngleY));

	}

	//キャラクター移動処理
	{
		//キャラクターの移動ベクトル
		XMFLOAT3 move = { 0,0,0 };

		//キャラクターの移動速度
		const float kCharacterSpeed = 0.2f;

		//押した方向で移動ベクトル
		if (input_->PushKey(DIK_LEFT)) {
			move = { -kCharacterSpeed,0,0 };
		}
		else if (input_->PushKey(DIK_RIGHT)) {
			move = { kCharacterSpeed,0,0 };
		}

		//上半身回転処理
		{
			//速さ
			const float kChestRotSpeed = 0.05f;

			//
			if (input_->PushKey(DIK_U)) {
				worldTransform_[PartId::Chest].rotation_.y -= kChestRotSpeed;

			}else if (input_->PushKey(DIK_I)) {
				worldTransform_[PartId::Chest].rotation_.y += kChestRotSpeed;

			}

			

		}

		{
			//速さ
			const float kHipRotSpeed = 0.05f;

			//
			if (input_->PushKey(DIK_J)) {
				worldTransform_[PartId::Hip].rotation_.y -= kHipRotSpeed;

			}
			else if (input_->PushKey(DIK_K)) {
				worldTransform_[PartId::Hip].rotation_.y += kHipRotSpeed;

			}



		}

		//注視点移動(ベクトルの加算)
		worldTransform_[PartId::Root].translation_.x += move.x;
		worldTransform_[PartId::Root].translation_.y += move.y;
		worldTransform_[PartId::Root].translation_.z += move.z;

		worldTransform_[PartId::Root].UpdateMatrix();
		worldTransform_[PartId::Spine].UpdateMatrix();
		worldTransform_[PartId::Chest].UpdateMatrix();
		worldTransform_[PartId::Head].UpdateMatrix();
		worldTransform_[PartId::ArmL].UpdateMatrix();
		worldTransform_[PartId::ArmR].UpdateMatrix();
		worldTransform_[PartId::Hip].UpdateMatrix();
		worldTransform_[PartId::LegL].UpdateMatrix();
		worldTransform_[PartId::LegR].UpdateMatrix();


		//デバッグ用表示
		debugText_->SetPos(50, 150);
		debugText_->Printf(
			"Root:(%f,%f,%f)", worldTransform_[PartId::Root].translation_.x,
			worldTransform_[PartId::Root].translation_.y,
			worldTransform_[PartId::Root].translation_.z);
	}

	////視点の移動ベクトル
	//{
	//	XMFLOAT3 move = { 0,0,0 };

	//	//視点の移動速さ
	//	const float kEyeSpeed = 0.2f;



	//	//押した方向で移動ベクトルを変更
	//	if (input_->PushKey(DIK_W)) {
	//		move = { 0,0,kEyeSpeed };
	//	}
	//	else if (input_->PushKey(DIK_S)) {
	//		move = { 0,0,-kEyeSpeed };
	//	}

	//	viewProjection_.eye.x += move.x;
	//	viewProjection_.eye.x += move.y;
	//	viewProjection_.eye.x += move.z;
	//}

	////物の位置
	//{
	//	XMFLOAT3 move = { 0,0,0 };

	//	//視点の移動速さ

	//	const float kTargetSpeed = 0.2f;

	//	//押した方向で移動ベクトルを変更

	//	if (input_->PushKey(DIK_LEFT)) {
	//		move = { -kTargetSpeed,0,0 };
	//	}
	//	else if (input_->PushKey(DIK_RIGHT)) {
	//		move = { kTargetSpeed,0,0 };
	//	}
	//	viewProjection_.target.x += move.x;
	//	viewProjection_.target.x += move.y;
	//	viewProjection_.target.x += move.z;
	//}

	////上方向回転処理
	//{
	//	//上方向の回転速さ
	//	const float kUpRotSpeed = 0.05f;

	//	//押した方向で移動ベクトるを変換
	//	if (input_->PushKey(DIK_SPACE)) {
	//		viewAngle += kUpRotSpeed;

	//		//2nを超えたら0に戻す
	//		viewAngle = fmodf(viewAngle, XM_2PI);

	//	}

	//	//方向ベクトルを計算
	//	viewProjection_.up = { cosf(viewAngle),sinf(viewAngle),0.0f };



	//}




	//行列の再計算
	viewProjection_.UpdateMatrix();

	//デバッグ用表示
	debugText_->SetPos(50, 50);
	debugText_->Printf(
		"eye:(%f,%f,%f)", viewProjection_.eye.x, viewProjection_.eye.y, viewProjection_.eye.z);

	debugText_->SetPos(50, 70);
	debugText_->Printf(
		"target:(%f,%f,%f)", viewProjection_.target.x, viewProjection_.target.y, viewProjection_.target.z);

	debugText_->SetPos(50, 90);
	debugText_->Printf(
		"up:(%f,%f,%f)", viewProjection_.up.x, viewProjection_.up.y, viewProjection_.up.z);



	//スプライト反映
	//sprite_->SetPosition(position);

	//スペースキーを押した瞬間
	//if (input_->TriggerKey(DIK_SPACE)) {
	//	//音声停止
	//	audio_->StopWave(voiceHandle_);
	//}

	//debugText_->SetPos(50, 70);						//コメント
	//debugText_->Printf("translation:(%f,%f,%f)",
	//	worldTransform_.translation_.x, worldTransform_.translation_.y, worldTransform_.translation_.z);

	//debugText_->SetPos(50, 85);
	//debugText_->Printf("rotation:(%f,%f,%f)",
	//	worldTransform_.rotation_.x, worldTransform_.rotation_.y, worldTransform_.rotation_.z);

	//debugText_->SetPos(50, 100);
	//debugText_->Printf("scale:(%f,%f,%f)",
	//	worldTransform_.scale_.x, worldTransform_.scale_.y, worldTransform_.scale_.z);
}

void GameScene::Draw() {

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	dxCommon_->ClearDepthBuffer();
#pragma endregion

#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Model::PreDraw(commandList);

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>
	/*for (size_t i = 0; i < _countof(worldTransform_); i++) {
		model_->Draw(worldTransform_[i], viewProjection_, textureHandle_);
	}*/

	//model_->Draw(worldTransform_[PartId::Root], viewProjection_, textureHandle_);
	//model_->Draw(worldTransform_[PartId::Spine], viewProjection_, textureHandle_);
	model_->Draw(worldTransform_[PartId::Chest], viewProjection_, textureHandle_);
	model_->Draw(worldTransform_[PartId::Head], viewProjection_, textureHandle_);
	model_->Draw(worldTransform_[PartId::ArmL], viewProjection_, textureHandle_);
	model_->Draw(worldTransform_[PartId::ArmR], viewProjection_, textureHandle_);
	model_->Draw(worldTransform_[PartId::Hip], viewProjection_, textureHandle_);
	model_->Draw(worldTransform_[PartId::LegL], viewProjection_, textureHandle_);
	model_->Draw(worldTransform_[PartId::LegR], viewProjection_, textureHandle_);
	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>
	/// 
	//sprite_->Draw();

	// デバッグテキストの描画
	debugText_->DrawAll(commandList);
	//
	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}
