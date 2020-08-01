/*+----------------------------------------------------------------+
  |	Title:		MemMgr.cpp　[共通環境]
  |	Comment:	メモリ管理
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "MemMgr.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
// 調整値
#define MEM_ALLOC_CELL_ALIGN	MEM_ALLOC_ALIGNMENT	// アライメント（※アドレスはこの値の倍数となる）
#define MEM_ALLOC_CELL_MARGIN	32                  // 確保マージン（※これ以下のメモリ分割は行わない）

// フィールドスタックサイズ
#define FIELD_STACK_SIZE		64

/*+----------------------------------------------------------------+
  |	Struct		構造体型宣言
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
void*	CMemMgr::s_pAddr;		// アドレス
uint32	CMemMgr::s_nSize;		// サイズ
uint32	CMemMgr::s_nUsed;		// 使用済サイズ

// 確保先（※デフォルトはフリー領域）
int32	CMemMgr::s_nCurField = eMEM_FIELD_D_FREE;

bool	CMemMgr::s_bAllocEnable;

// 管理データ
stMEM_FIELD CMemMgr::s_stArrField[eMEM_FIELD_MAX];					// フィールド
stMEM_FIELD_SHARE CMemMgr::s_stArrFieldShare[eMEM_FIELD_SHARE_MAX];	// 共有フィールド管理

uint32	CMemMgr::s_nTempStrAt;

uint32	CMemMgr::s_nFieldStackUse;
int32*	CMemMgr::s_nArrFieldStack;

/*+----------------------------------------------------------------+
  |	Program		プログラム実装
  +----------------------------------------------------------------+*/
//------------------------------------------------------------------
// 初期化
//------------------------------------------------------------------
bool CMemMgr::Init(){
#ifdef MGR_INIT_LOG
	LOGD( "@ CMemMgr::Init()\n" );
#endif

    // アプリに依存した領域の確保
    if( ! AllocForApp() ){
        return( false );
    }
    
	// フィールドスタック初期化
	s_nArrFieldStack = (int32*)util_sys_mem_alloc( sizeof(int32)*FIELD_STACK_SIZE );
	s_nFieldStackUse = 0;
	s_nCurField = eMEM_FIELD_D_FREE;

	return( true );
}

//-----------------------------------------------------------------------------------------------
// 終了
//-----------------------------------------------------------------------------------------------
// [Alloc]により割り当てられた領域に対する[Free]が、[Exit]後にコールされる場合があるので、
// 確保と解放の整合性を確認するためにも、領域割り当て情報である[mcell]関連は解放しないでおく
//-----------------------------------------------------------------------------------------------
void CMemMgr::Exit( void ){
#ifdef MGR_INIT_LOG
	LOGD( "@ CMemMgr::Exit()\n" );
#endif

	// フィールドスタックの解放
    util_sys_mem_free( s_nArrFieldStack, "CMemMgr:s_nArrFieldStack" );

	// プールバッファの解放
	for( int i=0; i<eMEM_FIELD_MAX; i++ ){
		stMEM_FIELD* pField = get_field( (eMEM_FIELD)i );
		if( pField->pCellPool != NULL ){
			util_sys_mem_free( pField->pCellPool, "CMemMgr:pField->pCellPool" );
		}
	}

	// マネージャに割り当てられた領域を解放
	if( s_pAddr != NULL ){
		util_sys_mem_free( s_pAddr, "CMemMgr:s_pAddr[MAIN_MEMORY_FOR_MEM_MGR]" );
		s_pAddr = NULL;
	}
}

//-------------------
// 確保先設定
//-------------------
void CMemMgr::PushTargetField( eMEM_FIELD fieldId ){
	if( s_nFieldStackUse >= FIELD_STACK_SIZE ){
		LOGE( "@ CMemMgr::PushTargetField: STACK OVER FLOW: fieldId=%d\n", fieldId );
		return;
	}

	s_nArrFieldStack[s_nFieldStackUse++] = fieldId;
	s_nCurField = fieldId;
}

//-------------------
// 確保先戻す
//-------------------
void CMemMgr::PopTargetField( void ){
	if( s_nFieldStackUse > 0 ){
		s_nFieldStackUse--;
	}

	if( s_nFieldStackUse > 0 ){
		s_nCurField = s_nArrFieldStack[s_nFieldStackUse-1];
	}else{
		s_nCurField = eMEM_FIELD_D_FREE;
	}
}

