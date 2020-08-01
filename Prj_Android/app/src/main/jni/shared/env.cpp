/*+----------------------------------------------------------------+
  |	Title:		env.cpp [共通環境]
  |	Comment:	動作環境（※デバイスに依存しない処理）
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
// 1フレームの時間（マイクロ秒）
#define BASE_INTERVAL_US	16666

// 制御フラグバッファサイズ
#define FLAG_BUF_SIZE		16

/*+----------------------------------------------------------------+
  |	Struct		構造体定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
// 管理データ
stENV g_stEnv;

// システムフラグスタック（処理用）
static int s_nFlagAt;
static int s_nArrFlag[FLAG_BUF_SIZE];

// システムフラグスタック（登録用）
volatile int s_nFlagReserveAt;
volatile int s_nArrFlagReserve[FLAG_BUF_SIZE];

// 表示用
static const char* s_pArrFlagName[] = {
	"eECF_INIT_SYSTEM",
	"eECF_EXIT_SYSTEM",
	"eECF_INIT_DRAW",
	"eECF_EXIT_DRAW",
	"eECF_INIT_TOUCH",
	"eECF_EXIT_TOUCH",
	"eECF_INIT_APP",
	"eECF_EXIT_APP",
	"eECF_START_GAME",
	"eECF_END_GAME",
	"eECF_SUSPEND_GAME",
	"eECF_RESUME_GAME",
};

/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
// フラグに対する処理
static bool _exec_for_flag( int flag );

// システムの初期化／終了
static bool _init_system( void );
static bool _exit_system( void );

// 描画の初期化／終了
static bool _init_draw( void );
static bool _exit_draw( void );

// タッチの初期化／終了
static bool _init_touch( void );
static bool _exit_touch( void );

// アプリの初期化／終了
static bool _init_app( void );
static bool _exit_app( void );

// ゲームの開始／停止／中断／再開
static bool _play_game( void );
static bool _stop_game( void );
static bool _suspend_game( void );
static bool _resume_game( void );

// ゲームの更新／表示
static void _update_game( int64 delta );
static void _draw_game( void );

/*+----------------------------------------------------------------+
  |	Program		プログラム実装
  +----------------------------------------------------------------+*/
//------------------
// 環境初期化
//------------------
bool env_init( void ){
#ifdef MGR_INIT_LOG
	LOGD( "@ env_init()\n" );
#endif

	// 環境データクリア
	memset( &g_stEnv, 0, sizeof(stENV) );

	return( true );
}

//------------------
// 環境終了
//------------------
bool env_exit( void ){
#ifdef MGR_INIT_LOG
	LOGD( "@ env_exit()\n" );
#endif

	return( true );
}

//------------------
// 環境更新
//------------------
bool env_update( void ){
	// ゲーム中でない／中断していれば無視
	if( !g_stEnv.bGamePlaying || g_stEnv.bGameSuspended ){
		return( false );
	}

	//---------------------------
	// 更新時間確認
	//---------------------------
	int64 delta = time_log_stop( eTIME_LOG_ID_FRAME );

	// インターバル時間設定（※フレームレートが落ちている場合は半分に＝できる限り挽回させてみる）
	int64 wait = (g_stEnv.nFrameRate < 1000)? BASE_INTERVAL_US/2: BASE_INTERVAL_US;

	// 指定時間経過していなければ無視
	if( delta < wait ){
		return( false );
	}

	//---------------------------
	// フレーム更新
	//---------------------------
	time_log_update( &g_stEnv.stTimeLogFrame, delta );
	time_log_start( eTIME_LOG_ID_FRAME );

	g_stEnv.nFrame++;
	g_stEnv.nTimePast = util_sys_time_usec() - g_stEnv.nTimeStart;
	g_stEnv.nFrameRate = (int)(((int64)1000*BASE_INTERVAL_US*g_stEnv.nFrame)/g_stEnv.nTimePast);

	//---------------------------
	// ゲーム更新
	//---------------------------
	time_log_start( eTIME_LOG_ID_UPDATE );
	_update_game( delta );
	time_log_update( &g_stEnv.stTimeLogUpdate, time_log_stop( eTIME_LOG_ID_UPDATE ) );

	//---------------------------
	// ゲーム描画
	//---------------------------
	time_log_start( eTIME_LOG_ID_DRAW );
	_draw_game();
	time_log_update( &g_stEnv.stTimeLogUpdate, time_log_stop( eTIME_LOG_ID_DRAW ) );

	return( true );
}

