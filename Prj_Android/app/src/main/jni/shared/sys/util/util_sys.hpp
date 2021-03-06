﻿/*+----------------------------------------------------------------+
  |	Title:		util_sys.hpp [共通環境]
  |	Comment:	システム関連ユーティリティー
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#ifndef __UTIL_SYS_HPP__
#define __UTIL_SYS_HPP__

/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "sys/mem/new_delete.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Function	関数
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Struct		構造体型宣言
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
// メモリ確保／解放（※システムから実際に確保する、[CMemMgr]管理外のメモリ操作）
extern void* util_sys_mem_alloc( uint32 size, uint32 alignment=MEM_ALLOC_ALIGNMENT, const char* pComment=NULL );
extern void  util_sys_mem_free( void *pAddr, const char* pComment=NULL );

// システム時間
extern int64 util_sys_time_sec( void );		// 秒
extern int64 util_sys_time_msec( void );	// ミリ秒
extern int64 util_sys_time_usec( void );	// マイクロ秒

// ローカル時間
extern void util_sys_local_time( int* pYear, int* pMonth, int* pDay, int *pWDay, int* pHour, int* pMin, int* pSec );

#endif /* __UTIL_SYS_HPP__ */
