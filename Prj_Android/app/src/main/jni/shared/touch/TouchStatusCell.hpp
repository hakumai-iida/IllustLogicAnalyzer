/*+----------------------------------------------------------------+
  |	Title:		TouchStatusCell.hpp [共通環境]
  |	Comment:	タッチ個々の状態要素（※管理環境における指一本の情報）
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#ifndef __TOUCH_STATUS_CELL_HPP__
#define __TOUCH_STATUS_CELL_HPP__

/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
#define	TOUCH_STATUS_CELL_TAP_CHECK_FRAME		    30
#define TOUCH_STATUS_CELL_PRESS_LONG_FRAME	        32
#define TOUCH_STATUS_CELL_MOVE_LOG_NUM               4

/*+----------------------------------------------------------------+
  |	Struct		構造体定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Class		クラス定義
  +----------------------------------------------------------------+*/
//----------------------------------------
// タッチ状態
//----------------------------------------
class CTouchStatusCell{
protected:
	bool m_bActive;			    // 活動中か？
	bool m_bTapped;			    // タップされたか？
	bool m_bPressed;		    // 押されているか？
	bool m_bTouchedDown;	    // 押されたか？
	bool m_bTouchedUp;		    // 離されたか？
	bool m_bMoved;			    // 動いたか？
	bool m_bFlicked;		    // フリックされたか？
	bool m_bStable;			    // 安定しているか？（※タップ／長押しが有効か？）

	float m_fX, m_fY;			// 座標
	float m_fX0, m_fY0;		    // 座標（直前）
	float m_fBaseX, m_fBaseY;	// タップ基準位置
	float m_fMoveX, m_fMoveY;	// 移動値（移動時）

	// フリック判定用
    float m_fArrLogMoveX[TOUCH_STATUS_CELL_MOVE_LOG_NUM];
	float m_fArrLogMoveY[TOUCH_STATUS_CELL_MOVE_LOG_NUM];
	float m_fFlickX, m_fFlickY;

	int m_nId;				    // 識別子
	int m_nTapTimer;		    // タップ受付時間
	int m_nPressTimer;		    // 押し下時間

	// テンポラリ
	bool m_bTempMoved;
	bool m_bTempTouchedDown;
	bool m_bTempTouchedUp;
	bool m_bTempTouchedDownUp;
	float m_fTempX, m_fTempY;

public:
    // コンストラクタ／デストラクタ
	CTouchStatusCell( void );
    virtual ~CTouchStatusCell( void ){}

    void clear( void );

    inline void setId( int id ){ m_nId = id; }

	void update( void );

	void onTouchedDown( float x, float y );
	void onTouchedUp( float x, float y );
	void onMoved( float x, float y );

	bool isCurPointInRect( float x, float y, float w, float h );
	bool isStartPointInRect( float x, float y, float w, float h );
	bool isCurPointInOval( float x, float y, float w, float h );
	bool isStartPointInOval( float x, float y, float w, float h );

	// 判定
	inline bool isActive( void ){ return( m_bActive ); }
	inline bool isTapped( void ){ return( m_bTapped ); }
	inline bool isPressed( void ){ return( m_bPressed ); }
	inline bool isTouchedDown( void ){ return( m_bTouchedDown ); }
	inline bool isTouchedUp( void ){ return( m_bTouchedUp ); }
	inline bool isMoved( void ){ return( m_bMoved ); }
	inline bool isFlicked( void ){ return( m_bFlicked ); }
	inline bool isStable( void ){ return( m_bStable ); }
	inline bool isPressedLong( void ){ return( m_bPressed && m_bStable && m_nPressTimer >= TOUCH_STATUS_CELL_PRESS_LONG_FRAME ); }

	// 取得
	inline int getId( void ){ return( m_nId ); }
	inline float getX( void ){ return( m_fX ); }
	inline float getY( void ){ return( m_fY ); }
	inline float getX0( void ){ return( m_fX0 ); }
	inline float getY0( void ){ return( m_fY0 ); }
	inline float getBaseX( void ){ return( m_fBaseX ); }
	inline float getBaseY( void ){ return( m_fBaseY ); }
	inline float getMoveX( void ){ return( m_fMoveX ); }
	inline float getMoveY( void ){ return( m_fMoveY ); }
	inline float getFlickX( void ){ return( m_fFlickX ); }
	inline float getFlickY( void ){ return( m_fFlickY ); }
	inline int getPressTimer( bool checkStable ){ return( (!checkStable || m_bStable )? m_nPressTimer: 0 ); }

protected:
	void clearTemp( void );
	void clearLogMoveXY( void );
	void pushLogMoveXY( float x, float y );

	float getLogMoveX( void );
	float getLogMoveY( void );
};

/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
#endif	// __TOUCH_STATUS_CELL_HPP__
