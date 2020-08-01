/*+----------------------------------------------------------------+
  |	Title:		ScrollBarMenu.hpp [共通環境]
  |	Comment:	スクロールバー付きメニュー（※基本的にデバッグ処理用）
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "ScrollBarMenu.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Struct		構造体定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Program		プログラム実装
  +----------------------------------------------------------------+*/
//-------------------
// コンストラクタ
//-------------------
CScrollBarMenu::CScrollBarMenu( eGRP grpId, int cW, int cH ): CMenu( grpId, cW, cH, true, 1 ){
}

//-------------------
// デストラクタ
//-------------------
CScrollBarMenu::~CScrollBarMenu( void ){
}

//------------------
// 入場
//------------------
void CScrollBarMenu::enter( void ){
	CMenu::enter();

	// スクロールバーが有効であれば
	if( !m_oScrollBar.isFill() ){
		m_oScrollBar.put( m_fX + m_fCenterW + 10, m_fY + 21 );	// 座標はその都度設定
		m_oScrollBar.enter();
    }else{
        m_oScrollBar.exit();
    }
}

//------------------
// 退場
//------------------
void CScrollBarMenu::exit( void ){
	CMenu::exit();
	m_oScrollBar.exit();
}

//------------------
// 動作指定
//------------------
void CScrollBarMenu::setActive( bool flag ){
	CMenu::setActive( flag );
	m_oScrollBar.setActive( flag );
}

//-------------------
// メニュー確定
//-------------------
void CScrollBarMenu::fixMenu( void ){
	CMenu::fixMenu();

	// スクロールバー設定
	m_oScrollBar.setForDisp( true, 2, m_fCenterH, 1 );
	m_oScrollBar.setForScroll( m_fInternalH, m_fCenterH );
	m_oScrollBar.setScrollOfs( m_fInternalOfsY );

	// フリック値調整
	setFlickAdjX( 1.0f );
	setFlickAdjY( 1.0f + (m_fInternalH/m_fCenterH)/2.0f );
}

//-------------------
// 更新
//-------------------
void CScrollBarMenu::onUpdate0( void ){
	CMenu::onUpdate0();

	// スクロールバー調整
	m_oScrollBar.setScrollOfs( m_fInternalOfsY );
	m_oScrollBar.setActive( isActive() );
	m_oScrollBar.onUpdate();
}

//-------------------
// 描画
//-------------------
void CScrollBarMenu::onDraw0( void ){
	CMenu::onDraw0();

	// スクロールバー表示
	m_oScrollBar.onDraw();
}
