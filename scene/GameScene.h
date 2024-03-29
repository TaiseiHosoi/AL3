﻿#pragma once

#include "Audio.h"
#include "DirectXCommon.h"
#include "DebugText.h"
#include "Input.h"
#include "Model.h"
#include "SafeDelete.h"
#include "Sprite.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include <DirectXMath.h>

/// <summary>
/// ゲームシーン
/// </summary>
class GameScene {
public:
	enum PartId {
		Root,	//大元
		Spine,	//脊椎
		Chest,	//胸
		Head,	//頭
		ArmL,	//左上
		ArmR,	//右腕
		Hip,	//尻
		LegL,	//左足
		LegR,	//右足
	};


public: // メンバ関数
  /// <summary>
  /// コンストクラタ
  /// </summary>
	GameScene();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~GameScene();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

private: // メンバ変数
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;
	DebugText* debugText_ = nullptr;
	uint32_t textureHandle_ = 0;	//テクスチャハンドル
	uint32_t soundDataHandle_ = 0;
	uint32_t voiceHandle_ = 0;
	//カメラ上方向の角度
	float viewAngle = 0.0f;


	//スプライト
	Sprite* sprite_ = nullptr;
	Model* model_ = nullptr;

	//ワールドトランスフォーム初期化
	WorldTransform worldTransform_[100];

	//ビュープロジェクション初期化
	ViewProjection viewProjection_;


	/// <summary>
	/// ゲームシーン用
	/// </summary>
};
