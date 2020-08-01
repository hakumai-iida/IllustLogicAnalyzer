/*+----------------------------------------------------------------+
  |	Title:		Updater.cpp [共通環境]
  |	Comment:	ゲーム要素更新基底
  |				更新を行う処理系の基本
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "Updater.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
#define DEFAULT_UPDATE_SPEED		1.0f
#define DEFAULT_UPDATE_IS_BY_TIME	false

/*+----------------------------------------------------------------+
  |	Struct		構造体型宣言
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
bool  CUpdater::s_bUpdateByTime = DEFAULT_UPDATE_IS_BY_TIME;	// 時間更新か？（※フレーム飛びするか？）
bool  CUpdater::s_bUpdatePause = false;							// 停止フラグ
float CUpdater::s_fUpdateSpeed = DEFAULT_UPDATE_SPEED;			// 再生速度：1.0fで100%
float CUpdater::s_fUpdatePastFrame = 0.0f;						// 経過フレーム
float CUpdater::s_fUpdateDeltaFrame = 0.0f;						// 前回の更新からの差分
float CUpdater::s_fUpdateDeltaFrameRemain = 0.0f;				// 前回の更新からの差分
float CUpdater::s_fUpdateDeltaFrameOriginal = 0.0f;				// 前回の更新からの差分

// １フレームめか？
bool  CUpdater::s_bUpdateFirstFrame = false;

// 更新速度アニメ
bool  CUpdater::s_bUpdateSpeedAnimOn;
float CUpdater::s_fUpdateSpeedAnimSec;
float CUpdater::s_fUpdateSpeedAnimSecMax;
float CUpdater::s_fUpdateSpeedAnimFrom;
float CUpdater::s_fUpdateSpeedAnimTo;

/*+----------------------------------------------------------------+
  |	Program		プログラム実装
  +----------------------------------------------------------------+*/
//-----------------------------------
// [クラス] リセット
//-----------------------------------
void CUpdater::ResetAll( void ){
	s_bUpdateByTime = DEFAULT_UPDATE_IS_BY_TIME;
	s_bUpdatePause = false;
	s_fUpdateSpeed = DEFAULT_UPDATE_SPEED;
	s_fUpdatePastFrame = 0.0f;
	s_fUpdateDeltaFrame = 0.0f;
	s_fUpdateDeltaFrameRemain = 0.0f;
	s_fUpdateDeltaFrameOriginal = 0.0f;
    
	s_bUpdateFirstFrame = false;

	ResetUpdateSpeedAnim();
}

//-----------------------------------
// [クラス] 更新準備
//-----------------------------------
void CUpdater::ReadyForUpdate( float deltaTime ){
	// 更新速度アニメ
	UpdateSpeedAnim( deltaTime );

	// ポーズ中でなければ更新
	if( !s_bUpdatePause ){
		// 実時間
		if( s_bUpdateByTime  ){
			s_fUpdateDeltaFrameOriginal = CConst::ConvSecToFrame( deltaTime );
		}
		// フレーム
		else{
			s_fUpdateDeltaFrameOriginal = 1.0f;	// 常に１フレーム経過
		}

		// 経過時間
		s_fUpdatePastFrame += s_fUpdateDeltaFrameOriginal;
	}
	else{
		s_fUpdateDeltaFrameOriginal = 0.0f;
	}

	// １フレーム目フラグオン
	s_bUpdateFirstFrame = true;

	// 経過時間をフレーム差分残り時間に設定
	s_fUpdateDeltaFrameRemain = s_fUpdateDeltaFrameOriginal;
}

//-----------------------------------
// [クラス] フレーム差分を確認して確定
//-----------------------------------
bool CUpdater::CheckThenFixDeltaFrame( void ){
	// 残りがあれば
	if( s_fUpdateDeltaFrameRemain > 0.0f ){
		if( s_fUpdateDeltaFrameRemain <= 1.0f ){
			s_fUpdateDeltaFrame = s_fUpdateDeltaFrameRemain;
			s_fUpdateDeltaFrameRemain = 0.0f;
		}else{
			s_fUpdateDeltaFrame = 1.0f;
			s_fUpdateDeltaFrameRemain -= 1.0f;
		}

		return( true );
	}

	// ここまで来たらフレーム差分無し
	s_fUpdateDeltaFrame = 0.0f;
	return( false );
}

//------------------------------
// [クラス] 更新速度アニメリセット
//------------------------------
void CUpdater::ResetUpdateSpeedAnim( void ){
	s_bUpdateSpeedAnimOn = false;
	s_fUpdateSpeedAnimSec = 0.0f;
	s_fUpdateSpeedAnimSecMax = 0.0f;
	s_fUpdateSpeedAnimFrom = 1.0f;
	s_fUpdateSpeedAnimTo = 1.0f;
}

//--------------------------
// [クラス] 更新速度アニメ設定
//--------------------------
void CUpdater::SetUpdateSpeedAnim( float sec, float speedFrom, float speedTo ){
	// カウントが無効なら即座に適用して返す
	if( sec <= 0 ){
		ResetUpdateSpeedAnim();
		SetUpdateSpeed( speedTo );
		return;
	}

	s_bUpdateSpeedAnimOn = true;
	s_fUpdateSpeedAnimSec = 0.0f;
	s_fUpdateSpeedAnimSecMax = sec;
	s_fUpdateSpeedAnimFrom = speedFrom;
	s_fUpdateSpeedAnimTo = speedTo;
}

