/*+----------------------------------------------------------------+
  |	Title:		GestureLoop.hpp [共通環境]
  |	Comment:	ジェスチャーループ（※スワイプ等による値の変動基本）
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#ifndef __GESTURE_LOOP_HPP__
#define __GESTURE_LOOP_HPP__

/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "Loop.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Struct		構造体型宣言
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Class		クラス定義
  +----------------------------------------------------------------+*/
//----------------------------------------
// バンクテクスチャ確認ループ
//----------------------------------------
class CGestureLoop: public CLoop{
protected:
    // 入力受付判定
	int m_nGestureBaseX;
	int m_nGestureBaseY;
	int m_nGestureBaseR;

	// 移動判定（※ドラッグ）
	bool m_bGestureMove;
	float m_fGestureMoveX;
	float m_fGestureMoveY;
    float m_fGestureLogX;
    float m_fGestureLogY;

	// スケール判定（※ピンチ）
	bool m_bGesturePinch;
    float m_fGesturePinchLen0;
	float m_fGestureScale;
    float m_fGestureLogScale;
    
public:
	CGestureLoop( void ){}
	virtual ~CGestureLoop( void ){}

protected:
	void setGestureBase( int x, int y, int r );

	void setGestureMoveXY( float x, float y );
	bool checkGestureMove( bool isUpdateSkip=false );
    float getGestureLogX( void ){ return( m_fGestureLogX ); }
    float getGestureLogY( void ){ return( m_fGestureLogY ); }

	void setGestureScale( float s );
    bool checkGestureScale( bool isUpdateSkip=false );
    float getGestureLogScale( void ){ return( m_fGestureLogScale ); }

	void drawGestureBase( int32 rgba );
};

/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/

#endif /* __GESTURE_LOOP_HPP__ */
