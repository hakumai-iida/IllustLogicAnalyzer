/*+----------------------------------------------------------------+
  |	Title:		mcell.cpp　[共通環境]
  |	Comment:	メモリ単位関連
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "mcell.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Struct		構造体型宣言
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
// セルの確保／解放
static stMCELL* _alloc_cell( stMCELL_FIELD* pF );
static void		_free_cell( stMCELL_FIELD* pF, stMCELL* pCell );

// セル操作
static void _set_cell( stMCELL* pCell, void* pAddr, uint32 rel, uint32 size );
static void _clear_cell( stMCELL* pCell );
static void _insert_cell_after( stMCELL* pCellNew, stMCELL* pCell );

/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Program		プログラム実装
  +----------------------------------------------------------------+*/
//-------------------------------------
// 初期化
//-------------------------------------
void mcell_field_init( stMCELL_FIELD* pF ){
	// 無効チェック
	if( pF == NULL ){ return; }

	// ０クリア
	memset( pF, 0, sizeof(stMCELL_FIELD ) );

	// 未初期化名
	pF->pName = "undef";
}

//-------------------------------------
// 名前の設定
//-------------------------------------
bool mcell_field_set_name( stMCELL_FIELD* pF, const char* pName ){
	// 無効チェック
	if( pF == NULL ){ return( false ); }
	if( pName == NULL ){ return( false ); }

	pF->pName = pName;

	return( true );
}

//-------------------------------------
// ダンプログの指定
//-------------------------------------
bool mcell_dump_log( stMCELL_FIELD* pF, bool isDumpLog ){
	// 無効チェック
	if( pF == NULL ){ return( false ); }

	pF->isDumpLog = isDumpLog;

	return( true );
}

//-------------------------------------
// メモリ領域の設定
//-------------------------------------
bool mcell_field_set_memory( stMCELL_FIELD* pF, void* pAddr, uint32 size, uint32 alignMem ){
	// 無効チェック
	if( pF == NULL ){ return( false ); }
	if( pAddr == NULL ){ return( false ); }

	// 設定
	pF->pAddr = pAddr;
	pF->size  = size;
	pF->alignMem = alignMem;

	return( true );
}

//-------------------------------------------------
// プールの設定（※領域設定後に呼ぶこと）
//-------------------------------------------------
bool mcell_field_set_pool( stMCELL_FIELD* pF, void* pAddr, uint32 numCell, uint32 alignCell ){
	// 無効チェック
	if( pF == NULL ){ return( false ); }
	if( pAddr == NULL ){ return( false ); }

	// 設定
	pF->pCellPool = (stMCELL*)pAddr;
	pF->numCell   = numCell;
	pF->alignCell = alignCell;

	// 全領域を解放
	mcell_free_all( pF );

	return( true );
}

//-------------------------------------
// 有効性の判定
//-------------------------------------
bool mcell_field_is_valid( stMCELL_FIELD* pF ){
	if( pF == NULL ){ return( false ); }
	if( pF->pAddr == NULL ){ return( false ); }
	if( pF->pCellPool == NULL ){ return( false ); }

	return( true );
}

//-------------------------------------
// プールサイズの算出
//-------------------------------------
uint32 mcell_calc_pool_size( int numCell ){
	return( numCell*sizeof(stMCELL) );
}

