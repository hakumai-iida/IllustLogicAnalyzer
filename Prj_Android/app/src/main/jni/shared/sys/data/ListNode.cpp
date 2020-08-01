/*+----------------------------------------------------------------+
  |	Title:		ListNode.cpp [共通環境]
  |	Comment:	リスト要素（※CListに登録したい場合に派生する）
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "ListNode.hpp"

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
//-----------------------
// コンストラクタ
//-----------------------
CListNode::CListNode( void ){
	m_nUid = UNIQUE_ID_INVALID;
	m_pPrev = NULL;
	m_pNext = NULL;
}

//----------------------------
// 接続：前に
//----------------------------
void CListNode::connectPrev( CListNode* pPrev ){
	// 無効は無視
	if( pPrev == NULL ){
		LOGE( "@ CListNode::connectPrev: NULL NODE\n" );
		return;
	}
	if( pPrev == this ){
		LOGE( "@ CListNode::connectPrev: CONNECT ITSELF\n" );
		return;
	}

	// 追加対象の前後を接続
	pPrev->m_pPrev = m_pPrev;
	pPrev->m_pNext = this;

	// 自分の前要素の後を差し替え
	if( m_pPrev != NULL ){
		m_pPrev->m_pNext = pPrev;
	}

	// 自分の前要素を差し替え
	m_pPrev = pPrev;
}

//----------------------------
// 接続：後ろに
//----------------------------
void CListNode::connectNext( CListNode* pNext ){
	// 無効は無視
	if( pNext == NULL ){
		LOGE( "@ CListNode::connectNext: NULL NODE\n" );
		return;
	}
	if( this == pNext ){
		LOGE( "@ CListNode::connectNext: CONNECT ITSELF\n" );
		return;
	}

	// 追加対象の前後を接続
	pNext->m_pPrev = this;
	pNext->m_pNext = m_pNext;

	// 自分の後要素の後を差し替え
	if( m_pNext != NULL ){
		m_pNext->m_pPrev = pNext;
	}

	// 自分の後要素を差し替え
	m_pNext = pNext;
}

//----------------------------
// 切断
//----------------------------
void CListNode::disconnect( void ){
	// 前要素の後を差し替え
	if( m_pPrev != NULL ){
		m_pPrev->m_pNext = m_pNext;
	}

	// 後要素の前を差し替え
	if( m_pNext != NULL ){
		m_pNext->m_pPrev = m_pPrev;
	}

	// 切断
	m_pPrev = NULL;
	m_pNext = NULL;
}
