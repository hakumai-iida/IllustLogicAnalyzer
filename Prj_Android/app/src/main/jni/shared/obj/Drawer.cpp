/*+----------------------------------------------------------------+
  |	Title:		Drawer.cpp [共通環境]
  |	Comment:	ゲーム要素表示基底
  |				（※このクラスは描画順の管理用なので位置情報はもたない）
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "Drawer.hpp"

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
eDRAW_PROCESS CDrawer::s_eDrawProcess;
CDrawer *CDrawer::s_pDrawHead;

/*+----------------------------------------------------------------+
  |	Program		プログラム実装
  +----------------------------------------------------------------+*/
//-----------------------
// [クラス] 登録開始
//-----------------------
void CDrawer::ReadyForRegist( eDRAW_PROCESS process ){
	s_eDrawProcess = process;
	s_pDrawHead = NULL;
}

//-----------------------
// [クラス] 登録要素の描画
//-----------------------
void CDrawer::DrawObjs( void  ){
	CDrawer* pObj = s_pDrawHead;

	while( pObj != NULL ){
		pObj->onDraw();
		pObj = pObj->getDrawNext();
	}
}

//----------------------
// [クラス] 現在のプロセス判定
//----------------------
bool CDrawer::CheckCurDrawProcess( eDRAW_PROCESS process ){
	return( s_eDrawProcess == process );
}

//------------------------------------------------------------------------
// [クラス] 登録
//------------------------------------------------------------------------
// 要素は優先順が大きいものほど前へ登録される（※優先順が大きいものほど先(奥)に描画される）
// 同じ優先順位の場合既存要素の後ろへ登録される（※登録順が早いものほど先(奥)に描画される）
//------------------------------------------------------------------------
void CDrawer::RegistObj( CDrawer* pNew ){
	// プロセスが違う場合は登録しない
	if( ! pNew->checkDrawProcess( s_eDrawProcess ) ){
		return;
	}

	CDrawer* pObj = s_pDrawHead;

	// 先頭が差し替わる場合は先頭に(※既存の先頭よりも優先順が大きい)
	if( pObj == NULL || pObj->m_nCurDrawPriority < pNew->m_nCurDrawPriority ){
		pNew->setDrawNext( pObj );
		s_pDrawHead = pNew;
		return;
	}

	// 追加先を調べる
	CDrawer* pNext;
	while( (pNext=pObj->getDrawNext()) != NULL ){
		// 一つ先の項目より優先順位が大きくなったら抜ける
		if( pNew->m_nCurDrawPriority > pNext->m_nCurDrawPriority ){
			break;
		}

		pObj = pNext;
	}

	// 末端に来ていた場合は末尾に追加
	if( pNext == NULL ){
		pObj->setDrawNext( pNew );
		pNew->setDrawNext( NULL );
		return;
	}

	// ここまできたら対象要素の後ろに追加
	pObj->setDrawNext( pNew );
	pNew->setDrawNext( pNext );
}

//--------------------------------------------
// [メンバー] コンストラクタ
//--------------------------------------------
CDrawer::CDrawer( void ){
	clearDrawer();
}

//--------------------------------------------
// [メンバー] クリア
//--------------------------------------------
void CDrawer::clearDrawer( void ){
	m_eDrawProcess = eDRAW_PROCESS_MAIN;
	m_eDrawLayer = eDP_LAYER_INVALID;
	m_nDrawPriorityOfs = 0;
	m_nDrawPriorityAdjust = 0;
    m_nCurDrawPriority = 0;
    m_pDrawNext = NULL;
}

//--------------------------------------------
// [メンバー] 描画登録（※リストへ自身を登録）
//--------------------------------------------
void CDrawer::registForDraw( void ){
	// 有効なレイヤーであれば
	if( m_eDrawLayer != eDP_LAYER_INVALID ){
		// 都度描画優先順位を計算
		m_nCurDrawPriority  = DRAW_LAYER_STEP * m_eDrawLayer;
		m_nCurDrawPriority += DRAW_PRIORITY_OFS_STEP * m_nDrawPriorityOfs;
		m_nCurDrawPriority += m_nDrawPriorityAdjust;

		// 登録
		RegistObj( this );
	}
}

//---------------------------------------------
// [メンバー] 実際の描画（※登録後にシステムから呼ばれる）
//---------------------------------------------
void CDrawer::onDraw( void ){
	onDraw0();
}
