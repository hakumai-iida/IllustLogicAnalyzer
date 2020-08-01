/*+----------------------------------------------------------------+
  |	Title:		new_delete.cpp [共通環境]
  |	Comment:	new/delete関連
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "new_delete.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
//#define DUMP_MEM_LOG

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
//----------------------------------------
// malloc
//----------------------------------------
void* MALLOC( size_t size ){
    void* pAddr = NULL;

    if( CMemMgr::IsAllocEnable() ){
        pAddr = CMemMgr::Alloc( (uint32)size );

#ifdef DUMP_MEM_LOG
        LOGD( "@ malloc(MGR) pAddr=%p size=%ld\n", pAddr, size );
#endif
	}else{
        pAddr = util_sys_mem_alloc( (uint32)size, MEM_ALLOC_ALIGNMENT, "SYSTEM" );

#ifdef DUMP_MEM_LOG
        LOGD( "@ malloc(SYS) pAddr=%p size=%ld\n", pAddr, size );
#endif
    }

    memset( pAddr, 0, size );   // 領域は０クリア
	return( pAddr );
}

//----------------------------------------
// new
//----------------------------------------
void* operator new( size_t size ){
    void* pAddr = NULL;

    if( CMemMgr::IsAllocEnable() ){
        pAddr = CMemMgr::Alloc( (uint32)size );

#ifdef DUMP_MEM_LOG
        LOGD( "@ new(MGR) pAddr=%p size=%ld\n", pAddr, size );
#endif
	}else{
        pAddr = util_sys_mem_alloc( (uint32)size, MEM_ALLOC_ALIGNMENT, "SYSTEM" );

#ifdef DUMP_MEM_LOG
        LOGD( "@ new(SYS) pAddr=%p size=%ld\n", pAddr, size );
#endif
    }

    memset( pAddr, 0, size );   // 領域は０クリア
	return( pAddr );
}

//------------------------------------------------------------------------
// new（※バッファ指定：既存の領域に対してコンストラクタを呼びたい時用）
//------------------------------------------------------------------------
void* operator new( size_t size, void* pAddr ){
#ifdef DUMP_MEM_LOG
    LOGD( "@ new(MGR/SYS) pAddr=%p\n", pAddr );
#endif

    memset( pAddr, 0, size );   // 領域は０クリア
	return( pAddr );            // 指定バッファを返すのみ
}

//----------------------------------------
// delete
//----------------------------------------
void operator delete( void* pAddr ){
	if( CMemMgr::IsManaged( pAddr ) ){
        CMemMgr::Free( pAddr );

#ifdef DUMP_MEM_LOG
        LOGD( "@ delete(MGR) pAddr=%p\n", pAddr );
#endif
	}else{
		util_sys_mem_free( pAddr );

#ifdef DUMP_MEM_LOG
        LOGD( "@ delete(SYS) pAddr=%p\n", pAddr );
#endif
	}
}

//----------------------------------------
// new[]
//----------------------------------------
void* operator new[]( size_t size ){
    void* pAddr = NULL;

    if( CMemMgr::IsAllocEnable() ){
        pAddr = CMemMgr::Alloc( (uint32)size );

#ifdef DUMP_MEM_LOG
        LOGD( "@ new[](MGR) pAddr=%p size=%ld\n", pAddr, size );
#endif
	}else{
        pAddr = util_sys_mem_alloc( (uint32)size, MEM_ALLOC_ALIGNMENT, "SYSTEM" );

#ifdef DUMP_MEM_LOG
        LOGD( "@ new[](SYS) pAddr=%p size=%ld\n", pAddr, size );
#endif
    }

    memset( pAddr, 0, size );   // 領域は０クリア
    return( pAddr );
}

//-----------------------------------------------------------
// new[]（※バッファ指定：既存の領域に対してコンストラクタを呼びたい時用）
//-----------------------------------------------------------
void* operator new[]( size_t size, void* pAddr ){
#ifdef DUMP_MEM_LOG
    LOGD( "@ new(MGR/SYS) pAddr=%p\n", pAddr );
#endif

    memset( pAddr, 0, size );   // 領域は０クリア
	return( pAddr );            // 指定バッファを返すのみ
}

//----------------------------------------
// delete[]
//----------------------------------------
void operator delete[]( void* pAddr ){
	if( CMemMgr::IsManaged( pAddr ) ){
		CMemMgr::Free( pAddr );

#ifdef DUMP_MEM_LOG
        LOGD( "@ delete[](MGR) pAddr=%p\n", pAddr );
#endif
	}else{
		util_sys_mem_free( pAddr );

#ifdef DUMP_MEM_LOG
        LOGD( "@ delete[](SYS) pAddr=%p\n", pAddr );
#endif
	}
}