//--------------------------------------------------------------------------
// 割り当て（※正常遷移を前提にフィールドの有効性の判定は無し）
//--------------------------------------------------------------------------
void* mcell_alloc( stMCELL_FIELD* pF, uint32 size0, stMCELL** ppCell ){
	// 確保サイズ０は無視しておく
	if( size0 == 0 ){ return( NULL ); }

	// アライメント調整関連
	uint32 ofsAdj = 0;
	uint32 sizeTotal = 0;
	uint32 sizeAdj = ALIGN_ADJ( size0, pF->alignCell );

	// 空きを探す
	stMCELL* pCell = pF->pCellHead;
	while( pCell != NULL ){
		// 指定サイズ以上の未割当領域があれば抜ける
		if( !pCell->isAllocated ){
			// アライメント調整
			if( (((ADDR)pCell->pAddr)%pF->alignMem ) == 0 ){ ofsAdj = 0; }
			else{ ofsAdj = pF->alignMem - (((ADDR)pCell->pAddr)%pF->alignMem); }

			// 実際に確保すべきサイズは[調整分オフセット＋調整済サイズ]となる
			sizeTotal = ofsAdj + sizeAdj;
			if( pCell->size >= sizeTotal ){ break; }
		}
		pCell = pCell->pNext;
	}

	// 候補がみつかっていたら
	if( pCell != NULL ){
		// 一定以上の空きがでるのであれば分割（※空くのが小さな領域であればまとめてしまう）
		if( (pCell->size-sizeTotal) >= pF->alignCell ){
			// セルの確保
			stMCELL* pCellTemp = _alloc_cell( pF );
			if( pCellTemp == NULL ){
				LOGE( "@ ** [MCELL] cell alloc error : %s **\n", pF->pName );
				return( NULL );
			}

			// セルの設定（※空き領域から確保サイズを引いた残りを新空き領域として追加）
			_set_cell( pCellTemp, pF->pAddr, pCell->addrRel+sizeTotal, pCell->size-sizeTotal );
			_insert_cell_after( pCellTemp, pCell );

			// 確保した領域のサイズの設定
			pCell->size = sizeTotal;
		}

		// 確保済みフラグを立てる
		pCell->isAllocated = true;
		pCell->size0 = size0;		// 指定されたサイズの履歴

		// 呼び出し元に返す実効アドレス（※アライメントに則したアドレス）
		pCell->pAddrFixed = (void* )(((ADDR)pCell->pAddr) + ofsAdj);

		// セル取得指定があれば設定
		if( ppCell != NULL ){
			*ppCell = pCell;
		}

		if( pF->isDumpLog ){
			LOGD( "@  == [MCELL] Alloc( pAddr=%p, size=%d(%d) ) : %s ==\n", pCell->pAddr, pCell->size, pCell->size0, pF->pName );
		}

		return( pCell->pAddrFixed );
	}

	// ここまできたら致命的
	LOGE( "@ ** [MCELL] alloc failed ( size=%d ) : %s **\n", size0, pF->pName );

	return( NULL );
}

//--------------------------------------------------------------------------
// 解放（※正常遷移を前提にフィールドの有効性の判定は無し）
//--------------------------------------------------------------------------
void mcell_free( stMCELL_FIELD* pF, void* pAddrFixed ){
	// [NULL]解放だけは無視しておく
	if( pAddrFixed == NULL ){ return; }

	// 対象を探す
	stMCELL* pCell = pF->pCellHead;
	while( pCell != NULL ){
		if( pCell->pAddrFixed == pAddrFixed ){ break; }
		pCell = pCell->pNext;
	};

	// 見つかっていたらセルの解放
	if( pCell != NULL ){
		if( pF->isDumpLog ){
			LOGD( "@  == [MCELL] Free( pAddr=%p, pAddrFixed=%p, size=%d(%d) ) : %s ==\n", pCell->pAddr, pCell->pAddrFixed,  pCell->size, pCell->size0, pF->pName );
		}
		_free_cell( pF, pCell );
	}else{
		// 解放エラーは致命的としておく（※利用側の重複解放や不正ポインタ使用等）
		LOGE( "@ ** [MCELL] free missed ( pAddrFixed=%p ) : %s **\n", pAddrFixed, pF->pName );
	}
}

//-------------------------------------
// 全領域解放
//-------------------------------------
void mcell_free_all( stMCELL_FIELD* pF ){
	// 無効は無視
	if( !mcell_field_is_valid( pF ) ){ return; }

	// ０クリア
	memset( pF->pCellPool, 0, mcell_calc_pool_size(pF->numCell) );
	pF->numActiveCell = 0;	// 活動数は０
	if( pF->isDumpLog ){
		LOGD( "@ [X] mcell_free_all() %d (use=%d) : %s\n", pF->numCell, pF->numActiveCell, pF->pName );
	}

	// ヘッダセルの設定
	pF->pCellHead = _alloc_cell( pF );
	if( pF->pCellHead == NULL ){
		LOGE( "@ ** [MCELL] head error : %s **\n", pF->pName );
		return;
	}

	// ヘッダセルの設定（※[全領域が空である]ことを表すセル）
	_set_cell( pF->pCellHead, pF->pAddr, 0, pF->size );
}

//------------------
// 領域がふくまれるか？
//------------------
bool mcell_is_contained( stMCELL_FIELD* pF, void* pAddr ){
	// 無効は無視
	if( pAddr == NULL ){ return( false ); }

	// フィールド領域内に収まっているか？
	ADDR addrStart = (ADDR) pF->pAddr;
	ADDR addrEnd = addrStart + pF->size;

	ADDR target = (ADDR) pAddr;

	return( target>=addrStart && target<addrEnd );
}

//------------------
// アラインの変更
//------------------
void mcell_set_alignment( stMCELL_FIELD* pF, uint32 alignMem ){
	pF->alignMem = alignMem;
}