//---------------------
// エラー関連
//---------------------
void env_set_fatal_error( bool flag ){ g_stEnv.bFatalError = flag; }
bool env_check_fatal_error( void ){ return( g_stEnv.bFatalError ); }
const char* env_get_error_log( void ){ return( g_stEnv.cArrErrorLog ); }

// ログ追加
void env_push_error_log( const char* pLog ){
	// 無効は無視
	if( pLog == NULL ){ return; }

	int len = (int)strlen( pLog );
	int use = (int)strlen( g_stEnv.cArrErrorLog );
	int rest = ERROR_LOG_BUFF_LEN - (use+1);

	if( len > rest ){
		LOGE( "@ env_push_error_log(%s): BUF SHORTAGE: %d > %d\n", pLog, (len+rest), ERROR_LOG_BUFF_LEN );
		return;
	}

	strcpy( &g_stEnv.cArrErrorLog[use], pLog );
}

//-----------------------------
// 入力イベント処理
//-----------------------------
bool env_push_input_event( stTOUCH_EVENT_DESCRIPTOR* pTed ){
	// ゲーム中であれば＆中断していなければ登録
	if( g_stEnv.bGamePlaying && !g_stEnv.bGameSuspended){
		if( CTouchMgr::PushLog( pTed ) ){
			return( true );
		}
	}
	return( false );
}

//---------------------------------------
// コールバックチェック
//---------------------------------------
bool env_check_callback( void ){
	// フラグ予約があれば処理枠に登録
	for( int i=0; i<s_nFlagReserveAt; i++ ){
		if( s_nFlagAt < FLAG_BUF_SIZE ){
			s_nArrFlag[s_nFlagAt++] = s_nArrFlagReserve[i];
		}else{
			LOGE( "@ util_sys_check_callback_flag: BUFFER OVERFLOW\n" );
			break;
		}
	}

	// 予約のクリア
	s_nFlagReserveAt = 0;

	// 処理枠がフラグが設定されていれば処理する
	while( s_nFlagAt > 0 ){
		// フラグ取り出し
		int flag = s_nArrFlag[0];

		// 処理枠のシフト
		s_nFlagAt--;
		for( int i=0; i<s_nFlagAt; i++ ){
			s_nArrFlag[i] = s_nArrFlag[i+1];
		}

		// フラグに対する処理
		LOGD( "@ env_check_callback_flag:[%s], rest=%d\n", s_pArrFlagName[flag], s_nFlagAt );
		if( !_exec_for_flag( flag ) ){
			return( false );
		}
	}

	return( true );
}

//---------------------------------------
// コールバックフラグ設定
//---------------------------------------
bool env_set_callback_flag( eENV_CALLBACK_FLAG flag ){
	if( s_nFlagReserveAt < FLAG_BUF_SIZE ){
		LOGD( "@ env_set_callback_flag:%s at %d\n", s_pArrFlagName[flag], s_nFlagReserveAt );
		s_nArrFlagReserve[s_nFlagReserveAt++] = flag;
		return( true );
	}

	LOGE( "@ env_set_callback_flag: BUFFER OVERFLOW\n" );
	return( false );
}

//---------------------------------------
// システム処理
//---------------------------------------
bool _exec_for_flag( int flag ){
	bool ret = false;

	switch( flag ){
	case eECF_INIT_SYSTEM:		ret = _init_system();		break;
	case eECF_EXIT_SYSTEM:		ret = _exit_system();		break;
	case eECF_INIT_DRAW:		ret = _init_draw();			break;
	case eECF_EXIT_DRAW:		ret = _exit_draw();			break;
	case eECF_INIT_TOUCH:		ret = _init_touch();		break;
	case eECF_EXIT_TOUCH:		ret = _exit_touch();		break;
	case eECF_INIT_APP:			ret = _init_app();			break;
	case eECF_EXIT_APP:			ret = _exit_app();			break;
	case eECF_PLAY_GAME:		ret = _play_game();			break;
	case eECF_STOP_GAME:		ret = _stop_game();			break;
	case eECF_SUSPEND_GAME:		ret = _suspend_game();		break;
	case eECF_RESUME_GAME:		ret = _resume_game();		break;
	}

	// いずれかの処理に失敗したら致命的
	if( !ret ){
		char buf[256];
		sprintf( buf, "\n【発生箇所】\n%s\n\nご不便をおかけいたしますが、アプリを終了いたします。\n", s_pArrFlagName[flag] );
		env_push_error_log( buf );
		env_set_fatal_error( true );
	}

	return( ret );
}