//------------------------
// アドレス取得
//------------------------
void* CMemMgr::GetAddr( eMEM_FIELD fieldId ){
	// 有効な静的フィールドであればアドレスを返す
	stMEM_FIELD* pField = get_field( fieldId );
	if( pField != NULL && pField->isStatic ){ return( pField->pAddr ); }

	return( NULL );
}

//------------------------
// サイズ取得
//------------------------
uint32 CMemMgr::GetSize( eMEM_FIELD fieldId ){
	// 有効な静的フィールドであればサイズを返す
	stMEM_FIELD* pField = get_field( fieldId );
	if( pField != NULL && pField->isStatic ){ return( pField->size ); }

	return( 0 );
}

//---------------------------------
// 共有メモリからアドレス取得
//---------------------------------
void* CMemMgr::GetAddrFromShare( eMEM_FIELD_SHARE fieldShareId ){
	// 共有領域の取得
	BYTE* pBuf = (BYTE* )GetAddr( eMEM_FIELD_S_SHARE );
	if( pBuf == NULL ){ return( NULL ); }

	// 有効な共有フィールドであれば共有領域内の指定アドレスを返す
	stMEM_FIELD_SHARE* pFieldS = get_field_share( fieldShareId );
	if( pFieldS != NULL ){ return( &pBuf[pFieldS->ofs] ); }

	return( NULL );
}

//---------------------------------
// 共有メモリからサイズ取得
//---------------------------------
uint32 CMemMgr::GetSizeFromShare( eMEM_FIELD_SHARE fieldShareId ){
	// 有効な共有フィールドであればサイズを返す
	stMEM_FIELD_SHARE* pFieldS = get_field_share( fieldShareId );
	if( pFieldS != NULL ){ return( pFieldS->size ); }

	return( 0 );
}

//---------------------------------
// 一時文字列領域からバッファ取得
//---------------------------------
char* CMemMgr::GetBufFromTempStr( int32 target ){
    // 固定バッファが指定されていなければ循環
    if( target < 0 || target >= MEM_TEMP_STR_BUF_FIXED ){
        s_nTempStrAt = (s_nTempStrAt+1) % (MEM_TEMP_STR_BUF_NUM-MEM_TEMP_STR_BUF_FIXED);
        target = s_nTempStrAt + MEM_TEMP_STR_BUF_FIXED;
    }

	char* buf = (char*) GetAddrFromShare(eMEM_FIELD_SHARE_TEMP_STR);
	buf = &buf[target*MEM_TEMP_STR_BUF_SIZE];
	buf[0] = '\0';
	return( buf );
}

//---------------------------------
// 一時文字列領域からバッファサイズ取得
//---------------------------------
uint32 CMemMgr::GetSizeFromTempStr( void ){
	return( MEM_TEMP_STR_BUF_SIZE );
}

//---------------------------------
// 確保
//---------------------------------
void* CMemMgr::Alloc( uint32 size ){
	return( mcell_alloc( &s_stArrField[s_nCurField].field, size ) );
}

//---------------------------------
// 解放
//---------------------------------
bool CMemMgr::Free( void* pAddr ){
	// 管理下にあれば格納先を探して解放
	for( int i=eMEM_FIELD_D_START; i<=eMEM_FIELD_D_END; i++ ){
		if( mcell_is_contained( &s_stArrField[i].field, pAddr ) ){
			mcell_free( &s_stArrField[i].field, pAddr );
			return( true );
		}
	}

	// ここに来たらダメ
	LOGE( "@ CMemMgr::Free: UNKNOWN pAddr[%p]\n", pAddr );
	return( false );
}

//---------------------------------
// アライン設定
//---------------------------------
void CMemMgr::SetFieldAlignment( eMEM_FIELD fieldId, uint32 alignment ){
	stMEM_FIELD* pField = get_field( fieldId );
	if( pField != NULL ){ mcell_set_alignment( &pField->field, alignment ); }
}