//--------------------------
// [クラス] 更新速度アニメ更新
//--------------------------
void CUpdater::UpdateSpeedAnim( float deltaTime ){
	// 無効は無視
	if( ! s_bUpdateSpeedAnimOn ){
		return;
	}

	// 時間経過（※ここは秒で処理している）
	s_fUpdateSpeedAnimSec += deltaTime;

	// 指定の時間が経過した
	if( s_fUpdateSpeedAnimSec >= s_fUpdateSpeedAnimSecMax ){
		s_fUpdateSpeed = s_fUpdateSpeedAnimTo;
		ResetUpdateSpeedAnim();
		return;
	}

	// 時間が経過していない
	if( s_fUpdateSpeedAnimSec <= 0.0f ){
		s_fUpdateSpeed = s_fUpdateSpeedAnimFrom;
		return;
	}

    // ここまできたら更新速度を割合で設定
	float rate = s_fUpdateSpeedAnimSec/s_fUpdateSpeedAnimSecMax;
	s_fUpdateSpeed = s_fUpdateSpeedAnimFrom * (1.0f-rate) + s_fUpdateSpeedAnimTo * rate;
}

//-----------------------------------
// [メンバー] コンストラクタ
//-----------------------------------
CUpdater::CUpdater( void ){
	clearUpdater();
}

//-----------------------------------
// [メンバー] クリア
//-----------------------------------
void CUpdater::clearUpdater( void ){
	m_bUpdatePause = false;
	m_fUpdateSpeed = 1.0f;
	m_fUpdatePastFrame = 0.0f;
	m_fUpdateDeltaFrame = 0.0f;
    
	m_bUpdateWithOriginalDeltaFrame = false;

	resetUpdateWait();
	clearCount();

	m_bReservedUpdateDeltaFrame = false;
	m_fReservedUpdateDeltaFrame = 0.0f;
}

//----------------------------------------------------
// [メンバー] 更新
//----------------------------------------------------
void CUpdater::update( void ){
	// フレーム更新
	updateFrameForUpdate();

	// 待ち時間中なら無視
	if( updateWaitCountForUpdate() ){
		return;
	}

	// 派生先の処理を呼び出す
	update0();
}

//----------------------------------------------------
// [メンバー] フレーム更新
//----------------------------------------------------
void CUpdater::updateFrameForUpdate( void ){
	// 経過時間設定
	if( ! m_bUpdatePause ){
		// 指定があれば更新時間を差し替える
		if( m_bReservedUpdateDeltaFrame ){
			m_fUpdateDeltaFrame = m_fUpdateSpeed * m_fReservedUpdateDeltaFrame;
		}else{
			if( m_bUpdateWithOriginalDeltaFrame ){
				m_fUpdateDeltaFrame = m_fUpdateSpeed * s_fUpdateDeltaFrame;
			}else{
				m_fUpdateDeltaFrame = s_fUpdateSpeed * m_fUpdateSpeed * s_fUpdateDeltaFrame;
			}
		}
		m_fUpdatePastFrame += m_fUpdateDeltaFrame;
	}else{
		m_fUpdateDeltaFrame = 0.0f;
	}

	// フラグは都度クリア
	m_bReservedUpdateDeltaFrame = false;
}

//----------------------------------------------------
// [メンバー] 待ちカウント更新
//----------------------------------------------------
bool CUpdater::updateWaitCountForUpdate( void ){
	// 待ち時間があれば更新をスキップ
	if( m_fCountForUpdateWait > 0.0f ){
		m_fCountForUpdateWait -= m_fUpdateDeltaFrame;
		return( true );
	}

	return( false );
}

//----------------------------------------------------
// [メンバー] 割合計算（※一定周期での繰り返し動作用）
//----------------------------------------------------
float CUpdater::getUpdatePastFrameRate( float interval, bool isLoop ){
	// 無効は終了(1.0f)としておく
	if( interval <= 0 ){
		return( 1.0f );
	}

	float past = m_fUpdatePastFrame;

	// 周期を超過しいる場合
	if( past >= interval ){
		// ループ無しなら終了
		if( ! isLoop ){
			return( 1.0f );
		}

		// 端数の算出
		int loop = (int)(past/interval);
		past -= loop*interval;
	}

	// 割合にして返す
	float rate = past / interval;
	if( rate <= 0.0f ){ rate = 0.0f; }
	else if( rate >= 1.0f ){ rate = 1.0f; }

	return( rate );
}

//--------------------
// カウンタ割合い取得
//--------------------
float CUpdater::getCountRate( void ){
	if( m_fCount <= 0 ){
		return( 0.0f );
	}

	if( m_fCount >= m_fCountMax ){
		return( 1.0f );
	}

	return( m_fCount / m_fCountMax );
}

//-------------------
// 更新時間デルタ予約
//-------------------
void CUpdater::reserveUpdateDeltaFrame( float frame ){
	m_bReservedUpdateDeltaFrame = true;
	m_fReservedUpdateDeltaFrame = frame;
}
