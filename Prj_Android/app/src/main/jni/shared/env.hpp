/*+----------------------------------------------------------------+
  |	Title:		env.hpp [共通環境]
  |	Comment:	動作環境（※デバイスに依存しない処理）
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#ifndef __ENV_HPP__
#define __ENV_HPP__

/*+----------------------------------------------------------------+
  |	Environment	環境指定
  +----------------------------------------------------------------+*/
//------------------------------------------
// 環境定義：環境スイッチ読み込み
//------------------------------------------
#include "native/env_native_switch.hpp"

// レンダーログ
#define RENDER_LOG

// 初期化関連ログ出力
#define	MGR_INIT_LOG

/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
//--------------------------------
// 基本ヘッダ
//--------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <math.h>

// システム定数
#include "sys/types.hpp"
#include "sys/common.hpp"

// 依存環境
#include "env_native.hpp"

// コンフィグ（※アプリの内容に依存した定義部分）
#include "config/config_for_app.hpp"

// システム
#include "sys/util/util_sys.hpp"
#include "sys/util/util_str.hpp"
#include "sys/math/Math.hpp"
#include "sys/data/Enum.hpp"
#include "sys/log/time_log.hpp"
#include "sys/file/FileMgr.hpp"
#include "sys/mem/MemMgr.hpp"
#include "sys/mem/new_delete.hpp"

// 定数
#include "const/Const.hpp"
#include "def/DefConst.hpp"
#include "draw/DrawConst.hpp"
#include "draw/tex/bezier/BezierConst.hpp"

// 入出力
#include "sys/data/InputBuffer.hpp"
#include "sys/data/OutputBuffer.hpp"
#include "sys/data/FileHeader.hpp"

// def
#include "def/DefTable.hpp"

// 入力
#include "touch/TouchMgr.hpp"

// 描画
#include "draw/MainRenderer.hpp"

// ループ
#include "loop/LoopMgr.hpp"

// アプリ
#include "app/AppMgr.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
// エラーログ保存用（※起動エラー等の文言表示用）
#define ERROR_LOG_BUFF_LEN	4096

//------------------
// 制御フラグ
//------------------
enum eENV_CALLBACK_FLAG{
	eECF_INVALID = -1,

	// システム初期化／終了
	eECF_INIT_SYSTEM,
	eECF_EXIT_SYSTEM,

	// 描画初期化／終了
	eECF_INIT_DRAW,
	eECF_EXIT_DRAW,

	// 入力初期化／終了
	eECF_INIT_TOUCH,
	eECF_EXIT_TOUCH,

	// アプリ初期化／終了
	eECF_INIT_APP,
	eECF_EXIT_APP,

	// ゲーム開始／停止
	eECF_PLAY_GAME,
	eECF_STOP_GAME,

	// ゲーム中断／再開
	eECF_SUSPEND_GAME,
	eECF_RESUME_GAME,

	eECF_MAX
};

/*+----------------------------------------------------------------+
  |	Struct		構造体型宣言
  +----------------------------------------------------------------+*/
// 環境
typedef struct{
	// 管理フラグ（※下に行くほどゲームより）
	bool bSystemValid;		// システムが有効か？
	bool bDrawValid;		// 描画が有効か？
	bool bTouchValid;		// タッチが有効か？
	bool bAppValid;			// アプリが有効か？
	bool bGamePlaying;		// ゲーム中か？
	bool bGameSuspended;	// ゲームが中断したか？

	// 計測関連
	int nFrame;				// 経過フレーム
	int nFrameRate;			// フレームレート（千分率）
	int64 nTimePast;		// 経過時間(us)
	int64 nTimeStart;		// 開始時刻(us)
	int64 nDeltaRemain;		// 時間差分あまり(us)

	// タイマーログ
	stTIME_LOG stTimeLogFrame;
	stTIME_LOG stTimeLogUpdate;
	stTIME_LOG stTimeLogDraw;

	// エラー遷移用
	bool bFatalError;
	char cArrErrorLog[ERROR_LOG_BUFF_LEN];

} stENV;

/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
// 管理データ
extern stENV g_stEnv;

/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
//---------------------
// 基本処理
//---------------------
extern bool env_init( void );		// 初期化
extern bool env_exit( void );		// 終了
extern bool env_update( void );		// 更新

// 入力イベント通知
extern bool env_push_input_event( stTOUCH_EVENT_DESCRIPTOR* pTed );

//---------------------
// エラー制御
//---------------------
extern void env_set_fatal_error( bool flag );
extern bool env_check_fatal_error( void );
extern const char* env_get_error_log( void );
extern void env_push_error_log( const char* pLog );

//---------------------
// 制御関連
//---------------------
// コールバック確認＆登録
extern bool env_check_callback( void );
extern bool env_set_callback_flag( eENV_CALLBACK_FLAG flag );

#endif /* __ENV_HPP__ */
