/*+----------------------------------------------------------------+
  |	Title:		Updater.hpp [共通環境]
  |	Comment:	要素更新基底
  |				更新を行う処理系の基本
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#ifndef __UPDATER_HPP__
#define __UPDATER_HPP__

/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Struct		構造体定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Class		クラス定義
  +----------------------------------------------------------------+*/
//-----------------------------------------------------------------
// Updater
//-----------------------------------------------------------------
class CUpdater{
	//-------------------------
	// [static] 全体管理用
	//-------------------------
protected:
	static bool  s_bUpdateByTime;				// 時間更新か？（※フレーム飛びするか？）
	static bool  s_bUpdatePause;				// 停止フラグ
	static float s_fUpdateSpeed;				// 再生速度：1.0fで100%
	static float s_fUpdatePastFrame;			// 経過フレーム
	static float s_fUpdateDeltaFrame;			// フレーム差分（※処理に利用する値）
	static float s_fUpdateDeltaFrameRemain;		// フレーム差分（※残りの値）
	static float s_fUpdateDeltaFrameOriginal;	// フレーム差分（※本来の値）

	// １フレーム目か？（※処理落ち等で１ループ中に複数フレームが呼ばれる際の判定用）
	static bool  s_bUpdateFirstFrame;

	// 更新速度アニメ用
	static bool  s_bUpdateSpeedAnimOn;
	static float s_fUpdateSpeedAnimSec;
	static float s_fUpdateSpeedAnimSecMax;
	static float s_fUpdateSpeedAnimFrom;
	static float s_fUpdateSpeedAnimTo;

public:
	static void ResetAll( void );
	static void ReadyForUpdate( float deltaTime );
	static bool CheckThenFixDeltaFrame( void );

    // 設定
	inline static void SetUpdateByTime( bool flag ){ s_bUpdateByTime = flag; }
	inline static void SwapUpdateByTime( void ){ s_bUpdateByTime = !s_bUpdateByTime; }
	inline static void SetUpdatePause( bool flag ){ s_bUpdatePause = flag; }
	inline static void SetUpdateSpeed( float speed ){ s_fUpdateSpeed = speed; }

    // 判定／取得
	inline static bool  IsUpdateByTime( void ){ return( s_bUpdateByTime ); }
	inline static bool  IsUpdatePaused( void ){ return( s_bUpdatePause ); }
	inline static float GetUpdateSpeed( void ){ return( s_fUpdateSpeed ); }
	inline static float GetUpdatePastFrame( void ){ return( s_fUpdatePastFrame ); }
	inline static float GetUpdateDeltaFrame( void ){ return( s_fUpdateDeltaFrame ); }
	inline static float GetUpdateDeltaFrameRemain( void ){ return( s_fUpdateDeltaFrameRemain ); }
	inline static float GetUpdateDeltaFrameOriginal( void ){ return( s_fUpdateDeltaFrameOriginal ); }

    // 初回フレーム
	inline static void SetUpdateFirstFrameDone( void ){ s_bUpdateFirstFrame = false; }
	inline static bool IsUpdateFirstFrame( void ){ return( s_bUpdateFirstFrame ); }

	// 更新速度アニメ
	static void ResetUpdateSpeedAnim( void );
	static void SetUpdateSpeedAnim( float sec, float speedFrom, float speedTo );
	static void UpdateSpeedAnim( float deltaTime );

	//-------------------------
	// [member] メンバー
	//-------------------------
protected:
	bool  m_bUpdatePause;				// 停止中か？
	float m_fUpdateSpeed;				// 再生速度：1.0fで100%
	float m_fUpdatePastFrame;			// 経過フレーム
	float m_fUpdateDeltaFrame;			// フレーム差分（※更新用の値：[update0]で経過時間差分の判定等をするための値）

	// 速度影響を受けない（※ポーズ中でも動かす際等）
	bool m_bUpdateWithOriginalDeltaFrame;

