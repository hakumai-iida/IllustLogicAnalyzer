/*+----------------------------------------------------------------+
  |	Title:		TouchNode.cpp [共通環境]
  |	Comment:	タッチノード基底
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "TouchNode.hpp"

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
// 更新
//----------------------------
void CTouchNode::onUpdate( void ){
	// 先にタッチ判定
	if( isActive() && m_nTapIgnoreFrame <= 0 ){
		scanTouch();
	}else{
		clearTouch();
	}

	// 親の更新
	CNode::onUpdate();

	// タッチ無視フレームのデクリメント
	if( m_nTapIgnoreFrame > 0 ){
		m_nTapIgnoreFrame--;
	}
}

//----------------------------
// 活動指定
//----------------------------
void CTouchNode::setActive( bool flag ){
	bool isActive0 = isActive();
	CNode::setActive( flag );

	// 活動へ切り替わった際は若干の入力無視フレームを設ける
	if( isActive() && !isActive0 ){ m_nTapIgnoreFrame = 2; }

	// 活動が停止したら[BACK]ボタン監視をオフ
	if( !isActive() ){ m_bCheckBackButton = false; }
}

//-----------------------------------------
// タッチ領域設定：矩形：[x, y]は矩形の左上
//-----------------------------------------
void CTouchNode::setTouchRect( float x, float y, float w, float h ){
	m_nTouchArea = eTOUCH_AREA_RECT;
	m_fTouchX = x;
	m_fTouchY = y;
	m_fTouchW = w;
	m_fTouchH = h;
}

//-------------------------------------
// タッチ領域設定：円：[x, y]は円の左上
//-------------------------------------
void CTouchNode::setTouchOval( float x, float y, float w, float h ){
	m_nTouchArea = eTOUCH_AREA_OVAL;
	m_fTouchX = x;
	m_fTouchY = y;
    m_fTouchW = w;
    m_fTouchH = h;
}

//------------------
// タッチ確認
//------------------
void CTouchNode::scanTouch( void ){
	float x = m_fX + m_fTouchX;
	float y = m_fY + m_fTouchY;

	if( m_nTouchArea == eTOUCH_AREA_OVAL ){
		m_pCellPressed = CTouchMgr::CheckOval( eTOUCH_CHECK_PRESSED, x, y, m_fTouchW, m_fTouchH );
		m_pCellMoved = CTouchMgr::CheckOval( eTOUCH_CHECK_MOVED, x, y, m_fTouchW, m_fTouchH );
		m_pCellTouchedDown = CTouchMgr::CheckOval( eTOUCH_CHECK_TOUCHED_DOWN, x, y, m_fTouchW, m_fTouchH );
		m_pCellTouchedUp = CTouchMgr::CheckOval( eTOUCH_CHECK_TOUCHED_UP, x, y, m_fTouchW, m_fTouchH );
		m_pCellTapped = CTouchMgr::CheckOval( eTOUCH_CHECK_TAPPED, x, y, m_fTouchW, m_fTouchH );
		m_pCellFlicked = CTouchMgr::CheckOval( eTOUCH_CHECK_FLICKED, x, y, m_fTouchW, m_fTouchH );
	}else{
		m_pCellPressed = CTouchMgr::CheckRect( eTOUCH_CHECK_PRESSED, x, y, m_fTouchW, m_fTouchH );
		m_pCellMoved = CTouchMgr::CheckRect( eTOUCH_CHECK_MOVED, x, y, m_fTouchW, m_fTouchH );
		m_pCellTouchedDown = CTouchMgr::CheckRect( eTOUCH_CHECK_TOUCHED_DOWN, x, y, m_fTouchW, m_fTouchH );
		m_pCellTouchedUp = CTouchMgr::CheckRect( eTOUCH_CHECK_TOUCHED_UP, x, y, m_fTouchW, m_fTouchH );
		m_pCellTapped = CTouchMgr::CheckRect( eTOUCH_CHECK_TAPPED, x, y, m_fTouchW, m_fTouchH );
		m_pCellFlicked = CTouchMgr::CheckRect( eTOUCH_CHECK_FLICKED, x, y, m_fTouchW, m_fTouchH );
	}
}

//------------------
// 結果のクリア
//------------------
void CTouchNode:: clearTouch( void ){
	m_pCellPressed = NULL;
	m_pCellMoved = NULL;
	m_pCellTouchedDown = NULL;
	m_pCellTouchedUp = NULL;
	m_pCellTapped = NULL;
	m_pCellFlicked = NULL;
}
