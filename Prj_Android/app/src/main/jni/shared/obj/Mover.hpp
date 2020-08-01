/*+----------------------------------------------------------------+
  |	Title:		Mover.hpp [共通環境]
  |	Comment:	シンプルな移動系
  |				[CUpdater]は継承させずシンプルな座標計算のみ行う
  |				[CObj]の派生クラス等でアニメさせたい場合は、
  |				このクラスのインスタンスをメンバーとして使う想定
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#ifndef __MOVER_HPP__
#define __MOVER_HPP__

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
// Mover
//-----------------------------------------------------------------
class CMover{
protected:
	bool  m_bStarted;			// 開始済みか？
	bool  m_bMoving;			// 移動中か？
	bool  m_bChecked;			// チェックフラグ（※汎用フラグ）
	float m_fMoveFrameMax;		// 最大フレーム（移動が終了するフレーム）
	float m_fMoveFrame;			// 現在の経過フレーム
	float m_fWaitFrame;			// 待ち時間

	eASYNC_TYPE m_eAsyncType;	// 同期タイプ
	float m_fAsyncPow;			// 同期の強さ

	// 座標関連
	float m_fCurX, m_fCurY;		// 現在位置
	float m_fFromX, m_fFromY;	// 開始位置
	float m_fToX, m_fToY;		// 目的位置

	// アルファ
	BYTE m_nCurAlpha;			// 現在透過度
	BYTE m_nFromAlpha;			// 開始透過度
	BYTE m_nToAlpha;			// 目的透過度
    
    // 色
    DWORD m_nCurRGBA;           // 現在色
    DWORD m_nFromRGBA;          // 開始色
    DWORD m_nToRGBA;            // 目的色

public:
	// コンストラクタ＆デストラクタ（※基本的に何もしない）
	CMover( void );
	virtual ~CMover( void ){}

	// クリア（※初期化用）
	void clearMover( void );
    
	// 設定
	void setAsyncType( eASYNC_TYPE type, float pow );

	// 座標
	void fixXY( float x, float y );
	void setFromToXY( float fX, float fY, float tX, float tY );
	void setFromCurToXY( float tX, float tY );

	// アルファ
	void fixAlpha( BYTE a );
	void setFromToAlpha( BYTE fA, BYTE tA );
	void setFromCurToAlpha( BYTE tA );
    
    // 色
    void fixRGBA( DWORD col );
    void setFromToRGBA( DWORD fCol, DWORD tCol );
    void setFromCurToRGBA( DWORD tCol );

	// 取得
	float getX( void ){ return( m_fCurX ); }
	float getY( void ){ return( m_fCurY ); }
	BYTE getAlpha( void ){ return( m_nCurAlpha ); }
    DWORD getRGBA( void ){ return( m_nCurRGBA ); }

	// 判定
	bool isStarted( void ){ return( m_bStarted ); }
	bool isMoving( void ){ return( m_bMoving ); }
	bool isChecked( void ){ return( m_bChecked ); }

	// 開始／再開／停止
	bool start( float frame, float initial=0.0f );
	bool restart( void );
	void stop( void );
	void stopTo( void );
	inline void setWait( float wait ){ m_fWaitFrame = wait; }

	// チェックフラグ設定
	void setChecked( bool flag ){ m_bChecked = flag; }

	// 更新（移動）
	bool updateWithFrame( float deltaFrame );
};

/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
#endif	// __MOVER_HPP__