	// 更新待ちカウンタ（※この値が有効な間は[update]がスキップされる）
	float  m_fCountForUpdateWait;

	// 汎用カウンタ（※[CObje]派生先で利用する汎用時間待ち処理用）
	float  m_fCount;
	float  m_fCountMax;

	// 更新フレーム横取り（※本来の時間軸とは違う速度で動かしたい時用）
	bool  m_bReservedUpdateDeltaFrame;
	float m_fReservedUpdateDeltaFrame;

public:
	// コンストラクタ＆デストラクタ
	CUpdater( void );
	virtual ~CUpdater( void ){}

    // クリア（※初期化用）
    void clearUpdater( void );

	// 設定
	inline void setUpdatePause( bool flag ){ m_bUpdatePause = flag; }
	inline void swapUpdatePause( void ){ m_bUpdatePause = !m_bUpdatePause; }
	inline void setUpdateSpeed( float speed ){ m_fUpdateSpeed = speed; }
	inline void resetUpdatePastFrame( void ){ m_fUpdatePastFrame = 0.0f; }
	inline void setUpdateWithOriginalDeltaFrame( bool flag ){ m_bUpdateWithOriginalDeltaFrame = flag; }

	// 取得
	inline bool  isUpdatePaused( void ){ return( m_bUpdatePause || s_bUpdatePause ); }
	inline float getUpdateSpeed( void ){ return( m_fUpdateSpeed ); }
	inline float getUpdatePastFrame( void ){ return( m_fUpdatePastFrame ); }
	inline float getUpdateDeltaFrame( void ){ return( m_fUpdateDeltaFrame ); }
	inline bool  isUpdateWithOriginalDeltaFrame( void ){ return( m_bUpdateWithOriginalDeltaFrame ); }

	// [m_fUpdatePastFrame]の指定[interval]における到達率を[0.0-1.0f]で返す（※単純な周期アニメ等で利用）
	float getUpdatePastFrameRate( float interval, bool isLoop=true );

    //-------------
	// 更新（※窓口）
    //-------------
	virtual void update( void );

	// 更新待ち
	inline void resetUpdateWait( void ){ m_fCountForUpdateWait = 0.0f; }
	inline void setUpdateWait( float wait ){ m_fCountForUpdateWait = wait; }

	//-------------
	// カウンタ関連
	//-------------
	inline void clearCount( void ){ m_fCountMax = m_fCount = 0.0f; }
    inline void resetCount( void ){ m_fCount = 0.0f; }
	inline void setCountMax( float max, float initial=0.0f ){ m_fCountMax = max; m_fCount = initial; }
	inline void setCount( float count ){ m_fCount = count; }

	// カウンタの変動があるか？
	inline bool isCountUpable( void ){ return( CConst::IsIncrementableToMax( m_fCount, m_fCountMax ) ); }
	inline bool isCountDownable( void ){ return( CConst::IsDecrementableToZero( m_fCount ) ); }

	// カウントアップ／ダウン（※返値は目的の値に到達しているかどうか？）
	inline bool countUp( float rate=1.0f ){ return( CConst::IncrementToMax( &m_fCount, m_fCountMax, rate*m_fUpdateDeltaFrame ) ); }
	inline bool countDown( float rate=1.0f ){ return( CConst::DecrementToZero( &m_fCount, rate*m_fUpdateDeltaFrame ) ); }

	// [m_fCountMax]に対する[m_fCount]の到達率（※[0.0-1.0]までの値）
	float getCountRate( void );

	//----------------
	// 更新時間横取り予約
	//----------------
	void reserveUpdateDeltaFrame( float frame );

protected:
	// フレーム更新
	void updateFrameForUpdate( void );

	// 待ち時間更新（※返値は待機中であるか？）
	bool updateWaitCountForUpdate( void );

	// 実更新（※派生先で上書きする）
	virtual void update0( void ){}
};

/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
#endif	// __UPDATER_HPP__
