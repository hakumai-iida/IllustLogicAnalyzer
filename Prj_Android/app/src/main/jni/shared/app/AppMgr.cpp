/*+----------------------------------------------------------------+
  |	Title:		AppMgr.cpp [共通環境]
  |	Comment:	アプリ管理
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "AppMgr.hpp"
#include "obj/TouchObj.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Struct		構造体型宣言
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
// 時間関連
int64 CAppMgr::s_nEnvTimePastUs;
float CAppMgr::s_fEnvTimePast;
float CAppMgr::s_fEnvTimeDelta;

CFade* CAppMgr::s_pFade;
bool CAppMgr::s_bPauseUpdate;

/*+----------------------------------------------------------------+
  |	Program		プログラム実装
  +----------------------------------------------------------------+*/
//----------------------------------------------------------
// 初期化
//----------------------------------------------------------
bool CAppMgr::OnAppStart( void ){
#ifdef MGR_INIT_LOG
	LOGD( "@ CAppMgr::OnAppStart()\n" );
#endif

	//-------------------------------------
	// Bg
	//-------------------------------------
	if( !CBgMgr::OnAppStart() ){
		LOGE( "@ CBgMgr::OnAppStart(): FAILED!!\n" );
		env_push_error_log( "[CBgMgr::OnAppStart()]に失敗しました。\n" );
		return( false );	// FATAL ERROR
	}

	// 領域確保
	CMemMgr::PushTargetField( eMEM_FIELD_D_APP );
	s_pFade = new CFade();
	CMemMgr::PopTargetField();

	SetDebug( true );

	// リセット
	ResetTime();
	CUpdater::ResetAll();
    CTouchMgr::ClearStatusAll();

	// フェードイン
	SetFadeRGBA( 0x000000FF );
	SetFadeIn();

	return( true );
}

//--------------------------
// 終了
//--------------------------
void CAppMgr::OnAppEnd( void ){
#ifdef MGR_INIT_LOG
	LOGD( "@ CAppMgr::OnAppEnd()\n" );
#endif

	SAFE_DELETE( s_pFade );

	CBgMgr::OnAppEnd();
}

//--------------------------
// 停止
//--------------------------
void CAppMgr::OnAppSuspend( void ){
    LOGD( "@ CAppMgr::OnAppSuspend\n" );
}

//--------------------------
// 再開
//--------------------------
void CAppMgr::OnAppResume( void ){
    LOGD( "@ CAppMgr::OnAppResume\n" );
}

//-----------------------------------
// デバッグ設定
//-----------------------------------
void CAppMgr::SetDebug ( bool flag ){
	CTouchObj::SetTouchAreaCheck( flag );
}

//-----------------------------------
// デバッグログ設定
//-----------------------------------
void CAppMgr::SetDebugLog( bool flag, bool isAll ){
    CMainRenderer::SetLogOn( flag );
	CMainRenderer::SetLogAll( isAll );
}

//-----------------------------------
// タッチ演出指定
//-----------------------------------
void CAppMgr::SetTouchEffect( bool isDraw, bool isSe ){
	CTouchMgr::SetTouchEffect( isDraw );
	CTouchMgr::SetTouchEffectSe( isSe );
}

//-----------------------------------------------------------------------
// バッファ切り替え（※ボス戦前の表示要素をボス戦後に引き継ぐための処理）
//-----------------------------------------------------------------------
void CAppMgr::SwitchMgrForBoss( bool isBoss, bool isClear ){
	// ボス戦用のバッファへ
	if( isBoss ){
		CBgMgr::SwitchBufForExtra();
#if 0
		CPopUpMgr::SwitchBufForExtra();
		CParticleMgr::SwitchBufForExtra();
		CTcbMgr::SwitchBufForExtra();
#endif
	// 通常バッファへ
	}else{
		CBgMgr::SwitchBufForNormal();
#if 0
		CPopUpMgr::SwitchBufForNormal();
		CParticleMgr::SwitchBufForNormal();
		CTcbMgr::SwitchBufForNormal();
#endif
	}

	// 指定があればクリア
	if( isClear ){
		// [CBgMgr]はクリア処理がない
#if 0
		CPopUpMgr::ReleaseAllForCur();
		CParticleMgr::ReleaseAllForCur();
		CTcbMgr::ReleaseAllForCur();
#endif
	}
}

//----------------------------------------
// 時間リセット
//----------------------------------------
void CAppMgr::ResetTime( void ){
	s_nEnvTimePastUs = 0;
	s_fEnvTimePast = 0.0f;
	s_fEnvTimeDelta = 0.0f;
}