//-------------------------------
// システム初期化：処理の基本部分
//-------------------------------
static bool _init_system( void ){
	time_log_start( eTIME_LOG_ID_INIT_SYSTEM );
#ifdef MGR_INIT_LOG
	LOGD( "@ env: [_init_system]:%s\n", ((g_stEnv.bSystemValid)? "true": "false") );
#endif

	// 初期化済みは正常終了
	if( g_stEnv.bSystemValid ){ return( true ); }

	// 表示の環境依存要素初期化
	if( ! env_native_init_display() ){
		env_push_error_log( "[env_native_init_display()]に失敗しました。\n" );
		return( false );	// FATAL_ERROR
	}

	// 画面サイズ確定
    if( ! CMainRenderer::OnDisplayChange( g_stEnvNative.nDisplayWidth, g_stEnvNative.nDisplayHeight ) ){
        env_push_error_log( "[CMainRenderer::OnDisplayChange()]に失敗しました。\n" );
        return( false );    // FATAL ERROR
    }

	// メモリマネージャの初期化
	if( ! CMemMgr::Init() ){
		env_push_error_log( "[CMemMgr::Init()]に失敗しました。\n" );
		return( false );	// FATAL_ERROR
	}

	// ファイルマネージャの初期化
	if( ! CFileMgr::Init() ){
		env_push_error_log( "[CFileMgr::Init()]に失敗しました。\n" );
		return( false );	// FATAL_ERROR
	}

    // 定義テーブルの初期化
    if( ! CDefTable::Init() ){
        env_push_error_log( "[CDefTable::Init()]に失敗しました。\n" );
        return( false );    // FATAL_ERROR
    }

    // ループマネージャの初期化
    if( ! CLoopMgr::Init() ){
        env_push_error_log( "[CLoopMgr::Init()]に失敗しました。\n" );
        return( false );    // FATAL_ERROR
    }

	// 管理フラグオン
	g_stEnv.bSystemValid = true;
	time_log_stop( eTIME_LOG_ID_INIT_SYSTEM );
	return( true );
}

//-------------------------------
// システム終了
//-------------------------------
static bool _exit_system( void ){
#ifdef MGR_INIT_LOG
	LOGD( "@ env: [_exit_system]:%s\n", ((g_stEnv.bSystemValid)? "true": "false") );
#endif

	// 終了済みは正常終了
	if( ! g_stEnv.bSystemValid ){ return( true ); }

    // ループマネージャの終了
    CLoopMgr::Exit();
    
    // テーブルの終了
    CDefTable::Exit();

	// ファイルマネージャの終了
	CFileMgr::Exit();

#ifdef MGR_INIT_LOG
	// 管理メモリが全て解放されているか？
	//LOGD( "@ MEMORY INFO at [_exit_system] ===========\n" );
	//for( int i=0; i<eMEM_FIELD_MAX; i++ ){
	//	CMemMgr::ReportField( (eMEM_FIELD) i );
	//}
#endif

	// メモリマネージャの終了
	CMemMgr::Exit();

	// 管理フラグオフ
	g_stEnv.bSystemValid = false;
	return( true );
}

//-------------------------------
// 描画初期化：表示部分
//-------------------------------
static bool _init_draw( void ){
	time_log_start( eTIME_LOG_ID_INIT_DRAW );
#ifdef MGR_INIT_LOG
	LOGD( "@ env: [_init_draw]:%s\n", ((g_stEnv.bDrawValid)? "true": "false") );
#endif

	// 初期化済みは正常終了
	if( g_stEnv.bDrawValid ){ return( true ); }

	// レンダラー初期化
	if( ! CMainRenderer::OnCreate() ){
		env_push_error_log( "[CMainRenderer::OnCreate()]に失敗しました。\n" );
		return( false );	// FATAL ERROR
	}

	// 管理フラグオン
	g_stEnv.bDrawValid = true;
	time_log_stop( eTIME_LOG_ID_INIT_DRAW );
	return( true );
}

