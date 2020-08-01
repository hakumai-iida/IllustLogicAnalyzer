/*+----------------------------------------------------------------+
  |	Title:		util_sys.cpp [共通環境]
  |	Comment:	システム関連ユーティリティー
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include <time.h>

#include "env.hpp"
#include "util_sys.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
//#define DUMP_MEM_LOG

/*+----------------------------------------------------------------+
  |	Struct		構造体型宣言
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
// 調整クロック
static int64 _clock( void );

/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
#ifdef EXE_ANDROID
// [Android] クロックの桁あふれ調整用
static int64 _lastClock;
static int64 _clockLoopCount;
#endif

/*+----------------------------------------------------------------+
  |	Program		プログラム領域
  +----------------------------------------------------------------+*/
//---------------------------------
// メモリ確保
//---------------------------------
void* util_sys_mem_alloc( uint32 size, uint32 alignment, const char* pComment ){
	void *pAddr = NULL;

#ifdef EXE_ANDROID
	// [Android]
    pAddr = memalign( alignment, size );
#endif

#ifdef EXE_IOS
    // [iOS]
    posix_memalign( &pAddr, alignment, size );
#endif

#ifdef DUMP_MEM_LOG
	if( pComment != NULL ){
		if( pAddr == NULL ){ LOGE( "@ [!] util_sys_mem_alloc[FAILED]( size=0x%08X, align=0x%08X ): %s\n", size, alignment, pComment ); }
		else{ LOGD( "@ [+] util_sys_mem_alloc[%p]( size=0x%08X, align=0x%08X ): %s\n", pAddr, size, alignment, pComment ); }
	}else{
		if( pAddr == NULL ){ LOGE( "@ [!] util_sys_mem_alloc[FAILED]( size=0x%08X, align=0x%08X )\n", size, alignment ); }
		else{ LOGD( "@ [+] util_sys_mem_alloc[%p]( size=0x%08X, align=0x%08X )\n", pAddr, size, alignment ); }
	}
#endif

	return( pAddr );
}

//---------------------------------
// メモリ解放
//---------------------------------
void util_sys_mem_free( void* pAddr, const char* pComment ){
#ifdef DUMP_MEM_LOG
	if( pComment != NULL ){ LOGD( "@ [-] util_sys_mem_free( pAddr=%p ): %s\n", pAddr, pComment ); }
	else{ LOGD( "@ [-] util_sys_mem_free( pAddr=%p )\n", pAddr ); }
#endif

	free( pAddr );
}

//---------------------------------
// システム時間
//---------------------------------
// 秒
int64 util_sys_time_sec( void ){
	return( _clock()/CLOCKS_PER_SEC );
}

// ミリ秒
int64 util_sys_time_msec( void ){
	return( _clock()/(CLOCKS_PER_SEC/1000) );
}

// マイクロ秒
int64 util_sys_time_usec( void ){
	return( _clock()/(CLOCKS_PER_SEC/1000000) );
}

//-------------------------------------------
// クロック実体（※必ず正の値を返す）
//-------------------------------------------
// [clock]の返値[clock_t]は[long]型のため、
// 最上位ビットが立つ瞬間に正負が入れ替わる
// 時間計測においで不整合がおこらないように調整した値を返す
//-------------------------------------------
static int64 _clock( void ){
    int64 totalClock = 0;
#ifdef EXE_ANDROID
#if 0
    // [Android] システムクロック取得
    // [Android OS 5.0]ではポーリング中にクロックが進まない(?)ようなため、
    // [clock]の値と実時間にずれが発生してしまう
	int64 curClock = clock();

	// [ANDROID] 正負が入れ替わったら（※[clock()]が負を返す場合、起動直後にもここの判定に通る）
	if( curClock < _lastClock ){
		_clockLoopCount++;
		LOGE( "@ util::_clock(): CHANGE MSB at %lldTH LOOP!!\n", _clockLoopCount );
	}
	_lastClock = curClock;

	// [ANDROID] ループを加味したトータル時間に調整
	totalClock = curClock + (_clockLoopCount*0x100000000);
#else
	// 実時間から取得：[Android OS 5.0]対応
	struct timeval tv;
	gettimeofday( &tv, NULL );

	// 秒の部分が一回りしていたら分が繰り上がったと判断
	if( tv.tv_sec < _lastClock ){
		_clockLoopCount++;
	}

	_lastClock = tv.tv_sec;

	// 現在の秒＋マイクロ秒＋経過分により、累計マイクロ秒を算出
	totalClock = (tv.tv_sec+60*_clockLoopCount)*1000000 + tv.tv_usec;
#endif
#endif

#ifdef EXE_IOS
    // [iOS] メディア時間（※システムクロックだとスレッドが処理している時間しか計上されない）
    // この処理は[Objective-C++]でコンパイルすること
    totalClock = 1000000*CACurrentMediaTime();
#endif
	return( totalClock );
}

//---------------------
// ローカル時間の取得
//---------------------
void util_sys_local_time( int* pYear, int* pMonth, int* pDay, int *pWDay, int* pHour, int* pMin, int* pSec ){
	time_t timer;
	time( &timer );

	struct tm *pTM = localtime( &timer );
	if( pYear != NULL ){ *pYear = pTM->tm_year+ 1900; }
	if( pMonth != NULL ){ *pMonth = pTM->tm_mon + 1; }
	if( pDay != NULL ){ *pDay = pTM->tm_mday; }
	if( pWDay != NULL ){ *pWDay = pTM->tm_wday; }
	if( pHour != NULL ){ *pHour = pTM->tm_hour; }
	if( pMin != NULL ){ *pMin = pTM->tm_min; }
	if( pSec != NULL ){ *pSec = pTM->tm_sec; }
}