//-----------------------------------------------------
// ゲーム更新（※前回からの経過時間をミリ秒で受け取る）
//-----------------------------------------------------
void CAppMgr::UpdateGame( int64 delta ){
	// 時間更新準備
	UpdateTime( delta );

	if( s_bPauseUpdate ){
		CUpdater::ReadyForUpdate( 0.0f );
	}else{
		CUpdater::ReadyForUpdate( s_fEnvTimeDelta );
	}

	// フレーム差分がある限り更新
	CTouchMgr::SetIgnoreDuplicated( false );
	while( CUpdater::CheckThenFixDeltaFrame() ){
		// 各種マネージャ更新
		CLoopMgr::Update();
#if 0
		CTcbMgr::Update();
		CParticleMgr::Update();
		CPopUpMgr::Update();
#endif
		CBgMgr::Update();

		// フェード更新
		s_pFade->update();

		// １フレーム目終了
		CUpdater::SetUpdateFirstFrameDone();

		// 以降の処理では入力情報を無視（※重複判定されないように）
		CTouchMgr::SetIgnoreDuplicated( true );
	}

	// 入力判定無視をオフ
	CTouchMgr::SetIgnoreDuplicated( false );
}

//-----------------------------------------------------------
// タイム更新（※前回からの経過時間をマイクロ秒で受け取る）
//-----------------------------------------------------------
void CAppMgr::UpdateTime( int64 deltaUs ){
	s_nEnvTimePastUs += deltaUs;

	// 用心（※オーバーフローしたらリセット）
	if( s_nEnvTimePastUs < 0 ){
		ResetTime();

		s_nEnvTimePastUs += deltaUs;
	}

	float past0 = s_fEnvTimePast;
	s_fEnvTimePast = s_nEnvTimePastUs / 1000000.0f;
	s_fEnvTimeDelta = s_fEnvTimePast - past0;
}

//-----------------------------------------------------------------------
// ゲーム描画
//-----------------------------------------------------------------------
// ゲーム要素の描画は[CDrawer::ResetObjs]後に全オブジェクトを描画登録し、
// [CDrawer::DrawObjs]により優先順位に沿って描画する流れとなる
// [RegistForDraw]の呼び出し順が描画順とならない点に注意すること
//-----------------------------------------------------------------------
void CAppMgr::DrawGame( eDRAW_PROCESS process ){
	// 描画登録開始
	CDrawer::ReadyForRegist( process );

	// 登録
	CBgMgr::RegistForDraw();
	CLoopMgr::RegistForDraw();
	s_pFade->registForDraw();

	// 登録した要素の描画
	CDrawer::DrawObjs();

	// 適用オフ
	CDrawCell::ResetApplyRate();
}

//---------------------------
// [DEBUG] ログの表示
//---------------------------
void CAppMgr::DrawLog( void ){
	char* buf = CMemMgr::GetBufFromTempStr();

	//------------------------------
	// 右上
	//------------------------------
	CDrawCell::SetLogAlignRT( 0.5f, 0, 168 );

	int time;

	time = (int)time_log_get( eTIME_LOG_ID_INIT_SYSTEM );
	sprintf( buf, "initSystem: %d.%03d", time/1000, time%1000 );
	CDrawCell::DrawLogAlign( buf );

	time = (int)time_log_get( eTIME_LOG_ID_INIT_DRAW );
	sprintf( buf, "initDraw: %d.%03d", time/1000, time%1000 );
	CDrawCell::DrawLogAlign( buf );

	time = (int)time_log_get( eTIME_LOG_ID_INIT_TOUCH );
	sprintf( buf, "initTouch: %d.%03d", time/1000, time%1000 );
	CDrawCell::DrawLogAlign( buf );

	time = (int)time_log_get( eTIME_LOG_ID_INIT_APP );
	sprintf( buf, "initApp: %d.%03d", time/1000, time%1000 );
	CDrawCell::DrawLogAlign( buf );

	time = (int)time_log_get( eTIME_LOG_ID_LOAD_BGM );
	sprintf( buf, "loadBGM: %d.%03d", time/1000, time%1000 );
	CDrawCell::DrawLogAlign( buf );

	time = (int)time_log_get( eTIME_LOG_ID_LOAD_IMG );
	sprintf( buf, "loadIMG: %d.%03d", time/1000, time%1000 );
	CDrawCell::DrawLogAlign( buf );

	time = (int)time_log_get( eTIME_LOG_ID_LOAD_ACT );
	sprintf( buf, "loadACT: %d.%03d", time/1000, time%1000 );
	CDrawCell::DrawLogAlign( buf );

	time = (int)time_log_get( eTIME_LOG_ID_LOAD_DEMO );
	sprintf( buf, "loadDEMO: %d.%03d", time/1000, time%1000 );
	CDrawCell::DrawLogAlign( buf );

	time = (int)time_log_get( eTIME_LOG_ID_PLAYER_DATA_LOAD );
	sprintf( buf, "@LOAD: %d.%03d", time/1000, time%1000 );
	CDrawCell::DrawLogAlign( buf );

	time = (int)time_log_get( eTIME_LOG_ID_PLAYER_DATA_SAVE );
	sprintf( buf, "@SAVE: %d.%03d", time/1000, time%1000 );
	CDrawCell::DrawLogAlign( buf );
}

