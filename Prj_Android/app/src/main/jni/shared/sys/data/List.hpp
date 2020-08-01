/*+----------------------------------------------------------------+
  |	Title:		List.hpp [共通環境]
  |	Comment:	リスト（※CListNodeから派生したクラスを格納）
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#ifndef __LIST_HPP__
#define __LIST_HPP__

/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "ListNode.hpp"

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
// リスト
//----------------------------------------
class CList{
protected:
	// 先頭／末尾
	CListNode*	m_pHead;
	CListNode*	m_pTail;

public:
	// コンストラクタ
	CList( void );

	//--------------------------------------------------------
	// デストラクタ：ここでは[deleteNode]等はよばない
    //--------------------------------------------------------
	// 利用する際は、リストに登録されているノードのメモリが
	// どう管理されているかを把握し、必要に応じて手動で破棄すること
    // メモリの解放と同様、登録されたノード同士の接続も適宜切断すること
	//--------------------------------------------------------
	virtual ~CList( void ){}

	// 取得：先頭／末尾
	inline CListNode* getHead( void ){ return( m_pHead ); }
	inline CListNode* getTail( void ){ return( m_pTail ); }

	// 要素数の取得
	int getSize( void );
    inline int getNum( void ){ return( getSize() ); }

	//--------------------------------------------------------
	// ノードの追加：ID無視（※ローカルデータ向け）
	// データの構築等でノードの移動等が行われる前提
	//--------------------------------------------------------
	// 同じIDがあっても追加される
	int add( CListNode* pNode );

	// 指定位置に挿入（※返値は実際の挿入位置）
	int insertAt( CListNode* pNode, int at );

	// 移動：前に
	int movePrev( CListNode* pNode );
	int movePrevAt( int at );

	// 移動：後に
	int moveNext( CListNode* pNode );
	int moveNextAt( int at );

	// 指定要素の取出し（※配列的アクセス：遅い）
	CListNode* getAt( int at );

	// 指定要素が配列的に何番目であるか？
	int getIndex( CListNode* pNode );

	//--------------------------------------------------------
	// ノードの追加：uid管理（※DB等のデータ向け）
	// 一度構築されたリストではノードの移動等は行われない前提
	//--------------------------------------------------------
	// 同じuidのノードがあった場合は上書きされる
	void addByUid( CListNode* pNode );

	// uidによる取出し（※ミス時は[NULL]が返る）
	CListNode* getByUid( int uid, bool isSorted=true );

    //--------------------------------------------------------
	// ノードの切断／削除
	//--------------------------------------------------------
	// ノードの切断（※リストから取り除くのみ）
	void disconnect( CListNode* pNode, bool isDelete=false );
	void disconnectAt( int at, bool isDelete=false );
	void disconnectByUid( int uid, bool isDelete=false );
	void disconnectAll( bool isDelete=false );

	// ノードの削除（※ノードの領域削除もおこなう）
	inline void deleteNode( CListNode* pNode ){ disconnect( pNode, true ); }
	inline void deleteNodeAt( int at ){ disconnectAt( at, true ); }
	inline void deleteNodeByUid( int uid ){ disconnectByUid( uid, true ); }
	inline void deleteNodeAll( void ){ disconnectAll( true ); }

protected:
	void moveNodePrev( CListNode* pNode );		// 指定ノードを一つ前に
	void moveNodeNext( CListNode* pNode );		// 指定ノードを一つ後に
	void disconnectNode( CListNode* pNode );	// 指定ノードを切断
};

/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/

#endif /* __LIST_HPP__ */
