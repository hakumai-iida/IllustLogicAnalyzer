/*+----------------------------------------------------------------+
  |	Title:		Node.cpp [共通環境]
  |	Comment:	ノード基底（※ループにより更新＆描画管理されるもの）
  |             開発／デバッグ用の表示物（※ゲーム表示要素の[CObj]とは別実装）
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "Node.hpp"

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
//----------------------------
// 配置
//----------------------------
void CNode::put( float x, float y ){
	m_fX = x;
	m_fY = y;
}

//----------------------------
// 表示指定
//----------------------------
void CNode::setVisible( bool flag ){
	m_bVisible = flag;
}

//-------------------------------------
// 表示確認（※[m_bVisible]への直参照はダメ）
//-------------------------------------
bool CNode::isVisible( void ){
	return( m_bVisible );
}

//----------------------------
// 活動指定
//----------------------------
void CNode::setActive( bool flag ){
	m_bActive = flag;

	// 活動指定時はすぐに暗転を解除
	if( m_bActive ){ setDark( false ); }
}

//----------------------------------------
// 活動確認（※[m_bActive]への直参照はダメ）
//----------------------------------------
bool CNode::isActive( void ){
	return( isVisible() && !m_bDark && m_bActive );
}

//----------------------------
// 暗転指定
//----------------------------
void CNode::setDark( bool flag ){
	m_bDark = flag;
}

//----------------------------------------
// 活動確認（※[m_bDark]への直参照はダメ）
//----------------------------------------
bool CNode::isDark( void ){
	return( m_bDark );
}

//--------
// 入場
//--------
void CNode::enter( void ){
	setActive( true );
	setVisible( true );
}

//--------
// 退場
//--------
void CNode::exit( void ){
	setActive( false );
	setVisible( false );
}

//---------------------------------
// 更新（※可視なら更新実体を呼ぶ）
//---------------------------------
void CNode::onUpdate( void ){
	if( isVisible() ){
		onUpdate0();
	}

	// 非活動指定時は暗転を設定（※暗転時は１フレ遅れることになる）
	if( !isActive() && !isDark() ){ setDark( true ); }
}

//---------------------------------
// 表示（※可視なら表示実体を呼ぶ）
//---------------------------------
void CNode::onDraw( void ){
	if( isVisible() ){
		onDraw0();
	}
}