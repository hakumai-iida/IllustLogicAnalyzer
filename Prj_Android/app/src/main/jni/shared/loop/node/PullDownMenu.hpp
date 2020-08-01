/*+----------------------------------------------------------------+
  |	Title:		PullDownMenu.hpp [共通環境]
  |	Comment:	プルダウンメニュー（※基本的にデバッグ処理用）
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#ifndef __PULL_DOWN_MENU_HPP__
#define __PULL_DOWN_MENU_HPP__

/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "StrMenu.hpp"
#include "Button.hpp"

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
// プルダウンメニュー
//----------------------------------------
class CPullDownMenu: public CStrMenu{
protected:
    float m_fBaseX, m_fBaseY;

    // 開閉ボタン
    CButton m_oButton;

    // 動作指定
	bool m_bExpandDirUp;            // 向き（※上に拡張するか？）
    bool m_bPutButtonRight;         // ボタンを右に置くか？
    bool m_bAutoCloseIfDecided;     // 選択時に自動で閉じるか？

	// 拡張管理
	bool m_bExpand;
	int  m_nExpandCount;
    
public:
    // コンストラクタ／デストラクタ（※基本的な動作はコンストラクタでのみ設定する想定）
	CPullDownMenu( bool isDirUp, bool isButtonRight, bool isAutoClose );
	virtual ~CPullDownMenu( void );

    virtual void fixMenu( void );
    virtual void setMaxCenterWH( int w, int h );

    virtual void put( float x, float y );
	virtual void enter( void );
	virtual void exit( void );
	virtual void setActive( bool isActive );

	bool isExpand( void ){ return( m_bExpand ); }

protected:
    virtual void setCenterWH( float w, float h );
	virtual float calcInternalW( void );
	virtual float calcInternalH( void );
	virtual float getItemTapH( void );

	virtual void onUpdate0( void );
	virtual void onDraw0( void );
    
    void fixBaseMenu( void );
    void fixScrollBar( void );
    void fixButton( void );
};

/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
#endif /* __PULL_DOWN_MENU_HPP__ */