//-------------------------------
// 描画終了
//-------------------------------
static bool _exit_draw( void ){
#ifdef MGR_INIT_LOG
	LOGD( "@ env: [_exit_draw]:%s\n", ((g_stEnv.bDrawValid)? "true": "false") );
#endif

	// 終了済みは正常終了
	if( ! g_stEnv.bDrawValid ){ return( true ); }

	// レンダラー破棄
	CMainRenderer::OnDestroy();

	// 表示の環境依存部分破棄
	env_native_exit_display();
    
	// 管理フラグオフ
	g_stEnv.bDrawValid = false;
	return( true );
}

//-------------------------------
// タッチ初期化
//-------------------------------
static bool _init_touch( void ){
	time_log_start( eTIME_LOG_ID_INIT_TOUCH );
#ifdef MGR_INIT_LOG
	LOGD( "@ env: [_init_touch]:%s\n", ((g_stEnv.bTouchValid)? "true": "false") );
#endif

	// 初期化済みは正常終了
	if( g_stEnv.bTouchValid ){ return( true ); }

	// タッチマネージャ開始
	if( ! CTouchMgr::OnCreate() ){
		env_push_error_log( "[CTouchMgr::OnCreate()]に失敗しました。\n" );
		return( false );	// FATAL_ERROR
	}

	// 管理フラグオン
	g_stEnv.bTouchValid = true;
	time_log_stop( eTIME_LOG_ID_INIT_TOUCH );
	return( true );
}

//----------------------------
// タッチ終了
//----------------------------
static bool _exit_touch( void ){
#ifdef MGR_INIT_LOG
	LOGD( "@ env: [_exit_touch]:%s\n", ((g_stEnv.bTouchValid)? "true": "false") );
#endif

	// 終了済みは正常終了
	if( ! g_stEnv.bTouchValid ){ return( true ); }

	// タッチマネージャ終了
	CTouchMgr::OnDestroy();

	// 管理フラグオフ
	g_stEnv.bTouchValid = false;
	return( true );
}

//-------------------------------
// アプリ初期化
//-------------------------------
static bool _init_app( void ){
	time_log_start( eTIME_LOG_ID_INIT_APP );
#ifdef MGR_INIT_LOG
	LOGD( "@ env: [_init_app]:%s\n", ((g_stEnv.bAppValid)? "true": "false") );
#endif

	// 初期化済みは正常終了
	if( g_stEnv.bAppValid ){ return( true ); }
    
    // 乱数初期化
    CRand::Init();
    
	// アプリマネージャ開始
	if( ! CAppMgr::OnAppStart() ){
		env_push_error_log( "[CAppMgr::OnAppStart()]に失敗しました。\n" );
		return( false );	// FATAL_ERROR
	}

    // ループ開始
    CLoopMgr::ReserveLoop( NULL, eLOOP_RESERVE_CREATE, eLOOP_ID_LOGIC_TEST, 0 );

	// 管理フラグオン
	g_stEnv.bAppValid = true;
	time_log_stop( eTIME_LOG_ID_INIT_APP );
	return( true );
}

//----------------------------
// アプリ終了
//----------------------------
static bool _exit_app( void ){
#ifdef MGR_INIT_LOG
	LOGD( "@ env: [_exit_app]:%s\n", ((g_stEnv.bAppValid)? "true": "false") );
#endif

	// 終了済みは正常終了
	if( ! g_stEnv.bAppValid ){ return( true ); }

	// アプリマネージャ終了
	CAppMgr::OnAppEnd();
    
    // 乱数終了
    CRand::Exit();

	// 管理フラグオフ
	g_stEnv.bAppValid = false;
	return( true );
}

