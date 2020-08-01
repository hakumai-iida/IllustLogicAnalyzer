/*+----------------------------------------------------------------+
  |	Title:		mcell.hpp　[共通環境]
  |	Comment:	メモリ単位関連
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#ifndef __MCELL_HPP__
#define __MCELL_HPP__

/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Struct		構造体定義
  +----------------------------------------------------------------+*/
//------------------------------------------------------------------
// メモリ単位構造体（※各メモリはリンク付きリストで管理される）
//------------------------------------------------------------------
struct stMCELL{
	void*		pAddr;			// 確保アドレス
	void*		pAddrFixed;		// 呼び出し元に返すアドレス
	uint32		size;			// 確保サイズ（※アライン調整により指定サイズよりも大きくなる場合がある）
	uint32		size0;			// Alloc時に指定されたサイズ（※アライン調整分の領域が問題になる場合はこちらを使用）
	uint32		addrRel;		// フィールド内でのアドレス相対値（※デフラグが必要になったら使用予定）

	stMCELL*	pPrev;			// リスト：前要素
	stMCELL*	pNext;			// リスト：後要素

	bool		isActive;		// 有効フラグ（※セルとして活動中）
	bool		isAllocated;	// メモリ割り当て済みフラグ
};

//------------------------------------------------
// メモリフィールド管理構造体
//------------------------------------------------
struct stMCELL_FIELD{
	void*		pAddr;			// 割り当てられた領域（※この領域内でやりくりされる）
	uint32		size;			// 割り当てられたサイズ
	uint32		alignMem;		// アライメント
	uint32		alignCell;		// 確保する最小メモリ（※極端に小さな領域を確保しようとた際、最低、このサイズが確保される）

	uint32		numCell;		// 割り当てられたセル数（※[pCellPool]を配列アクセスする際の最大数）
	stMCELL*	pCellPool;		// 割り当てられたセルプール領域
	uint32		lastCellAt;		// 最後に確保したセルプールインデックス（※次回検索開始位置）

	stMCELL*	pCellHead;		// リンクリスト先頭（※この要素を先頭に有効なメモリセルがリンク付きリストで管理される）

	// デバッグ用
	const char*	pName;			// フィールド名
	uint32		numActiveCell;	// 活動中のセル数
	bool		isDumpLog;		// ログをダンプするか？
};

/*+----------------------------------------------------------------+
  |	Class		クラス定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
// 初期化
extern void mcell_field_init( stMCELL_FIELD* pF );

// 名前設定（※デバッグログにて利用される）
extern bool mcell_field_set_name( stMCELL_FIELD* pF, const char* pName );
extern bool mcell_dump_log( stMCELL_FIELD* pF, bool isDumpLog );

// 設定（※[set_memory]⇒[set_pool]の順によぶこと）
extern bool mcell_field_set_memory( stMCELL_FIELD* pF, void* pAddr, uint32 size, uint32 alignMem );		// メモリ領域設定
extern bool mcell_field_set_pool( stMCELL_FIELD* pF, void* pAddr, uint32 numCell, uint32 alignCell );	// プール設定

// 有効性の判定
extern bool mcell_field_is_valid( stMCELL_FIELD* pF );

// プールサイズの算出（※[mcell_field_set_pool]へ割り当てる領域は呼び出し元が自前で準備すること）
extern uint32 mcell_calc_pool_size( int numCell );

// 割り当てと解放
extern void* mcell_alloc( stMCELL_FIELD* pF, uint32 size0, stMCELL** ppCell=NULL );
extern void  mcell_free( stMCELL_FIELD* pF, void* pAddrFixed );
extern void  mcell_free_all( stMCELL_FIELD* pF );	// 全領域解放

// 領域がふくまれるか？
extern bool mcell_is_contained( stMCELL_FIELD* pF, void*pAddr );

// アラインの変更
extern void mcell_set_alignment( stMCELL_FIELD* pF, uint32 alignMem );

#endif	// __MCELL_HPP__
