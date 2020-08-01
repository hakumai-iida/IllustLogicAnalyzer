/*+----------------------------------------------------------------+
  |	Title:		DataList.hpp [共通環境]
  |	Comment:	データリスト（※リスト操作を備えたデータ）
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "DataList.hpp"

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
/*+----------------------------------------------------------------+
  |	Program		プログラム実装
  +----------------------------------------------------------------+*/
//--------------------
// コンストラクタ
//--------------------
CDataList::CDataList( void ){
    m_bNodeDisconnectOnly = false;
}

//--------------------
// コンストラクタ
//--------------------
CDataList::CDataList( bool isDisconnectOnly ){
    m_bNodeDisconnectOnly = isDisconnectOnly;
}

//--------------------
// デストラクタ
//--------------------
CDataList::~CDataList( void ){
    if( m_bNodeDisconnectOnly ){
        m_oDataList.disconnectAll();
    }else{
        m_oDataList.deleteNodeAll();
    }
}

//----------------------
// 選択設定
//----------------------
int CDataList::setSelect( int select ){
	m_nSelect = select;
	if( m_nSelect < 0 ){
		m_nSelect = 0;
	}else{
		int num = m_oDataList.getSize();
		if( select >= num ){
			m_nSelect = num - 1;
		}
	}
    
    return( m_nSelect );
}

//----------------------
// 選択増減
//----------------------
int CDataList::addSelect( int add ){
	m_nSelect += add;
	if( m_nSelect >= m_oDataList.getSize() ){ m_nSelect = 0; }
	else if( m_nSelect < 0 ){ m_nSelect = m_oDataList.getSize() - 1; }
    
    return( m_nSelect );
}

//----------------------------------
// データの取得
//----------------------------------
CListNode* CDataList::getData0( int select ){
	return( m_oDataList.getAt( select ) );
}

//----------------------------------
// 選択データの取得
//----------------------------------
CListNode* CDataList::getSelectedData0( void ){
	return(  m_oDataList.getAt( m_nSelect ) );
}

//-------------------
// データ追加
//-------------------
CListNode* CDataList::addData0( CListNode* pNewData ){
	// 要素があれば
	if( m_oDataList.getSize() > 0 ){
		// 選択中の要素の後ろに追加
		m_nSelect += 1;
		m_oDataList.insertAt( pNewData, m_nSelect );
	}
	// 先頭に追加
	else{
		m_oDataList.add( pNewData );
		m_nSelect = 0;
	}

	// 追加データを返しておく
	return( pNewData );
}

//---------------
// データ移動：上
//---------------
CListNode* CDataList::moveUpData0( void ){
	CListNode *pData = getSelectedData0();
	m_nSelect = m_oDataList.movePrev( pData );
	return( pData );
}

//---------------
// データ移動：下
//---------------
CListNode* CDataList::moveDownData0( void ){
	CListNode* pData = getSelectedData0();
	m_nSelect = m_oDataList.moveNext( pData );
	return( pData );
}

//-------------
// データ削除
//-------------
CListNode* CDataList::deleteData0( void ){
    CListNode* pNode = NULL;
    
    // 選択対象の削除
    if( m_bNodeDisconnectOnly ){
        // リストから削除した後に呼び出し元がアクセスできるように保持しておく
        pNode = m_oDataList.getAt( m_nSelect );
        m_oDataList.disconnectAt( m_nSelect );
    }else{
        // メモリを解放する場合は返値は[NULL]のまま（※アクセスされても困るので）
        m_oDataList.deleteNodeAt( m_nSelect );
    }
    
    // 選択位置の調整
    if( m_nSelect >= m_oDataList.getSize() ){
        m_nSelect = m_oDataList.getSize() - 1;
    }
    
    return( pNode );
}
