/*+----------------------------------------------------------------+
  |	Title:		time_log.cpp [共通環境]
  |	Comment:	時間ログ
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "time_log.hpp"

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
static int64	s_arr_time_start[eTIME_LOG_ID_MAX];
static int64	s_arr_time_log[eTIME_LOG_ID_MAX];

/*+----------------------------------------------------------------+
  |	Program		プログラム実装
  +----------------------------------------------------------------+*/
//----------------------
// ログ初期化
//----------------------
void time_log_init( stTIME_LOG* pLog ){
	memset( pLog, 0, sizeof(stTIME_LOG) );
}

//----------------------
// ログ更新
//----------------------
void time_log_update( stTIME_LOG* pLog, int64 time ){
	int  i, id, max;
	int64 neck, total;

	// 当該ログの更新
	pLog->arrTime[pLog->arrAt] = time;
	pLog->count++;

	// ネックと平均の算出
	neck = 0;
	total = 0;
	id = pLog->arrAt;
	max = pLog->count;
	if( max >= TIME_LOG_BUF_SIZE ){ max = TIME_LOG_BUF_SIZE; }

	for( i=0; i<max; i++ ){
		total += pLog->arrTime[id];
		if( neck < pLog->arrTime[id] ){ neck = pLog->arrTime[id]; }

		// ログをさかのぼる
		id--;
		if( id < 0 ){ id = TIME_LOG_BUF_SIZE - 1; }
	}

	// ネックと平均の更新
	pLog->neck = neck;
	pLog->avg = total/max;

	// 更新
	pLog->arrAt++;
	if( pLog->arrAt >= TIME_LOG_BUF_SIZE ){ pLog->arrAt = 0; }
}

//-------------------------
// タイマー開始
//-------------------------
void time_log_start( int id ){
	if( id < 0 || id >= eTIME_LOG_ID_MAX ){
		LOGE( "@ time_log_start(%d) error\n", id );
		return;
	}

	// タイマー開始
	s_arr_time_start[id] = util_sys_time_usec();
}

//-------------------------
// タイマー停止
//-------------------------
int64 time_log_stop( int id ){
	if( id < 0 || id >= eTIME_LOG_ID_MAX ){
		LOGE( "@ time_log_stop(%d) error\n", id );
		return( 0 );
	}

	int64 end = util_sys_time_usec();
	if( end < s_arr_time_start[id] ){
		LOGE( "@ time_log_stop(%d): TIME INVALID: %lld < %lld\n", id, end, s_arr_time_start[id] );
		return( 0 );
	}

	// 経過時間を算出して返す
	s_arr_time_log[id] = end - s_arr_time_start[id];
	return( s_arr_time_log[id] );
}

//-------------------------
// タイマーログ
//-------------------------
int64 time_log_get( int id ){
	return( s_arr_time_log[id] );
}
