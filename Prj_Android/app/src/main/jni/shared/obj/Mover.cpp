/*+----------------------------------------------------------------+
  |	Title:		Mover.cpp [共通環境]
  |	Comment:	シンプルな移動系
  |				[CUpdater]は継承させずシンプルな座標計算のみ行う
  |				[CObj]の派生クラス等でアニメさせたい場合は、
  |				このクラスのインスタンスをメンバーとして使う想定
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "Mover.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Struct		構造体型宣言
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Program		プログラム実装
  +----------------------------------------------------------------+*/
//------------------------
// コンストラクタ
//------------------------
CMover::CMover( void ){
	clearMover();
}

//------------------------
// クリア
//------------------------
void CMover::clearMover(){
	m_bStarted = false;
	m_bMoving = false;
	m_bChecked = false;
	m_fMoveFrameMax = m_fMoveFrame = 0;
	m_fWaitFrame = 0;

	m_eAsyncType = eASYNC_TYPE_LINEAR;  // デフォルト
	m_fAsyncPow = 0.5f;                 // デフォルト

	fixXY( 0.0f, 0.0f );
	fixAlpha( 0xFF );
    fixRGBA( 0xFFFFFF );
}

//------------------------
// 設定：同期タイプ
//------------------------
void CMover::setAsyncType( eASYNC_TYPE type, float pow ){
	m_eAsyncType = type;
	m_fAsyncPow = pow;
}

//------------------------
// 設定：座標
//------------------------
void CMover::fixXY( float x, float y ){
	m_fCurX = m_fFromX = m_fToX = x;
	m_fCurY = m_fFromY = m_fToY = y;
}

void CMover::setFromToXY( float fX, float fY, float tX, float tY ){
	m_fCurX = m_fFromX = fX;
	m_fCurY = m_fFromY = fY;
	m_fToX = tX;
	m_fToY = tY;
}

void CMover::setFromCurToXY( float tX, float tY ){
	m_fFromX = m_fCurX;
	m_fFromY = m_fCurY;
	m_fToX = tX;
	m_fToY = tY;
}

//------------------------
// 設定：アルファ
//------------------------
void CMover::fixAlpha( BYTE a ){
	m_nCurAlpha = m_nFromAlpha = m_nToAlpha = a;
}

void CMover::setFromToAlpha( BYTE fA, BYTE tA ){
	m_nCurAlpha = m_nFromAlpha = fA;
	m_nToAlpha = tA;
}

void CMover::setFromCurToAlpha( BYTE tA ){
	m_nFromAlpha = m_nCurAlpha;
	m_nToAlpha = tA;
}

//------------------------
// 設定：色
//------------------------
void CMover::fixRGBA( DWORD col ){
    m_nCurRGBA = m_nFromRGBA = m_nToRGBA = col;
}

void CMover::setFromToRGBA( DWORD fCol, DWORD tCol ){
    m_nCurRGBA = m_nFromRGBA = fCol;
    m_nToRGBA = tCol;
}

void CMover::setFromCurToRGBA( DWORD tCol ){
    m_nFromRGBA = m_nCurRGBA;
    m_nToRGBA = tCol;
}

//-----------------
// 開始
//-----------------
bool CMover::start( float frame, float initial ){
	m_bStarted = true;
	m_bMoving = true;
	m_fMoveFrameMax = frame;
	m_fMoveFrame = initial;
	return( updateWithFrame( 0.0f ) );
}

//-----------------
// 再開
//-----------------
bool CMover::restart( void ){
	return( start( m_fMoveFrameMax, m_fMoveFrame ) );
}

//-----------------
// 停止
//-----------------
void CMover::stop( void ){
	m_bMoving = false;
}

//-----------------
// 停止：目的地へ
//-----------------
void CMover::stopTo( void ){
	m_fMoveFrame = m_fMoveFrameMax;
	updateWithFrame( 0.0f );
}

//-----------------
// 移動（更新）
//-----------------
bool CMover::updateWithFrame( float deltaFrame ){
	// 移動中でなければ無視
	if( !m_bMoving ){
		return( false );
	}

	if( m_fWaitFrame > 0 ){
		m_fWaitFrame -= deltaFrame;
		return( true );
	}

	if( deltaFrame > 0 ){
		m_fMoveFrame += deltaFrame;
	}

	// 無効/時間経過であれば即座に停止
	if( m_fMoveFrameMax <= 0 || m_fMoveFrame >= m_fMoveFrameMax ){
		m_fCurX = m_fToX;
		m_fCurY = m_fToY;
		m_nCurAlpha = m_nToAlpha;
		m_bMoving = false;
		return( false );
	}

	// ここまで来たら移動途中
	float rate = m_fMoveFrame/m_fMoveFrameMax;
	rate = CConst::ApplyAsyncType( m_eAsyncType, rate, m_fAsyncPow );

    m_fCurX = CConst::CalcAsyncF( rate, m_fFromX, m_fToX );
    m_fCurY = CConst::CalcAsyncF( rate, m_fFromY, m_fToY );
    m_nCurAlpha = CConst::CalcAsyncB( rate, m_nFromAlpha, m_nToAlpha );
    m_nCurRGBA = CConst::CalcAsyncRGBA( rate, m_nFromRGBA, m_nToRGBA );

	return( true );
}