//---------------------------
// [DEBUG] フィールドリポート
//---------------------------
void CMemMgr::ReportField( eMEM_FIELD fieldId, bool isDetail ){
	LOGD( "@ CMemMgr::ReportField(%d)\n", fieldId );
	stMEM_FIELD* pField = get_field( fieldId );

	// 無効
	if( pField == NULL ){
		LOGD( "@ * NULL *  \n" );
		return;
	}

	// ワーク
	uint32 sizeFree = 0;
	uint32 sizeUsed = 0;

	// プロパティの出力
	LOGD( "@ name     = %s\n", pField->field.pName );
	LOGD( "@ pAddr    = %p\n", pField->pAddr );
	LOGD( "@ size     = 0x%08X(%08d)\n", pField->size, pField->size );

	if( pField->isStatic ){
		sizeUsed = pField->size;
	}else{
		stMCELL* pCell = pField->field.pCellHead;

		if( isDetail ){
			LOGD( "@ cell  = {\n" );
		}

		// 管理データの出力
		while( pCell != NULL ){
			if( isDetail ){
				if( pCell->isAllocated ){ LOGD( "@  [*] rel=0x%08X, size=0x%08X(%8d)\n", pCell->addrRel, pCell->size, pCell->size ); }
				else{ LOGD( "@  [ ] rel=0x%08X, size=0x%08X(%8d)\n", pCell->addrRel, pCell->size, pCell->size ); }
			}

			if( pCell->isAllocated ){ sizeUsed += pCell->size; }
			else{ sizeFree += pCell->size; }
			pCell = pCell->pNext;
		}

		if( isDetail ){
			LOGD( "@ }\n" );
		}
	}
	LOGD( "@ status: %3.2f%%(used:%d/free:%d)\n", 100.0f*sizeUsed/pField->size, sizeUsed, sizeFree );

	if( !pField->isStatic ){
		uint32 cellActive = pField->field.numActiveCell;
		uint32 cellNum = pField->field.numCell;
		LOGD( "@ cell status: %3.2f%%(active:%u/%u)\n", 100.0f*cellActive/cellNum, cellActive, cellNum );
	}
}

//-------------------
// [DEBUG] 情報の表示
//-------------------
void CMemMgr::DrawLog( void ){
	char* buf = GetBufFromTempStr();
	stMEM_FIELD* pField;
	int i, used, rgba;
	float usedRate;

	//------------------------------
	// 右下（表示順は下から上）
	//------------------------------
	CDrawCell::SetLogAlignRB( 0.5f, 0, 0 );

	// 下段から表示
	for( i=eMEM_FIELD_MAX-1; i>=0; i-- ){
		pField = &s_stArrField[i];
		if( pField->pAddr != NULL ){

			// 使用サイズの算出
			if( pField->isStatic ){
				used = pField->size;
			}else{
				stMCELL* pCell = pField->field.pCellHead;
				used = 0;
				while( pCell != NULL ){
					if( pCell->isAllocated ){ used += pCell->size; }
					pCell = pCell->pNext;
				}
			}

			// 利用領域警告
			if( !pField->isStatic ){
				usedRate = 100.0f*used/pField->size;
				if( usedRate >= 96.0f ){ rgba = 0xFF8080FF; }
				else if( usedRate >= 92.0f ){ rgba = 0xFFFF80FF; }
				else{ rgba = 0x80FF80FF; }
			}else{
				rgba = 0xEEEEEEFF;
			}

			// field情報
			if( pField->size >= 1*MB ){
				sprintf( buf, "F[%d]:  %.1fm(%.1f%%)%d/%d: %s", i, pField->size/MBf, 100.0f*used/pField->size,
						pField->field.numActiveCell, pField->field.numCell, pField->field.pName );
			}else{
				sprintf( buf, "F[%d]:  %.1fk(%.1f%%)%d/%d: %s", i, pField->size/KBf, 100.0f*used/pField->size,
						pField->field.numActiveCell, pField->field.numCell, pField->field.pName );
			}
			CDrawCell::DrawLogAlign( buf, rgba );
		}
	}

	// 全体情報
	sprintf( buf, "M:%.1fm(%.1f%%)", s_nSize/MBf, 100.0f*s_nUsed/s_nSize );
	CDrawCell::DrawLogAlign( buf );

	// label
	sprintf( buf, "[CMemMgr]" );
	CDrawCell::DrawLogAlign( buf );
}

//---------------------------------------
// フィールド情報取得
//---------------------------------------
const stMEM_FIELD* CMemMgr::GetFieldInfo( eMEM_FIELD fieldId ){
	return( get_field( fieldId ) );
}