//-----------------------------
// ゲーム開始
//-----------------------------
static bool _play_game( void ){
#ifdef MGR_INIT_LOG
	LOGD( "@ env: [_play_game]:%s\n", ((g_stEnv.bGamePlaying)? "true": "false") );
#endif

	// 開始済みは正常終了
	if( g_stEnv.bGamePlaying ){ return( true ); }

	// タイマー関連リセット
	time_log_init( &g_stEnv.stTimeLogFrame );
	time_log_init( &g_stEnv.stTimeLogUpdate );
	time_log_init( &g_stEnv.stTimeLogDraw );

	g_stEnv.nFrame = 0;
	g_stEnv.nFrameRate = 1000;
	g_stEnv.nTimePast = 0;
	g_stEnv.nTimeStart = util_sys_time_usec();

	// ループタイマー開始
	time_log_start( eTIME_LOG_ID_FRAME );

#ifdef MGR_INIT_LOG
	// ゲーム開始時のメモリ状況
	//LOGD( "@ MEMORY INFO at [_play_game] ===========\n" );
	//for( int i=0; i<eMEM_FIELD_MAX; i++ ){
	//	CMemMgr::ReportField( (eMEM_FIELD) i );
	//}
#endif

	// 入力クリア
	CTouchMgr::ClearStatusAll();

	// 管理フラグオン
	g_stEnv.bGamePlaying = true;
	return( true );
}

//-----------------------------
// ゲーム停止
//-----------------------------
static bool _stop_game( void ){
#ifdef MGR_INIT_LOG
	LOGD( "@ env: [_stop_game]:%s\n", ((g_stEnv.bGamePlaying)? "true": "false") );
#endif

	// 停止済みは正常終了
	if( ! g_stEnv.bGamePlaying ){ return( true ); }

	// 管理フラグオフ
	g_stEnv.bGamePlaying = false;
	return( true );
}

//-----------------------------
// ゲーム中断
//-----------------------------
static bool _suspend_game( void ){
#ifdef MGR_INIT_LOG
	LOGD( "@ env: [_suspend_game]:%s\n", ((g_stEnv.bGameSuspended)? "true": "false") );
#endif

	if( ! g_stEnv.bSystemValid ){ return( true ); }
	if( ! g_stEnv.bAppValid ){ return( true ); }
	if( ! g_stEnv.bDrawValid ){ return( true ); }
	if( ! g_stEnv.bTouchValid ){ return( true ); }

	// 中断済みなら正常終了
	if( g_stEnv.bGameSuspended ){ return( true ); }

    // ループサスペンド
    CLoopMgr::OnAppSuspend();

    // アプリサスペンド
    CAppMgr::OnAppSuspend();
    
    // 描画サスペンド
    CMainRenderer::OnSuspend();

	// 環境サスペンド
	env_native_suspend_loop();

	// 管理フラグオン
	g_stEnv.bGameSuspended = true;
	return( true );
}

//-----------------------------
// ゲーム再開
//-----------------------------
static bool _resume_game( void ){
#ifdef MGR_INIT_LOG
	LOGD( "@ env: [_resume_game]:%s\n", ((g_stEnv.bGameSuspended)? "true": "false") );
#endif

	// 再開済みなら正常終了
	if( ! g_stEnv.bGameSuspended ){ return( true ); }

    // 環境リジューム
    env_native_resume_loop();

    // 描画再開
    CMainRenderer::OnResume();

    // アプリ再開
    CAppMgr::OnAppResume();

    // ループ再開
    CLoopMgr::OnAppResume();

	// 管理フラグオフ
	g_stEnv.bGameSuspended = false;
	return( true );
}

//------------------
// ゲーム更新
//------------------
static void _update_game( int64 delta ){
	delta += g_stEnv.nDeltaRemain;

	// タッチマネージャ更新（※アプリ更新前）
	CTouchMgr::Update();

	// アプリ更新（※時間の差分をマイクロ秒で渡す）
	CAppMgr::UpdateGame( delta );
	CAppMgr::CheckDebugInput();

	// タッチ演出更新（※アプリ更新後）
	CTouchMgr::UpdateForGame();

	// 余りは次回更新へ
	g_stEnv.nDeltaRemain = delta%1000;
}

//------------------
// ゲーム描画
//------------------
static void _draw_game( void ){
	if( env_native_render_start() ){
		CMainRenderer::OnDrawFrame();
		env_native_render_end();
	}
}
