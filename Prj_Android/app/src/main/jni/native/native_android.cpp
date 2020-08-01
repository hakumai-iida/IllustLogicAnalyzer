/*+----------------------------------------------------------------+
  |	Title:		native_android.cpp [Android環境]
  |	Comment:	メイン
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
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
/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
static const char* s_pArrAppCmdName[] = {
	"APP_CMD_INPUT_CHANGED",
	"APP_CMD_INIT_WINDOW",
	"APP_CMD_TERM_WINDOW",
	"APP_CMD_WINDOW_RESIZED",
	"APP_CMD_WIDOW_REDRAW_NEEDED",
	"APP_CMD_CONTENT_RECT_CHANGED",
	"APP_CMD_GAINED_FOCUS",
	"APP_CMD_LOST_FOCUS",
	"APP_CMD_CONFIG_CHANGED",
	"APP_CMD_LOW_MEMORY",
	"APP_CMD_START",
	"APP_CMD_RESUME",
	"APP_CMD_SAVE_STATE",
	"APP_CMD_PAUSE",
	"APP_CMD_STOP",
	"APP_CMD_DESTROY",
};

/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
// ハンドラ
static void on_app_cmd( struct android_app* pApp, int cmd );
static int on_input_event( struct android_app* pApp, AInputEvent* event );

/*+----------------------------------------------------------------+
  |	Program		プログラム実装
  +----------------------------------------------------------------+*/
//-------------------
// Main関数
//-------------------
void android_main( struct android_app* pApp ){
	LOGD( "@ android_main: START" );

	// コールバック設定
	pApp->onAppCmd = on_app_cmd;
	pApp->onInputEvent = on_input_event;

	// 環境初期化
	env_init();
	env_native_init( pApp );

	// 予め環境依存処理をよんでおく（※ゲーム中に呼ぶと、初回時に数フレームかかるようなため）
	env_native_setKeepScreen( false );

	//----------------------------------------
	// ゲームループ開始
	//----------------------------------------
	LOGD( "@ android_main: LOOP START" );

	int id, event;
	bool isErrorDialogActive = false;
	struct android_poll_source* pSource;

	while( true ){
		// イベント処理
		while( (id=ALooper_pollAll( 1, NULL, &event, (void**)&pSource)) >= 0 ){
			// 内部イベントを処理する
			if( pSource != NULL ){
				pSource->process( pApp, pSource );
			}

			// 情報取得キューのデータを処理する
			if( id == LOOPER_ID_USER ){
			}
		}

		// 終了指定
		if( pApp->destroyRequested != 0 ){
			LOGD( "@ [pApp->destroyRequested]!!\n" );
			break;
		}

		// エラーダイアログ表示中なら、ユーザーの入力を待ってアプリ終了
		if( isErrorDialogActive ){
			if( !env_native_isDialogBusy() ){
				break;
			}
		}else{
			// メモリ管理有効
			CMemMgr::SetAllocEnable( true );

			// コールバック確認（※制御系のエラーはここで発生する）
			env_check_callback();

			// 致命的エラーが発生していたらダイアログ表示
			if( env_check_fatal_error() ){
				isErrorDialogActive = true;
				env_native_callDialog( "エラーが発生しました", env_get_error_log(), "OK", NULL, NULL );
			}else{
				// 環境更新
				env_update();
			}

			// メモリ管理無効
			CMemMgr::SetAllocEnable( false );
		}
	}

	//-----------------------------
	// 終了処理
	//-----------------------------
	LOGD( "@ android_main: END\n" );
	env_check_callback();
	env_native_exit();
	env_exit();

	// エラー発生時は明示的な終了も読んでおく
	//(※端末によっては再度[android_main]が呼ばれたりするので)
	if( env_check_fatal_error() ){
		env_native_exitApp( true );
	}
}