//------------------------
// フィールド設定
//------------------------
bool CMemMgr::set_field( eMEM_FIELD fieldId, uint32 size, bool isStatic, uint32 numCell, const char* pName, bool isDumpLog ){
	stMEM_FIELD* pField = get_field( fieldId );
	if( pField == NULL ){
		env_push_error_log( "メモリフィールドの作成に失敗しました。\n" );
		return( false );	// FATAL ERROR
	}

	void* pAddr;
	uint32 temp = s_nUsed + size;

	// サイズ超過
	if( temp > s_nSize ){
		LOGE( "@** [MEM] set_field size error field=%d, size=%d/%d **\n", fieldId, size, s_nSize-s_nUsed );
		env_push_error_log( "メモリフィールドの容量が足りません。\n" );
		return( false );	// FATAL ERROR
	}

	// メインメモリ確保
	pAddr = (void* )((ADDR)s_pAddr+s_nUsed);
	s_nUsed = temp;

	// 属性設定
	pField->pAddr     = pAddr;
	pField->size      = size;
	pField->isStatic  = isStatic;
	pField->sizePool  = 0;
	pField->pCellPool = NULL;

	// MCELLフィールド初期化
	mcell_field_init( &pField->field );

	// 動的設定
	if( !pField->isStatic ){
		// セルプール領域確保（※この時点ではまだ[CMemMgr]が使えないのでシステムから確保）
		pField->sizePool = mcell_calc_pool_size( numCell );
		pField->pCellPool = util_sys_mem_alloc( pField->sizePool, MEM_ALLOC_ALIGNMENT, "CMemMgr:pField->pCellPool" );
		if( pField->pCellPool == NULL ){
			LOGE( "@ [MEM] cellPool malloc error field=%d, poolSize=%d\n", fieldId, pField->sizePool );
			env_push_error_log( "メモリフィールドセルの確保に失敗しました。\n" );
			return( false );	// FATAL ERROR
		}

		// フィールド設定
		if( !mcell_field_set_name( &pField->field, pName ) ){
			env_push_error_log( "メモリフィールド名の設定に失敗しました。\n" );
			return( false );	// FATAL ERROR
		}

		if( !mcell_dump_log( &pField->field, isDumpLog ) ){
			env_push_error_log( "メモリフィールドのダンプ設定に失敗しました。\n" );
			return( false );	// FATAL ERROR
		}

		if( !mcell_field_set_memory( &pField->field, pField->pAddr, pField->size, MEM_ALLOC_CELL_ALIGN ) ){
			env_push_error_log( "メモリフィールドの設定に失敗しました。\n" );
			return( false );	// FATAL ERROR
		}

		if( !mcell_field_set_pool( &pField->field, pField->pCellPool, numCell, MEM_ALLOC_CELL_MARGIN ) ){
			env_push_error_log( "メモリフィールドのセルプールの設定に失敗しました。\n" );
			return( false );	// FATAL ERROR
		}
	}

	LOGD( "@ [MEM] set_field(%2d) addr=%p, size=0x%08X, isStatic=%d numCell=%3d(0x%06X:%p) name=%s\n",
		fieldId, pField->pAddr, pField->size, (pField->isStatic)? 1: 0, numCell, pField->sizePool, pField->pCellPool, pName );

	return( true );
}

//------------------------
// 共有フィールド設定
//------------------------
bool CMemMgr::set_field_share( eMEM_FIELD_SHARE fieldShareId, uint32 ofs, uint32 size ){
	stMEM_FIELD_SHARE* pFieldS = get_field_share( fieldShareId );
	if( pFieldS == NULL ){
		env_push_error_log( "共有メモリフィールドの作成に失敗しました。\n" );
		return( false );	// FATAL ERROR
	}

	pFieldS->ofs  = ofs;
	pFieldS->size = size;

	LOGD( "@ [MEM] set_field_share(%2d) ofs=0x%08X, size=0x%08X\n", fieldShareId, pFieldS->ofs, pFieldS->size );

	return( true );
}

//---------------------------
// 共有フィールドサイズ算出
//--------------------------
uint32 CMemMgr::calc_share_size( void ){
	int i;
	uint32 end = 0;
	stMEM_FIELD_SHARE* pFieldS;

	// 末端アドレスの検出
	for( i=0; i<eMEM_FIELD_SHARE_MAX; i++ ){
		pFieldS = get_field_share( (eMEM_FIELD_SHARE)i );
		if( pFieldS->size > 0 ){
			if( end < (pFieldS->ofs+pFieldS->size) ){
				end = (pFieldS->ofs+pFieldS->size);
			}
		}
	}

	return( end );
}