//-------------------------
// [DEBUG] デバッグ入力確認
//-------------------------
void CAppMgr::CheckDebugInput( void ){
#if 0
    int checkSize = 128;
    int checkOfs = checkSize/2;
    
	// 画面左上が押されていたら入力チェック
	CTouchStatusCell* pCheckBase = CTouchMgr::CheckOval( eTOUCH_CHECK_PRESSED, -checkOfs, -checkOfs, checkSize, checkSize );

	if( pCheckBase != NULL ){
		CTouchStatusCell* pCheck;
		// 画面中上がタッチされた：スクリーンショット
		pCheck = CTouchMgr::CheckOval( eTOUCH_CHECK_TOUCHED_DOWN, SA_W/2-checkOfs, -checkOfs, checkSize, checkSize );
		if( pCheck != NULL ){
			// スクリーンショットの出力
			CMainRenderer::ReserveScreenShot();
		}

		// 画面左下がタッチされた：背景切替
		pCheck = CTouchMgr::CheckOval( eTOUCH_CHECK_TOUCHED_DOWN, 0-checkOfs, SA_H-checkOfs, checkSize, checkSize );
		if( pCheck != NULL ){
			CBgMgr::SetNextBg();
		}

		// 画面右上がタッチされた：速度アップ
		pCheck = CTouchMgr::CheckOval( eTOUCH_CHECK_TOUCHED_DOWN, SA_W-checkOfs, -checkOfs, checkSize, checkSize );
		if( pCheck != NULL ){
			float speed = CUpdater::GetUpdateSpeed();
			speed += 0.1f;
			CUpdater::SetUpdateSpeed( speed );
		}

		// 画面右中がタッチされた
		 pCheck = CTouchMgr::CheckOval( eTOUCH_CHECK_TOUCHED_DOWN, SA_W-checkOfs, SA_H/2-checkOfs, checkSize, checkSize );
		if( pCheck != NULL ){
			s_bPauseUpdate = !s_bPauseUpdate;		// ポーズ切り替え
			//CUpdater::SwapUpdateByTime();			// 更新モード切替
		}

		// 画面右下がタッチされた：速度ダウン
		pCheck = CTouchMgr::CheckOval( eTOUCH_CHECK_TOUCHED_DOWN, SA_W-checkOfs, SA_H-checkOfs, checkSize, checkSize );
		if( pCheck != NULL ){
			float speed = CUpdater::GetUpdateSpeed();
			speed -= 0.1f;
			if( speed < 0.1f ){ speed = 0.1f; }
			CUpdater::SetUpdateSpeed( speed );
		}

		// 画面中下がタッチされた：ログ切替
		pCheck = CTouchMgr::CheckOval( eTOUCH_CHECK_TOUCHED_DOWN, SA_W/2-checkOfs, SA_H-checkOfs, checkSize, checkSize );
		if( pCheck != NULL ){
			// デバッグ出力切替
			CMainRenderer::SwapLogOn();
		}
        
        // 画面左下がタッチされた：ログ切替
        pCheck = CTouchMgr::CheckOval( eTOUCH_CHECK_TOUCHED_DOWN, 0-checkOfs, SA_H-checkOfs, checkSize, checkSize );
        if( pCheck != NULL ){
#if 0
            // メモリダンプ（ついで）
            LOGD( "@ MEMORY INFO ==========================\n" );
            for( int i=0; i<eMEM_FIELD_MAX; i++ ){
                CMemMgr::ReportField( (eMEM_FIELD) i );
            }
            CMemMgr::ReportField( eMEM_FIELD_D_FREE );
#endif

            // デバッグ出力タイプ切替
            CMainRenderer::SwapLogAll();
        }
	}
#endif
}

//--------------------------
// 終了
//--------------------------
void CAppMgr::Finish( void ){ env_native_exitApp( false ); }
void CAppMgr::ExitForce( void ){ env_native_exitApp( true ); }

//------------------------------
// スクリーン設定（※消灯しない）
//------------------------------
void CAppMgr::SetKeepScreen( bool flag ){ env_native_setKeepScreen( flag ); }

//--------------------------
// サイト呼び出し
//--------------------------
void CAppMgr::OpenSite( const char* pUrl ){ env_native_openSite( pUrl ); }

//--------------------------
// バッテリー残量取得
//--------------------------
int CAppMgr::GetBatteryRest( void ){ return( env_native_getBatteryRest() ); }

//--------------------------
// cpu種別取得
//--------------------------
const char* CAppMgr::GetCpuArchitecture( void ){ return( env_native_getCpuArchitecture() ); }

//--------------------------
// バージョンを取得
//--------------------------
const char* CAppMgr::GetVersion( void ){ return( env_native_getVersion() ); }

//----------------------------
// ビルドバージョンを取得
//----------------------------
const char* CAppMgr::GetBuildVersion( void ){ return( env_native_getBuildVersion() ); }

//--------------------------
// 文字列テクスチャ作成
//--------------------------
BYTE* CAppMgr::CreateStringTexture( const char* pText, int type, int size, stINFO_FOR_STRING_TEXTURE* pInfo ){
	return( env_native_createStringTexture( pText, type, size, pInfo ) );
}