//==================================================================
// コマンドハンドラ
//==================================================================
// 起動時／再開時のコマンド発行の流れ
//   [ APP_CMD_START]
//   [ APP_CMD_RESUME]
//   [ APP_CMD_INPUT_CHANGED](※呼ばれないこともある)
//   [*APP_CMD_INIT_WINDOW]
//   [*APP_CMD_GAINED_FOCUS]
//------------------------------------------------------------------
// 中断(ホームボタン／メニューを押した等)時のコマンド発行の流れ
//   [*APP_CMD_PAUSE]
//   [ APP_CMD_LOST_FOCUS] (※状況により前後する)
//   [*APP_CMD_TERM_WINDOW](※状況により前後する)
//   [ APP_CMD_SAVE_STATE](※呼ばれないこともある)
//   [ APP_CMD_STOP]
//------------------------------------------------------------------
// 戻る(戻るボタンを押した)時は中断時に追加して下記が発行される
//（※中断時、タスクメニューからタスクを削除した場合も下記が発行される）
//   [ APP_CMD_INPUT_CHANGED](※呼ばれないこともある)
//   [*APP_CMD_DESTROY]（※[pApp->destroyRequested]が立つ）
//------------------------------------------------------------------
// ロック(電源ボタン／放置)時のコマンド発行の流れ
//   [*APP_CMD_PAUSE]
//   [ APP_CMD_SAVE_STATE]
//   [ APP_CMD_STOP]
//  （※放置の場合は一瞬ここで留まる＆この時点ではフォーカスが保持される）
//   [ APP_CMD_LOST_FOCUS]（※ここでフォーカスを失う）
//   (※端末によりアプリの破棄まで行く場合もある)
//   ( APP_CMD_TERM_WINDOW)
//   ( APP_CMD_INPUT_CHANGED)
//   ( APP_CMD_CMD_DESTROY)
//------------------------------------------------------------------
// ロック解除時の復帰の流れ（※破棄されている場合は起動の流れとなる）
//   [ APP_CMD_START]
//   [ APP_CMD_RESUME]
//   [*APP_CMD_TERM_WINDOW](※呼ばれないこともある)
//   [*APP_CMD_INIT_WINDOW](※呼ばれないこともある)
//   [*APP_CMD_GAINED_FOCUS]（※[APP_CMD_LOST_FOCUS]が呼ばれていた場合に呼ばれる）
//------------------------------------------------------------------
static void on_app_cmd( struct android_app* pApp, int cmd ){
	LOGD( "@ on_app_cmd: pApp=%08X, cmd=[%d:%s]\n", (uint32)pApp, cmd, s_pArrAppCmdName[cmd] );

	switch( cmd ){
    // [ ] AInputQueueが変化した
	case APP_CMD_INPUT_CHANGED:
		break;

	// [*] ANativeWindowが利用できるようになった
	case APP_CMD_INIT_WINDOW:
		// 全て初期化
		env_set_callback_flag( eECF_INIT_SYSTEM );
		env_set_callback_flag( eECF_INIT_DRAW );
		env_set_callback_flag( eECF_INIT_TOUCH );
		env_set_callback_flag( eECF_INIT_APP );
		break;

	// [*] ANativeWindowが終了要求を出した
	case APP_CMD_TERM_WINDOW:
		// 環境に依存する部分のみ解放
		env_set_callback_flag( eECF_EXIT_DRAW );
		break;

	// [ ] ANativeWindowがリサイズ要求を出した
	case APP_CMD_WINDOW_RESIZED:
		break;

	// [ ] ANativeWindowが再描画の要求を出した
	case APP_CMD_WINDOW_REDRAW_NEEDED:
		break;

	// [ ] Windowの内部表示領域が変化した：ソフトウェアキーボードが表示された等
    case APP_CMD_CONTENT_RECT_CHANGED:
		break;

	// [*] Activityがアクティブ状態になった
	case APP_CMD_GAINED_FOCUS:
		// ここでゲームの開始
		env_set_callback_flag( eECF_PLAY_GAME );
		env_set_callback_flag( eECF_RESUME_GAME );

		// フラグを立てておく
		g_stEnvNative.bFocusGained = true;
		break;

	// [*] Activityが非アクティブ状態になった
	case APP_CMD_LOST_FOCUS:
		// フラグを寝かせておく
		g_stEnvNative.bFocusGained = false;
		break;

	// [ ] 機器の設定が変化した
	case APP_CMD_CONFIG_CHANGED:
		break;

	// [ ] システムのメモリが不足した
	case APP_CMD_LOW_MEMORY:
		break;

	// [ ] Activityが開始した:onStart
	case APP_CMD_START:
		break;

	// [*] Activityが再開した:onResume
	case APP_CMD_RESUME:
		// フォーカスを保持していたらここで復帰
		if( g_stEnvNative.bFocusGained ){
			env_set_callback_flag( eECF_PLAY_GAME );
			env_set_callback_flag( eECF_RESUME_GAME );
		}
		break;

	// [ ] 状態保存の開始（※利用しない）
	case APP_CMD_SAVE_STATE:
		break;

	// [*] Activityが一時停止した:onPause
	case APP_CMD_PAUSE:
		// アプリ中断／終了時の最初に必ず呼ばれるようなので、 ここでゲームの停止
		env_set_callback_flag( eECF_SUSPEND_GAME );
		env_set_callback_flag( eECF_STOP_GAME );
		break;

	// [ ] Activityが停止した:onStop
	case APP_CMD_STOP:
		break;

	// [*] Activityが破棄された:onDestroy
	case APP_CMD_DESTROY:
		// 全て終了
		env_set_callback_flag( eECF_EXIT_APP );
		env_set_callback_flag( eECF_EXIT_TOUCH );
		env_set_callback_flag( eECF_EXIT_DRAW );
		env_set_callback_flag( eECF_EXIT_SYSTEM );
		break;

	default:
		break;
	}
}

//---------------------------
// 入力ハンドラ
//---------------------------
static int on_input_event( struct android_app* /*pApp*/, AInputEvent* pEvent ){
	// 入力を渡す
	stTOUCH_EVENT_DESCRIPTOR ted;
	ted.pInputEvent = pEvent;
	if( env_push_input_event( &ted ) ){
		// 受理されたら処理済みとする
		return( 1 );
	}

	return( 0 );
}