//------------------
// セルの確保
//------------------
static stMCELL* _alloc_cell( stMCELL_FIELD* pF ){
	uint32 i;

	// 前回確保した次の要素から検索を始める
	for( i=pF->lastCellAt+1; i<pF->numCell; i++ ){
		if( !pF->pCellPool[i].isActive ){
			// 空きがあればリセット＆活動フラグを設定
			_clear_cell( &pF->pCellPool[i] );
			pF->pCellPool[i].isActive = true;
			pF->numActiveCell++;	// 活動数をインクリメント

			// 次回検索位置を保持しておく
			pF->lastCellAt = i;

			if( pF->isDumpLog ){
				LOGD( "@ [*] _alloc_cell(D) i=%d use=%d/%d : %s\n", i, pF->numActiveCell, pF->numCell, pF->pName );
			}
			return( &pF->pCellPool[i] );
		}
	}

	// 先頭から前回確保した要素まで検索する
	for( i=0; i<=pF->lastCellAt; i++ ){
		if( !pF->pCellPool[i].isActive ){
			// 空きがあればリセット＆活動フラグを設定
			_clear_cell( &pF->pCellPool[i] );
			pF->pCellPool[i].isActive = true;
			pF->numActiveCell++;	// 活動数をインクリメント

			// 次回検索位置を保持しておく
			pF->lastCellAt = i;

			if( pF->isDumpLog ){
				LOGD( "@ [*] _alloc_cell(U) i=%d use=%d/%d : %s\n", i, pF->numActiveCell, pF->numCell, pF->pName );
			}
			return( &pF->pCellPool[i] );
		}
	}

	// ここまで来たら空きが無い
    LOGE( "@ ** [MCELL]: _alloc_cell error at=%d, use=%d/%d\n", pF->lastCellAt, pF->numActiveCell, pF->numCell );

	return( NULL );
}

//------------------------------------
// セルの解放（※前後のセルの調整込）
//------------------------------------
static void _free_cell( stMCELL_FIELD* pF, stMCELL* pCell ){
	stMCELL* pCellTemp;

	// フラグを寝かす
	pCell->isAllocated = false;

	// 後の要素が空きであれば取り込む
	if( pCell->pNext != NULL ){
		if( !pCell->pNext->isAllocated ){
			pCellTemp = pCell->pNext;

			// 取り込む（※サイズを足しこむのみ）
			pCell->size += pCell->pNext->size;
			pCell->pNext = pCell->pNext->pNext;
			if( pCell->pNext != NULL ){
				pCell->pNext->pPrev = pCell;
			}

			// 取り込まれた要素はクリア
			_clear_cell( pCellTemp );

			pF->numActiveCell--;// 活動数をデクリメント
			if( pF->isDumpLog ){
				LOGD( "@ [-] _free_cell(N) use=%d/%d : %s\n", pF->numActiveCell, pF->numCell, pF->pName );
			}
		}
	}

	// 前の要素が空きであれば取り込まれる
	if( pCell->pPrev != NULL ){
		if( !pCell->pPrev->isAllocated ){
			pCellTemp = pCell->pPrev;

			// 取り込まれる（※サイズを足しこむのみ）
			pCellTemp->size += pCell->size;
			pCellTemp->pNext = pCell->pNext;
			if( pCellTemp->pNext != NULL ){
				pCellTemp->pNext->pPrev = pCellTemp;
			}

			// 取り込まれた要素はクリア
			_clear_cell( pCell );

			pF->numActiveCell--;// 活動数をデクリメント
			if( pF->isDumpLog ){
				LOGD( "@ [-] _free_cell(P) use=%d/%d : %s\n", pF->numActiveCell, pF->numCell, pF->pName );
			}
		}
	}
}

//-------------------------
// セル設定
//-------------------------
static void _set_cell( stMCELL* pCell, void* pAddr, uint32 rel, uint32 size ){
	pCell->addrRel = rel;
	pCell->size    = size;
	pCell->pAddr   = (void*)((ADDR)pAddr+pCell->addrRel);
}

//------------------
// セルのクリア
//------------------
static void _clear_cell( stMCELL* pCell ){
	memset( pCell, 0, sizeof(stMCELL) );
}

//-------------------------
// 指定セルの後ろに追加
//-------------------------
static void _insert_cell_after( stMCELL* pCellNew, stMCELL* pCell ){
	pCellNew->pPrev = pCell;
	pCellNew->pNext = pCell->pNext;

	pCell->pNext = pCellNew;
	if( pCellNew->pNext != NULL ){
		pCellNew->pNext->pPrev = pCellNew;
	}
}
