/*+----------------------------------------------------------------+
  |	Title:		time_log.hpp [共通環境]
  |	Comment:	時間ログ
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#ifndef __TIME_LOG_HPP__
#define __TIME_LOG_HPP__

/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
#define	TIME_LOG_BUF_SIZE	120

// タイマーＩＤ
enum eTIME_LOG_ID{
	// 更新関連
	eTIME_LOG_ID_FRAME,                 // フレーム時間計測
	eTIME_LOG_ID_UPDATE,                // 更新時間計測
	eTIME_LOG_ID_DRAW,                  // 描画時間計測

	// 初期化関連
	eTIME_LOG_ID_INIT_SYSTEM,           // [env::_init_system]
	eTIME_LOG_ID_INIT_DRAW,             // [env::_init_draw]
	eTIME_LOG_ID_INIT_TOUCH,            // [env::_init_touch]
	eTIME_LOG_ID_INIT_APP,              // [env::_init_app]

	// セーブデータ用
	eTIME_LOG_ID_PLAYER_DATA_LOAD,      // プレイヤーデータ読み込み
	eTIME_LOG_ID_PLAYER_DATA_SAVE,      // プレイヤーデータ保存
    
    // 画像処理用
    eTIME_LOG_ID_BEZIER,                // ベジェ処理
    eTIME_LOG_ID_TEX_TRANS,             // テクスチャ転送
    eTIME_LOG_ID_TEX_GEN,               // テクスチャ生成

	// マネージャ用
	eTIME_LOG_ID_LOAD_BGM,              // BGM読み込み
	eTIME_LOG_ID_LOAD_IMG,              // IMG読み込み
	eTIME_LOG_ID_LOAD_ACT,              // ACT読み込み
	eTIME_LOG_ID_LOAD_DEMO,             // DEMO読み込み

	eTIME_LOG_ID_TEMP_FOR_APP,          // テンポラリ：[CAppMgr]用
    eTIME_LOG_ID_TEMP_FOR_AUDIO_CELL,	// テンポラリ：[CAudioCell]用
	eTIME_LOG_ID_TEMP,                  // テンポラリ

	eTIME_LOG_ID_MAX
};

/*+----------------------------------------------------------------+
  |	Struct		構造体定義
  +----------------------------------------------------------------+*/
// 処理時間ログ
struct stTIME_LOG{
	int64	avg;
	int64	neck;
	int64	arrTime[TIME_LOG_BUF_SIZE];
	int	arrAt;
	int	count;
};

/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
//--------------
// ログ操作
//--------------
extern void time_log_init( stTIME_LOG* pLog );
extern void time_log_update( stTIME_LOG* pLog, int64 time );

//--------------
// タイマー
//--------------
extern void  time_log_start( int id );
extern int64 time_log_stop( int id );
extern int64 time_log_get( int id );

#endif /* __TIME_LOG_HPP__ */
