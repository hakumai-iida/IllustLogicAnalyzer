/*+----------------------------------------------------------------+
  |	Title:		List.cpp [共通環境]
  |	Comment:	リスト（※CListNodeから派生したクラスを格納）
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "List.hpp"

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
//----------------------
// コンストラクタ
//----------------------
CList::CList( void ){
	m_pHead = NULL;
	m_pTail = NULL;
}

//----------------------
// 要素数取得
//----------------------
int CList::getSize( void ){
	int num = 0;

	CListNode* pCur = m_pHead;
	while( pCur != NULL ){
		num++;
		pCur = pCur->getNext0();
	}

	return( num );
}

//----------------------
// [位置管理] 末尾に追加
//----------------------
int CList::add( CListNode* pNode ){
	// 無効は無視
	if( pNode == NULL ){
		LOGE( "@ CList::add: NULL NODE\n" );
		return( -1 );
	}

	// 要素がなければリスト先頭として設定
	if( m_pHead == NULL ){
		m_pHead = m_pTail = pNode;
		return( 0 );
	}

	// 末尾に追加
	m_pTail->connectNext( pNode );
	m_pTail = pNode;

	// 追加位置を返す
	int at = getSize() - 1;
	return( at );
}

//----------------------
// [位置管理] 挿入
//----------------------
int CList::insertAt( CListNode* pNode, int at ){
	// 無効は無視
	if( pNode == NULL ){
		LOGE( "@ CList::insertAt: NULL NODE\n" );
		return( -1 );
	}

	if( at < 0 ){
		LOGE( "@ CList::insertAt: INVALID INDEX:%d\n", at );
		return( -1 );
	}

	// 先頭が指定された
	if( at == 0 ){
		if( m_pHead != NULL ){
			m_pHead->connectPrev( pNode );
		}else{
			m_pTail = pNode;
		}
		m_pHead = pNode;

		return( 0 );
	}

	int num = getSize();

	// 無効は無視
	if( at > num ){
		return( -1 );
	}

	// 末尾が指定された
	if( at == num ){
		if( m_pTail != NULL ){
			m_pTail->connectNext( pNode );
		}else{
			m_pHead = pNode;
		}
		m_pTail = pNode;

		return( num );
	}

	// 中間位置が指定された
	CListNode* pTarget = getAt( at );
	pTarget->connectPrev( pNode );

	return( at );
}

//----------------------
// [位置管理] 前へ移動
//----------------------
int CList::movePrev( CListNode* pNode ){
	// 無効は無視
	if( pNode == NULL ){
		LOGE( "@ CList::movePrev: NULL NODE\n" );
		return( -1 );
	}

	int at = getIndex( pNode );
	if( at < 0 ){
		LOGE( "@ CList::movePrev: INVALID NODE\n" );
		return( -1 );
	}

	// 移動先があれば移動
	if( at >= 1 ){
		moveNodePrev( pNode );
		return( at - 1 );
	}

	return( at );
}

//--------------------
// [位置管理] 前へ移動
//--------------------
int CList::movePrevAt( int at ){
	CListNode* pNode = getAt( at );

	if( pNode != NULL ){
		// 移動先があれば移動
		if( at >= 1 ){
			moveNodePrev( pNode );
			return( at - 1 );
		}
	}else{
		LOGE( "@ CList::movePrevAt: INVALID INDEX:%d\n", at );
		at = -1;
	}

	return( at );
}

//----------------------
// [位置管理] 後ろへ移動
//----------------------
int CList::moveNext( CListNode* pNode ){
	// 無効は無視
	if( pNode == NULL ){
		LOGE( "@ CList::moveNext: NULL NODE\n" );
		return( -1 );
	}

	int at = getIndex( pNode );
	if( at < 0 ){
		LOGE( "@ CList::moveNext: INVALID NODE\n" );
		return( -1 );
	}

	// 移動先があれば移動
	int num = getSize();
	if( at < (num-1) ){
		moveNodeNext( pNode );
		return( at + 1 );
	}

	return( at );
}

//----------------------
// [位置管理] 後ろへ移動
//----------------------
int CList::moveNextAt( int at ){
	CListNode* pNode = getAt( at );

	if( pNode != NULL ){
		// 移動先があれば移動
		int num = getSize();
		if( at < (num-1) ){
			moveNodeNext( pNode );
			return( at + 1 );
		}
	}else{
		LOGE( "@ CList::moveNextAt: INVALID INDEX:%d\n", at );
		at = -1;
	}

	return( at );
}

//----------------------
// [位置管理] 取得
//----------------------
CListNode* CList::getAt( int at ){
	// 無効は無視
	if( at < 0 ){ return( NULL ); }

	CListNode* pCur = m_pHead;
	while( pCur != NULL ){
		if( at <= 0 ){
			return( pCur );
		}

		at--;
		pCur = pCur->getNext0();
	}

	return( NULL );
}

//-------------------------------
// [位置管理] インデックスの取得
//-------------------------------
int CList::getIndex( CListNode* pNode ){
	int at = 0;
	CListNode* pCur = m_pHead;
	while( pCur != NULL ){
		if( pCur == pNode ){
			return( at );
		}

		at++;
		pCur = pCur->getNext0();
	}

	return( -1 );
}

//-----------------------------------------------------------------------
// [uid管理] ユニークIDによる追加
//-----------------------------------------------------------------------
// 要素はuid(m_nUid)の大きい順で格納される（例：[5,4,3,2,1,0]）ため、
// 要素を[0,1,2,3,4,5]の順で登録しても、[3,5,1,2,0,4]の順で登録しても
// 結果としては[(m_pHead=>)5,4,3,2,1,0(<=m_pTail)]の並びでリスト構築される
// uidの指定が自由になる状況では、[0,1,2,3,4,5...]の順で追加することで
// 追加位置の検索を最適化できる（逆に[5,4,3,2,1,0]の順は最も非効率）
//-----------------------------------------------------------------------
// 格納順とは逆の並びでデータを取り出す際は、[m_pTail]から
// [CNodeList::getPrev]を辿っていくことで、[0,1,2,3,4,5]の並びで取り出せる
// 逆に[m_pHead]から[CNodeList::getNext]を辿ることで、
// リストの格納順のままの[5,4,3,2,1,0]の並びで取り出せる
//-----------------------------------------------------------------------
// 大量のデータ登録、並びに意味のある取出しの際は上記の点に注意すること
//-----------------------------------------------------------------------
void CList::addByUid( CListNode* pNode ){
	// 無効は無視
	if( pNode == NULL ){
		LOGE( "@ CList::addById: NULL NODE!\n" );
		return;
	}

	// 無効なuidは無視
	if( pNode->getUid() < 0 ){
		LOGE( "@ CList::addById: INVALID uid=%d\n", pNode->getUid() );
		return;
	}

	// 要素がなければリスト先頭として設定
	if( m_pHead == NULL ){
		m_pHead = m_pTail = pNode;
		return;
	}

	// 追加対象の検索
	CListNode* pTemp = m_pHead;
	while( pTemp != NULL ){
		if( pNode->getUid() >= pTemp->getUid() ){ break; }
		pTemp = pTemp->getNext0();
	}

	// uidが重複している場合は削除対象
	CListNode* pDupNode = NULL;
	if( pTemp != NULL ){
		if( pNode->getUid() == pTemp->getUid() ){
			pDupNode = pTemp;
		}
	}

	// 対象が有効なら前に追加
	if( pTemp != NULL ){
		pTemp->connectPrev( pNode );

		// 対象が先頭であればヘッドの差し替え
		if( m_pHead == pTemp ){
			m_pHead = pNode;
		}
	}else{
		// ここにきたら末尾に追加
		m_pTail->connectNext( pNode );
		m_pTail = pNode;
	}

	// 削除対象が有効であれば削除（※結果として上書きとなる）
	if( pDupNode != NULL ){
		deleteNode( pDupNode );
	}
}

//---------------------------------------------------------
// [uid管理] ユニークIDによる取得（※ミス時は[NULL]が返る）
//---------------------------------------------------------
CListNode* CList::getByUid( int uid, bool isSorted ){
	CListNode* pNode = m_pHead;
	while( pNode != NULL ){
		if( pNode->getUid() == uid ){
			return( pNode );
		}

		// 対象IDの方が小さくなったら抜ける（※ノードはIDの大きい順に並んでいる）
		if( isSorted && pNode->getUid() < uid ){
			break;
		}

		pNode = pNode->getNext0();
	}

	return( NULL );
}

//----------------------
// 切断／削除：直接
//----------------------
void CList::disconnect( CListNode* pNode, bool isDelete ){
	int at = getIndex( pNode );

	// 有効なら切断
	if( at >= 0 ){
		disconnectNode( pNode );

		// 指定があれば削除
		if( isDelete ){
			SAFE_DELETE( pNode );
		}
	}
}

//----------------------
// 切断／削除：位置指定
//----------------------
void CList::disconnectAt( int at, bool isDelete ){
	CListNode* pNode = getAt( at );

	// 有効なら切断
	if( pNode != NULL ){
		disconnectNode( pNode );

		// 指定があれば削除
		if( isDelete ){
			SAFE_DELETE( pNode );
		}
	}
}

//----------------------
// 削除／切断：uid指定
//----------------------
void CList::disconnectByUid( int uid, bool isDelete ){
	CListNode* pNode = getByUid( uid );

	// 有効なら切断
	if( pNode != NULL ){
		disconnectNode( pNode );

		// 指定があれば削除
		if( isDelete ){
			SAFE_DELETE( pNode );
		}
	}
}

//----------------------
// 全切断／全削除
//----------------------
void CList::disconnectAll( bool isDelete ){
	CListNode* pNode = m_pHead;

    if( isDelete ){
        while( pNode != NULL ){
            CListNode* pNext = pNode->getNext0();
            SAFE_DELETE( pNode );   // 削除
            pNode = pNext;
        }
    }else{
        while( pNode != NULL ){
            CListNode* pNext = pNode->getNext0();
            pNode->disconnect();    // 切断
            pNode = pNext;
        }
    }

	m_pHead = NULL;
	m_pTail = NULL;
}

//---------------------------
// [位置管理] 前へ移動（実体）
//---------------------------
void CList::moveNodePrev( CListNode* pNode ){
	// 一つ前のノードの取得
	CListNode* pPrev = pNode->getPrev0();

	// 一旦切断
	disconnectNode( pNode );

	// 一つ前の前に接続
	pPrev->connectPrev( pNode );

	// 先頭であれば差し替え
	if( pPrev == m_pHead ){
		m_pHead = pNode;
	}
}

//----------------------------
// [位置管理] 後ろへ移動(実体)
//----------------------------
void CList::moveNodeNext( CListNode* pNode ){
	// 一つ後ろのノードの取得
	CListNode* pNext = pNode->getNext0();

	// 一旦切断
	disconnectNode( pNode );

	// 一つ後ろの後ろに接続
	pNext->connectNext( pNode );

	// 末尾であれば差し替え
	if( pNext == m_pTail ){
		m_pTail = pNode;
	}
}

//----------------------
// 切断（実体）
//----------------------
void CList::disconnectNode( CListNode* pNode ){
	if( pNode == m_pHead ){
		m_pHead = pNode->getNext0();
	}

	if( pNode == m_pTail ){
		m_pTail = pNode->getPrev0();
	}

	pNode->disconnect();
}
