/*+----------------------------------------------------------------+
  |	Title:		TouchNode.hpp [共通環境]
  |	Comment:	タッチノード基底
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#ifndef __TOUCH_NODE_HPP__
#define __TOUCH_NODE_HPP__

/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "Node.hpp"

#include "touch/TouchMgr.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
// タッチ判定タイプ
enum eTOUCH_AREA{
	eTOUCH_AREA_RECT,	    // 矩形
	eTOUCH_AREA_OVAL,	    // 円
};

/*+----------------------------------------------------------------+
  |	Struct		構造体型宣言
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Class		クラス定義
  +----------------------------------------------------------------+*/
//----------------------------------------
// タッチノード基底
//----------------------------------------
class CTouchNode: public CNode{
protected:
	// 判定用
	int m_nTouchArea;
	float m_fTouchX, m_fTouchY;
	float m_fTouchW, m_fTouchH;

	// 判定結果
	CTouchStatusCell* m_pCellPressed;
	CTouchStatusCell* m_pCellMoved;
	CTouchStatusCell* m_pCellTouchedDown;
	CTouchStatusCell* m_pCellTouchedUp;
	CTouchStatusCell* m_pCellTapped;
	CTouchStatusCell* m_pCellFlicked;

	// [BACK]ボタンへ関連付け
	bool m_bCheckBackButton;

	// 入力無視フレーム
	int m_nTapIgnoreFrame;

public:
	CTouchNode( void ){}
	virtual ~CTouchNode( void ){}

    //--------------------------------------
	// 判定
    //--------------------------------------
    // 押されているか？
	inline virtual bool isPressed( void ){ return( m_pCellPressed != NULL ); }
    
    // 長押しされているか？
	inline virtual bool isPressedLong( void ){
		if( m_pCellPressed != NULL ){ return( m_pCellPressed->isPressedLong() ); }
		return( false );
	}
    
    // 移動（ドラッグ）したか？
	inline virtual bool isMoved( void ){ return( m_pCellMoved != NULL ); }

    // フリックされたか？
    inline virtual bool isFlicked( void ){ return( m_pCellFlicked != NULL ); }

    // 押下されたか？
	inline virtual bool isTouchedDown( bool isCheckBackButton = true ){
		if( isCheckBackButton && m_bCheckBackButton && CTouchMgr::CheckBackButtonDown() ){ return( true ); }
		return( m_pCellTouchedDown != NULL );
	}
    
    // 押上されたか？
	inline virtual bool isTouchedUp( bool isCheckBackButton = true ){
		if( isCheckBackButton && m_bCheckBackButton && CTouchMgr::CheckBackButtonUp() ){ return( true ); }
		return( m_pCellTouchedUp != NULL );
	}
    
    // タップされたか？
	inline virtual bool isTapped( bool isCheckBackButton = true ){
		if( isCheckBackButton && m_bCheckBackButton && CTouchMgr::CheckBackButton() ){ return( true ); }
		return( m_pCellTapped != NULL );
	}

	// 取得
	inline virtual CTouchStatusCell* getCellPressed( void ){ return( m_pCellPressed ); }
	inline virtual CTouchStatusCell* getCellMoved( void ){ return( m_pCellMoved ); }
	inline virtual CTouchStatusCell* getCellTouchedDown( void ){ return( m_pCellTouchedDown ); }
	inline virtual CTouchStatusCell* getCellTouchedUp( void ){ return( m_pCellTouchedUp ); }
	inline virtual CTouchStatusCell* getCellTapped( void ){ return( m_pCellTapped ); }
	inline virtual CTouchStatusCell* getCellFlicked( void ){ return( m_pCellFlicked ); }

	// 幅／高さ
	inline virtual float getW( void ){ return( m_fTouchW ); }
	inline virtual float getH( void ){ return( m_fTouchH ); }

	// 更新
	virtual void onUpdate( void );

	// 上書き（※[BACK]ボタン制御）
	virtual void setActive( bool flag );

	// 設定
	virtual void setTouchRect( float x, float y, float w, float h );
	virtual void setTouchOval( float x, float y, float w, float h );

	inline virtual void setCheckBackButton( bool flag ){ m_bCheckBackButton = flag; }

protected:
	virtual void scanTouch( void );
	virtual void clearTouch( void );
};

/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/

#endif /* __TOUCH_NODE_HPP__ */
