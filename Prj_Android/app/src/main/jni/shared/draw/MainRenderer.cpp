/*+----------------------------------------------------------------+
  |	Title:		MainRenderer::cpp [GLES2.0環境]
  |	Comment:	レンダラー
  |             指定は環境に依存しないゲーム座標で行う
  |             描画は[CDrawCell]経由で行い、[CShader]のレンダー時に
  |             フレームバッファ座標に変換される
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "MainRenderer.hpp"

#include "sys/util/util_endian.hpp"

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
// デバイス関連
int CMainRenderer::s_nDisplayDevicePixelRate;
float CMainRenderer::s_fDisplayNativeScale;

// ゲーム座標関連
int CMainRenderer::s_nGamePixelRate;
int CMainRenderer::s_nRequiredW;
int CMainRenderer::s_nRequiredH;
int CMainRenderer::s_nGameW;
int CMainRenderer::s_nGameH;
int CMainRenderer::s_nGameSafeAreaMarginT;
int CMainRenderer::s_nGameSafeAreaMarginB;
int CMainRenderer::s_nGameSafeAreaMarginL;
int CMainRenderer::s_nGameSafeAreaMarginR;
int CMainRenderer::s_nGameSafeAreaW;
int CMainRenderer::s_nGameSafeAreaH;

// フレームバッファ関連
int CMainRenderer::s_nFrameBufferW;
int CMainRenderer::s_nFrameBufferH;
int CMainRenderer::s_nScreenShotW;
int CMainRenderer::s_nScreenShotH;

// 現在の描画対象
eFRAME_BUFFER CMainRenderer::s_eFrameBufferCur;
eFRAME_BUFFER CMainRenderer::s_eFrameBufferCached;
int CMainRenderer::s_nCurFrameBufferW;
int CMainRenderer::s_nCurFrameBufferH;

// フラグ
bool CMainRenderer::s_bLogOn;
bool CMainRenderer::s_bLogAll;
bool CMainRenderer::s_bReserveCache;
bool CMainRenderer::s_bOffScreenCache;

BYTE CMainRenderer::s_nAlphaForDrawMono;
BYTE CMainRenderer::s_nAlphaForDrawNega;
BYTE CMainRenderer::s_nAlphaForDrawSepia;

// スクリーンショット関連
bool CMainRenderer::s_bSSDump;
int CMainRenderer::s_nSSCount;
char CMainRenderer::s_cArrDumpFileName[COMMON_NAME_LEN];

/*+----------------------------------------------------------------+
  |	Program		プログラム実装
  +----------------------------------------------------------------+*/
//----------------------------------
// サーフェイス切替時（※画面サイズ確定）
//----------------------------------
bool CMainRenderer::OnDisplayChange( int devW, int devH ){
#ifdef MGR_INIT_LOG
    LOGD( "@ CMainRenderer::OnDisplayChange:\n" );
#endif

    //------------------------------
    // デバイスピクセル率適用
    //------------------------------
#ifdef EXE_IOS
    s_nDisplayDevicePixelRate = g_stEnvNative.nDisplayDevicePixelRate;
    s_fDisplayNativeScale = g_stEnvNative.fDisplayNativeScale;
#else
    // [iOS]以外でピクセル率の概念は無い
    s_nDisplayDevicePixelRate = 1;
    s_fDisplayNativeScale = 1.0f;
#endif
    
    // ピクセル率をデバイスサイズへ適用すると実解像度となる
    devW *= s_nDisplayDevicePixelRate;
    devH *= s_nDisplayDevicePixelRate;
    
    //------------------------------
    // 要求サイズ算出
    //------------------------------
    // アプリを起動するのに必要な最低要件
    s_nGamePixelRate = REQUIRED_P_RATE;
    s_nRequiredW = REQUIRED_W_FOR_P_RATE;
    s_nRequiredH = REQUIRED_H_FOR_P_RATE;

    // 解像度が想定に足りていなければ致命的
    if( devW < s_nGamePixelRate*s_nRequiredW || devH < s_nGamePixelRate*s_nRequiredH ){
        char* pBuf = CMemMgr::GetBufFromTempStr();
        sprintf( pBuf, "解像度が不足しています。\n（※このアプリは[%dx%d]の解像度が必要ですが確保できた解像度は[%dx%d]でした）\n",
                s_nGamePixelRate*s_nRequiredW, s_nGamePixelRate*s_nRequiredH, devW, devH );
        env_push_error_log( pBuf );
        return( false );
    }
    
    // 高解像度向けのピクセル率を確認
    if( devW >= s_nGamePixelRate*STEP_W_FOR_P_RATE && devH >= s_nGamePixelRate*STEP_H_FOR_P_RATE ){
        // 要求エリアを格上げ（※タテ／ヨコの判定比率を変更）
        s_nRequiredW = STEP_W_FOR_P_RATE;
        s_nRequiredH = STEP_H_FOR_P_RATE;

        // ピクセル率の算出
        while( devW >= (s_nGamePixelRate+1)*STEP_W_FOR_P_RATE && devH >= (s_nGamePixelRate+1)*STEP_H_FOR_P_RATE ){
            s_nGamePixelRate += 1;
        }
    }
    
    //--------------------------------------
    // ゲームエリア／フレームバッファサイズ確定
    //--------------------------------------
    // ゲームエリア確定（※ピクセル率で割り切れない端数は無視しておく）
    s_nGameW = devW / s_nGamePixelRate;
    s_nGameH = devH / s_nGamePixelRate;

    // フレームバッファサイズ確定（※ゲームエリアサイズにピクセル率をかけたもの）
    s_nFrameBufferW = s_nGamePixelRate * s_nGameW;
    s_nFrameBufferH = s_nGamePixelRate * s_nGameH;

    //------------------------------
    // セーフエリア算出
    //------------------------------
#ifdef EXE_IOS
    // 横向き(ランドスケール)時はちゃんと横向きの値が設定されてくるので向きは気にしなくても良い
    int nDeviceSafeAreaMarginT = g_stEnvNative.nDisplayDevicePixelRate*g_stEnvNative.fSafeAreaMarginTop;
    int nDeviceSafeAreaMarginB = g_stEnvNative.nDisplayDevicePixelRate*g_stEnvNative.fSafeAreaMarginBottom;
    int nDeviceSafeAreaMarginL = g_stEnvNative.nDisplayDevicePixelRate*g_stEnvNative.fSafeAreaMarginLeft;
    int nDeviceSafeAreaMarginR = g_stEnvNative.nDisplayDevicePixelRate*g_stEnvNative.fSafeAreaMarginRight;

    // ゲームエリア座標へ変換（※上下左右に端数がでるかもしれないが、とりあえず無視しておく）
    s_nGameSafeAreaMarginT = nDeviceSafeAreaMarginT/s_nGamePixelRate;
    s_nGameSafeAreaMarginB = nDeviceSafeAreaMarginB/s_nGamePixelRate;
    s_nGameSafeAreaMarginL = nDeviceSafeAreaMarginL/s_nGamePixelRate;
    s_nGameSafeAreaMarginR = nDeviceSafeAreaMarginR/s_nGamePixelRate;
#else
    // [iOS]以外ではセーフエリアの概念はない
    s_nGameSafeAreaMarginT = 0;
    s_nGameSafeAreaMarginB = 0;
    s_nGameSafeAreaMarginL = 0;
    s_nGameSafeAreaMarginR = 0;
#endif

    // セーフエリア確定
    s_nGameSafeAreaW = s_nGameW - (s_nGameSafeAreaMarginL+s_nGameSafeAreaMarginR);
    s_nGameSafeAreaH = s_nGameH - (s_nGameSafeAreaMarginT+s_nGameSafeAreaMarginB);
    
    //-----------------------------------
    // スクリーンショットサイズ
    //-----------------------------------
    // 画像サイズが極大になられると困るので最小限の解像度でバッファを確保する
    s_nScreenShotW = REQUIRED_P_RATE * s_nGameW;
    s_nScreenShotH = REQUIRED_P_RATE * s_nGameH;
    
    // 用心に４の倍数へ
    s_nScreenShotW = 4*((s_nScreenShotW+3)/4);
    s_nScreenShotH = 4*((s_nScreenShotH+3)/4);

#ifdef MGR_INIT_LOG
    LOGD( "@ RENDERER INFO\n" );
    LOGD( "@  s_nDisplayDevicePixelRate = %d\n", s_nDisplayDevicePixelRate );
    LOGD( "@  s_fDisplayNativeScale     = %f\n", s_fDisplayNativeScale );
    LOGD( "@  s_nGamePixelRate          = %d\n", s_nGamePixelRate );
    LOGD( "@  s_nRequiredW              = %d\n", s_nRequiredW );
    LOGD( "@  s_nRequiredH              = %d\n", s_nRequiredH );
    LOGD( "@  s_nGameW                  = %d\n", s_nGameW );
    LOGD( "@  s_nGameH                  = %d\n", s_nGameH );
    LOGD( "@  s_nGameSafeAreaMarginT    = %d\n", s_nGameSafeAreaMarginT );
    LOGD( "@  s_nGameSafeAreaMarginB    = %d\n", s_nGameSafeAreaMarginB );
    LOGD( "@  s_nGameSafeAreaMarginL    = %d\n", s_nGameSafeAreaMarginL );
    LOGD( "@  s_nGameSafeAreaMarginR    = %d\n", s_nGameSafeAreaMarginR );
    LOGD( "@  s_nGameSafeAreaW          = %d\n", s_nGameSafeAreaW );
    LOGD( "@  s_nGameSafeAreaH          = %d\n", s_nGameSafeAreaH );
    LOGD( "@  s_nFrameBufferW           = %d\n", s_nFrameBufferW );
    LOGD( "@  s_nFrameBufferH           = %d\n", s_nFrameBufferH );
    LOGD( "@  s_nScreenShotW            = %d\n", s_nScreenShotW );
    LOGD( "@  s_nScreenShotH            = %d\n", s_nScreenShotH );
    LOGD( "@  IGNORE (%d, %d) pixels\n", devW-s_nFrameBufferW, devH-s_nFrameBufferH );
#endif
    
    return( true );
}

//----------------------
// 初期化
//----------------------
bool CMainRenderer::OnCreate( void ){
#ifdef MGR_INIT_LOG
	LOGD( "@ CMainRenderer::OnCreate()\n" );
#endif
       
    if( ! CBezier::OnCreate() ){
        LOGE( "@ CBezier::OnCreate(): FAILED!!\n" );
        env_push_error_log( "[CBezier::OnCreate()]に失敗しました。\n" );
        return( false );
    }
    
    if( ! CStrokeMgr::OnCreate() ){
        LOGE( "@ CStrokeMgr::OnCreate(): FAILED!!\n" );
        env_push_error_log( "[CStrokeMgr::OnCreate()]に失敗しました。\n" );
        return( false );
    }
    
    if( ! CFillMgr::OnCreate() ){
        LOGE( "@ CFillMgr::OnCreate(): FAILED!!\n" );
        env_push_error_log( "[CFillMgr::OnCreate()]に失敗しました。\n" );
        return( false );
    }
    
    if( ! CBmpGenerator::OnCreate() ){
        LOGE( "@ CBmpGenerator::OnCreate(): FAILED!!\n" );
        env_push_error_log( "[CBmpGenerator::OnCreate()]に失敗しました。\n" );
        return( false );
    }
    
	if( ! CShaderMgr::OnCreate() ){
		LOGE( "@ CShaderMgr::OnCreate(): FAILED!!\n" );
		env_push_error_log( "[CShaderMgr::OnCreate()]に失敗しました。\n" );
		return( false );	// FATAL ERROR
	}
	
	if( ! CFrameBufferMgr::OnCreate() ){
		LOGE( "@ CFrameBufferMgr::OnCreate(): FAILED!!\n" );
		env_push_error_log( "[CFrameBufferMgr::OnCreate()]に失敗しました。\n" );
		return( false );	// FATAL ERROR
	}
    
    if( ! CImgMgr::OnCreate() ){
		LOGE( "@ CImgMgr::OnCreate(): FAILED!!\n" );
		env_push_error_log( "[CImgMgr::OnCreate()]に失敗しました。\n" );
		return( false );
	}
	
	if( ! CSprMgr::OnCreate() ){
		LOGE( "@ CSprMgr::OnCreate(): FAILED!!\n" );
		env_push_error_log( "[CSprMgr::OnCreate()]に失敗しました。\n" );
		return( false );	// FATAL ERROR
	}

	if( ! CGrpMgr::OnCreate() ){
		LOGE( "@ CGrpMgr::OnCreate(): FAILED!!\n" );
		env_push_error_log( "[CGrpMgr::OnCreate()]に失敗しました。\n" );
		return( false );	// FATAL ERROR
	}
	
	if( ! CFontMgr::OnCreate() ){
		LOGE( "@ CFontMgr::OnCreate(): FAILED!!\n" );
		env_push_error_log( "[CFontMgr::OnCreate()]に失敗しました。\n" );
		return( false );	// FATAL ERROR
	}

	if( ! CNumMgr::OnCreate() ){
		LOGE( "@ CNumMgr::OnCreate(): FAILED!!\n" );
		env_push_error_log( "[CNumMgr::OnCreate()]に失敗しました。\n" );
		return( false );	// FATAL ERROR
	}

	if( ! CTexCacheMgr::OnCreate() ){
		LOGE( "@ CTexCacheMgr::OnCreate(): FAILED!!\n" );
		env_push_error_log( "[CTexCacheMgr::OnCreate()]に失敗しました。\n" );
		return( false );
	}

	if( ! CTexStringMgr::OnCreate() ){
		LOGE( "@ CTexStringMgr::OnCreate(): FAILED!!\n" );
		env_push_error_log( "[CTexStringMgr::OnCreate()]に失敗しました。\n" );
		return( false );
	}
    
    if( ! CTexMgr::OnCreate() ){
        LOGE( "@ CTexMgr::OnCreate(): FAILED!!\n" );
        env_push_error_log( "[CTexMgr::OnCreate()]に失敗しました。\n" );
        return( false );
    }
    
	// 初期値
	s_eFrameBufferCur = eFRAME_BUFFER_HEAD;
	s_eFrameBufferCached = eFRAME_BUFFER_TAIL;

	s_bReserveCache = false;
	s_bOffScreenCache = false;

	return( true );
}

//----------------------
// 終了
//----------------------
void CMainRenderer::OnDestroy( void ){
#ifdef MGR_INIT_LOG
	LOGD( "@ CMainRenderer::OnDestroy\n" );
#endif

    CTexMgr::OnDestroy();
	CTexStringMgr::OnDestroy();
	CTexCacheMgr::OnDestroy();
	CNumMgr::OnDestroy();
	CFontMgr::OnDestroy();
	CGrpMgr::OnDestroy();
	CSprMgr::OnDestroy();
	CImgMgr::OnDestroy();
	CFrameBufferMgr::OnDestroy();
	CShaderMgr::OnDestroy();
    CBmpGenerator::OnDestroy();
    CFillMgr::OnDestroy();
    CStrokeMgr::OnDestroy();
    CBezier::OnDestroy();
}

//----------------------
// サスペンド
//----------------------
void CMainRenderer::OnSuspend( void ){
    LOGD( "@ CMainRenderer::OnSuspend\n" );
}

//----------------------
// リジューム
//----------------------
void CMainRenderer::OnResume( void ){
    LOGD( "@ CMainRenderer::OnResume\n" );
}

//-----------------------------
// フレームバッファのキャッシュ
//-----------------------------
eFRAME_BUFFER CMainRenderer::CacheCurFrameBuffer( void ){
	// 現在のフレームバッファをキャッシュに退避
	s_eFrameBufferCached = s_eFrameBufferCur;

	// フレームバッファを更新
	s_eFrameBufferCur = (eFRAME_BUFFER)(s_eFrameBufferCur + 1);
	if( s_eFrameBufferCur > eFRAME_BUFFER_TAIL ){
		s_eFrameBufferCur = eFRAME_BUFFER_HEAD;
	}

	return( s_eFrameBufferCached );
}

//----------------------
// 描画時
//----------------------
void CMainRenderer::OnDrawFrame( void ){
	//---------------------------
	// 予約があればキャッシュ
	//---------------------------
	if( s_bReserveCache ){
		CacheCurFrameBuffer();
	}

	// テクスチャキャッシュ管理更新
	CTexCacheMgr::Update();
	CTexStringMgr::Update();

	//-----------------
	// ゲーム内容の描画
	//-----------------
	CDrawCell::ResetDepthOfs();

	// 描画対象をフレームバッファへ
	SetRenderTarget( s_eFrameBufferCur, true );

	// ゲーム描画
	CAppMgr::DrawGame( eDRAW_PROCESS_MAIN );
    
	//-----------------------
	// ノード（ループ管理のUI）
	//-----------------------
    CLoopMgr::DrawNode();

	//-----------------------
	// ログ
	//-----------------------
    if( s_bLogOn ){
        if( s_bLogAll ){
            CMemMgr::DrawLog();
            CLoopMgr::DrawLog();
            CAppMgr::DrawLog();
            DrawLog();
        }

        DrawRenderLog();
    }

	//-----------------------
	// キャッシュの後始末
	//-----------------------
	if( s_bReserveCache ){
		// オフスクリーンの指定であれば、
		if( s_bOffScreenCache ){
			// 今回描画した内容をキャッシュし、画面には前回の内容を表示させる
			eFRAME_BUFFER temp = s_eFrameBufferCached;
			s_eFrameBufferCached = s_eFrameBufferCur;
			s_eFrameBufferCur = temp;
		}

		// フラグクリア
		s_bReserveCache = false;
		s_bOffScreenCache = false;
	}

	//-------------------------------
	// フレームバッファの内容を画面へ
	//-------------------------------
	SetRenderTarget( eFRAME_BUFFER_INVALID, true );

	// 表示
    CDrawCell* pDC = CDrawCell::GetFreeCell();
	pDC->clear();
	pDC->setFrameBuffer( s_eFrameBufferCur );
    
	// 特殊：モノクロ
	if( s_nAlphaForDrawMono > 0 ){
		float monoRate = s_nAlphaForDrawMono/255.0f;
		pDC->setMonoRate( monoRate );
	}
	// 特殊：ネガポジ
	else if( s_nAlphaForDrawNega > 0 ){
		float negaRate = s_nAlphaForDrawNega/255.0f;
		pDC->setNegaRate( negaRate );
	}
	// 特殊：セピア
	else if( s_nAlphaForDrawSepia > 0 ){
		float sepiaRate = s_nAlphaForDrawSepia/255.0f;
		pDC->setSepiaRate( sepiaRate );
	}

    float scaleForDisplay = s_fDisplayNativeScale/s_nDisplayDevicePixelRate;
    pDC->setScaleXY( scaleForDisplay, scaleForDisplay );

    // [OpenGL]との処理の兼ね合いで描画処理の際に上下反転しているためY座標を調整
    float ofsY = s_nFrameBufferH*(1.0f-scaleForDisplay);
    pDC->drawFB_I( 0.0f, ofsY );
    
    //-------------------------------------------------
    // スクリーンショット（フレームバッファの最終状況をBMPで出力）
    //-------------------------------------------------
    if( s_bSSDump ){
        // スクリーンショット用のバッファへフレームバッファの内容を出力
        SetRenderTarget( eFRAME_BUFFER_SCREEN_SHOT );
        
        // ディスプレイへ描画したセルをそのまま流用（※出力サイズを合わせるだけ）
        pDC->setWidth( s_nScreenShotW );
        pDC->setHeight( s_nScreenShotH );
        pDC->drawFB_I( 0, 0 );

        // 出力名が無効ならデフォルトに設定
        if( s_cArrDumpFileName[0] == '\0' ){
            sprintf( s_cArrDumpFileName, "ss%04d.bmp", s_nSSCount );
            s_nSSCount++;
        }
        
        // 出力
        DumpScreenShotToLocal( s_cArrDumpFileName );
        
        // 後始末
        s_cArrDumpFileName[0]= '\0';
        s_bSSDump = false;
    }
}

//---------------------
// 描画先の変更
//---------------------
void CMainRenderer::SetRenderTarget( eFRAME_BUFFER fbId, bool isClear, int r, int g, int b, int a ){
	// 対象が有効であれば
	if( fbId >= 0 ){
		CFrameBuffer* pFB = CFrameBufferMgr::GetFrameBuffer( fbId );
		glBindRenderbuffer( GL_RENDERBUFFER, pFB->getGlRenderBuffer() );
		glBindFramebuffer( GL_FRAMEBUFFER, pFB->getGlFrameBuffer() );
		glClearColor( pFB->getR()/255.0f, pFB->getG()/255.0f, pFB->getB()/255.0f, pFB->getA()/255.0f );

		// 描画サイズ設定
		s_nCurFrameBufferW = pFB->getWidth();
		s_nCurFrameBufferH = pFB->getHeight();
	}
	// フレームバッファへ
	else{
#ifdef EXE_IOS
		// [iOS] ビューに関連付けられたフレームバッファを指定
		glBindRenderbuffer( GL_RENDERBUFFER, g_stEnvNative.nRenderbuffer );
		glBindFramebuffer( GL_FRAMEBUFFER, g_stEnvNative.nFramebuffer );
#endif

#ifdef EXE_ANDROID
		// [Android] 無効値（※画面）を指定
		glBindRenderbuffer( GL_RENDERBUFFER, 0 );
		glBindFramebuffer( GL_FRAMEBUFFER, 0 );
#endif
		glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );

		// スクリーンサイズ設定
		s_nCurFrameBufferW = s_nFrameBufferW;
		s_nCurFrameBufferH = s_nFrameBufferH;
	}

	// ビューの設定
	glViewport( 0, 0, s_nCurFrameBufferW, s_nCurFrameBufferH );

	// シェーダーにレンダー領域サイズの通知
	CShader::SetCurRenderAreaSize( s_nCurFrameBufferW, s_nCurFrameBufferH );

    //-------------------------------------------------------------------------------------
	// レンダーバッファが切り替わったのでシェーダーキャッシュクリア（※バインドし直し）
    //-------------------------------------------------------------------------------------
	// TODO:調べる
	// [TexCache]関連で[CShader::ClearCache]を呼ばないで処理した場合下記の流れで不具合発生
	//
	//  step.1 [ObjA]の[TexCache]への描画(FB_WORKが対象) ⇒ キャッシュ[TexP]が設定される
	//  step.2 [ObjA]の[TexCache]を使った描画(FBが対象)が範囲外スキップ ⇒ キャッシュ情報はそのまま
	//  step.3 [ObjB]の[TexCache]への描画(FB_WORKが対象) ⇒ たまたまパレットが同じ（キャッシュヒット）
	// このケースでは、[ObjB]の[TexCache]描画結果が不正となってしまう（何も表示されない＝透明？）
	//
	// 理由として、ゲーム側が[step.1]で設定したパレットのバインドが、継続していると期待している一方で、
	// GL内部ではレンダーバッファ切り替えにより、テクスチャのバインドが外されてしまったためと考えても良いのか？
	//-------------------------------------------------------------------------------------
	CShader::ClearCache();

	// シザー＆デプステストオフ
	SetScissorOff();
	SetDepthTestOff();

	// 指定があればバッファクリア
	if( isClear ){
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	}
}

//---------------------
// ブレンド：ＯＦＦ
//---------------------
void CMainRenderer::SetBlendOff( void ){
	glDisable( GL_BLEND );
	glBlendFunc( GL_ONE, GL_ZERO );
}

//----------------------------------------------------------------
// ブレンド：半透明
// メモ：半透明は結構おそいので不要なアルファブレンディングや
// 　　　アルファテスティングの使用は必要最小限に留めること
//----------------------------------------------------------------
void CMainRenderer::SetBlendAlpha( void ){
	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
}

void CMainRenderer::SetBlendAlphaOne( void ){
	glEnable( GL_BLEND );
	glBlendFunc( GL_ONE, GL_ONE_MINUS_SRC_ALPHA );
}

//---------------------
// ブレンド：加算
//---------------------
void CMainRenderer::SetBlendAdd( void ){
	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE );
}

//---------------------
// ブレンド：乗算
//---------------------
void CMainRenderer::SetBlendMulti( void ){
	glEnable( GL_BLEND );
	glBlendFunc( GL_ZERO, GL_SRC_COLOR );
}

//-----------------------------------
// デプステスト（※カリングとは排他）
//-----------------------------------
void CMainRenderer::SetDepthTestOn( void ){
	glEnable( GL_DEPTH_TEST );
	glDisable( GL_CULL_FACE );
}

void CMainRenderer::SetDepthTestOff( void ){
	glDisable( GL_DEPTH_TEST );
	glEnable( GL_CULL_FACE );
}

//---------------------
// シザー設定：ＯＮ
//---------------------
bool CMainRenderer::SetScissor( float x, float y, float w, float h, bool isGame, bool isSafeArea ){
	// 指定があれば調整
    if( isGame ){
        if( isSafeArea ){
            x += SA_OFS_X;
            y += SA_OFS_Y;
        }
        
        x *= GM_P_RATE;
        y *= GM_P_RATE;
        w *= GM_P_RATE;
        h *= GM_P_RATE;
    }

    //------------------------
	// 調整
    //------------------------
    // 横幅が負であれば、Xのマイナス方向へ伸ばす
	if( w < 0 ){
		x += w;
		w *= -1;
	}

    // 縦幅が負であれば、Yのマイナス方向へ伸ばす
	if( h < 0 ){
		y += h;
		h *= -1;
	}

	// 画面外調整：左（※余分な領域はカット）
	if( x < 0.0f ){
		w += x;
		x = 0.0f;
	}

	// 画面外調整：右（※余分な領域はカット）
	if( (x+w) > CFB_W ){
		w -= (x+w) - CFB_W;
	}

	// 画面外調整：上（※余分な領域はカット）
	if( y < 0.0f ){
		h += y;
		y = 0.0f;
	}

	// 画面外調整：下（※余分な領域はカット）
	if( (y+h) > CFB_H ){
		h -= (y+h) - CFB_H;
	}

	// 画面外／範囲が無効なら無視
	if( x >= CFB_W ){ return( false ); }
	if( w <= 0.0f ){ return( false ); }
	if( y >= CFB_H ){ return( false ); }
	if( h <= 0.0f ){ return( false ); }

    //-----------------------------------
	// ここまで来たらシザーを有効に
    //-----------------------------------
	glEnable( GL_SCISSOR_TEST );

	// ビューの値を加味する（※Yは下が基準）
	glScissor( (int)x, (int)(CFB_H-(y+h)), (int)w, (int)h );

	// シェーダーにレンダー領域変更の通知
	CShader::SetCurRenderArea( x, y, w, h );

	return( true );
}

//---------------------
// シザー設定：ＯＦＦ
//---------------------
void CMainRenderer::SetScissorOff( void ){
	glDisable( GL_SCISSOR_TEST );

	// シェーダーにレンダー領域変更の通知
	CShader::SetCurRenderArea( 0, 0, s_nCurFrameBufferW, s_nCurFrameBufferH );
}

//-------------------------------
// スクリーンショット予約
//-------------------------------
void CMainRenderer::ReserveScreenShot( const char* pFileName ){
	s_bSSDump = true;

	// ファイル名のコピー（※バッファに収まらない場合は切り捨て）
    if( pFileName ){
        util_str_cpy( s_cArrDumpFileName, pFileName, COMMON_NAME_LEN );
    }else{
        s_cArrDumpFileName[0] = '\0';
    }
}

//-------------------------------------------------------------
// スクリーンショット保存
//（※呼び出し前にスクリーンショット用のフレームバッファが設定されている想定）
//-------------------------------------------------------------
bool CMainRenderer::DumpScreenShotToLocal( const char* pFileName ){
	// 出力領域確保
	BYTE* pBuf = (BYTE*) CMemMgr::GetAddrFromShare( eMEM_FIELD_SHARE_SS_OUT );
	uint32 sizeBufOut = CMemMgr::GetSizeFromShare( eMEM_FIELD_SHARE_SS_OUT );

    // BMPサイズ ＝ [ヘッダサイズ ＋ 画素(RGB)サイズ]
    uint32 sizeBmp = (uint32)(54 + 3*s_nScreenShotW*s_nScreenShotH);
    sizeBmp = 4*((sizeBmp+3)/4);        // ４バイトアライメント

    // サイズ確認
    LOGD( "@ SIZE FOR OUTPUT: %d/%d\n", sizeBmp, sizeBufOut );
	if( sizeBmp > sizeBufOut ){
        LOGE( "@ CMainRenderer::DumpScreenShotToLocal: BUF SHORTAGE:%s: %d > %d\n", pFileName, sizeBmp, sizeBufOut );
		return( false );
	}

	// 出力バッファ（※リトルエンディアン）
	COutputBuffer ob;
	ob.setBuffer( pBuf, sizeBufOut );
	ob.setForLittleEndian();

	// ファイルヘッダ: 14byte
	ob.writeInt8( 0x42 );		        // 1:B
	ob.writeInt8( 0x4D );		        // 1:M
	ob.writeInt32( sizeBmp );	        // 4:ファイルサイズ
	ob.writeInt16( 0 );			        // 2:予約
	ob.writeInt16( 0 );			        // 2:予約
	ob.writeInt32( 54 );		        // 4:データオフセット

	// 情報ヘッダ： 40byte
	ob.writeInt32( 40 );			    // 4:ヘッダサイズ
	ob.writeInt32( s_nScreenShotW );	// 4:横幅
	ob.writeInt32( s_nScreenShotH );	// 4:縦幅
	ob.writeInt16( 1 );				    // 2:プレーン数(1)
	ob.writeInt16( 24 );			    // 2:画素サイズ(RGB=24)
	ob.writeInt32( 0 );				    // 4:圧縮形式(0)
	ob.writeInt32( 0 );				    // 4:画像データ部のサイズ(0)
	ob.writeInt32( 0 );				    // 4:横方向の解像度(0)
	ob.writeInt32( 0 );				    // 4:縦方向の解像度(0)
	ob.writeInt32( 0 );				    // 4:パレット数(0)
	ob.writeInt32( 0 );				    // 4:パレットインデックス(0)

	//---------------------------------------
	// 画素設定
	//---------------------------------------
	// フレームバッファ読み込み領域確保
	BYTE* pRGBA = (BYTE*) CMemMgr::GetAddrFromShare( eMEM_FIELD_SHARE_SS_WORK );
	uint32 sizeBufWork = CMemMgr::GetSizeFromShare( eMEM_FIELD_SHARE_SS_WORK );
    
    // リソースサイズ（※フレームバッファ[RGBA]内容サイズ）
    uint32 sizeRsc = 4*s_nScreenShotW*s_nScreenShotH;

    // サイズ確認
    LOGD( "@ SIZE FOR WORK: %d/%d\n", sizeRsc, sizeBufWork );
	if( sizeRsc > sizeBufWork ){
        LOGE( "@ CMainRenderer::DumpScreenShotToLocal: RGBA BUF SHORTAGE:%s: %d > %d\n", pFileName, sizeRsc, sizeBufWork );
		return( false );
	}

    // フレームバッファの内容をバッファに読み込む
	glReadPixels( 0, 0, s_nScreenShotW, s_nScreenShotH, GL_RGBA, GL_UNSIGNED_BYTE, pRGBA );

	// [RGB]を抽出
	BYTE* pRGB = &pBuf[54];
	int at=0, atRGBA;
	for( int i=0; i<s_nScreenShotH; i++ ){
		for( int j=0; j<s_nScreenShotW; j++ ){
			atRGBA = 4*(s_nScreenShotW*i+j);
			pRGB[at++] = pRGBA[atRGBA+2];
			pRGB[at++] = pRGBA[atRGBA+1];
			pRGB[at++] = pRGBA[atRGBA+0];
		}
	}

	//----------------------------
	// 出力
	//----------------------------
	if( CFileMgr::SaveLocalDataToCaches( pFileName, pBuf, sizeBmp ) ){
		LOGD( "@ CMainRenderer::DumpScreenShotToLocal: %s\n", pFileName );
		return( true );
	}

    LOGE( "@ CMainRenderer::DumpScreenShotToLocal: FAILED TO SAVE: %s\n", pFileName );
    return( false );
}

#ifdef RENDER_LOG
//-------------------
// [DEBUG] ログの表示
//-------------------
void CMainRenderer::DrawRenderLog( void ){
	char* buf = CMemMgr::GetBufFromTempStr();

	int frame = g_stEnv.nFrame;
	float rate = g_stEnv.nFrameRate/10.0f;	// 百分率に

	float avgFrame = g_stEnv.stTimeLogFrame.avg/1000.0f;
	float avgUpdate = g_stEnv.stTimeLogUpdate.avg/1000.0f;
	float avgDraw = g_stEnv.stTimeLogDraw.avg/1000.0f;

	float neckFrame = g_stEnv.stTimeLogFrame.neck/1000.0f;
	float neckUpdate = g_stEnv.stTimeLogUpdate.neck/1000.0f;
	float neckDraw = g_stEnv.stTimeLogDraw.neck/1000.0f;

	//------------------------------
	// 左上：処理速度情報
	//------------------------------
	CDrawCell::SetLogAlignLT( 0.5f, 0, 0 );

	// フレーム情報
	sprintf( buf, "FRM: %d %.1f%%" , frame, rate );
	CDrawCell::DrawLogAlign( buf );

	// 処理時間平均
	sprintf( buf, "AVG: F:%.1f U:%.1f D:%.1f", avgFrame, avgUpdate, avgDraw );
	CDrawCell::DrawLogAlign( buf );

	// 処理時間ネック
	sprintf( buf, "NCK: F:%.1f U:%.1f D:%.1f", neckFrame, neckUpdate, neckDraw );
	CDrawCell::DrawLogAlign( buf );
    
    // TIME
    sprintf( buf, "TIM: %.2f", CAppMgr::GetTimePastSec() );
    CDrawCell::DrawLogAlign( buf );
    
    // UPDATE MODE
    if( CUpdater::IsUpdateByTime() ){
        sprintf( buf, "UPD: BY TIME x %.1f", CUpdater::GetUpdateSpeed() );
        CDrawCell::DrawLogAlign( buf, 0xFFC0C0FF );
    }else{
        sprintf( buf, "UPD: BY FRAME x %.1f", CUpdater::GetUpdateSpeed() );
        CDrawCell::DrawLogAlign( buf, 0xC0C0FFFF );
    }

	//------------------------------
	// 左上：CPU＆クロック情報
	//------------------------------
	CDrawCell::SetLogAlignLT( 0.5f, 96, 0 );

	// cpu & endian
	sprintf( buf, "cpu: %s(%s)", CAppMgr::GetCpuArchitecture(), (util_endian_is_big()? "BE": "LE") );
    CDrawCell::DrawLogAlign( buf );

	// clock()値
	sprintf( buf, "clck: %lld", (int64)clock() );
	CDrawCell::DrawLogAlign( buf );

	// util_sys_time_usec()値
	sprintf( buf, "usec: %lld", util_sys_time_usec() );
	CDrawCell::DrawLogAlign( buf );
}
#endif  // RENDER_LOG

//-------------------
// [DEBUG] ログの表示
//-------------------
void CMainRenderer::DrawLog( void ){
    char* buf = CMemMgr::GetBufFromTempStr();
    
    //------------------------------
    // 右上：描画関連管理情報
    //------------------------------
    CDrawCell::SetLogAlignRT( 0.5f, 0, 0 );
    
    // SS 情報
    sprintf( buf, "SCREEN SHOT: %d", s_nSSCount );
    CDrawCell::DrawLogAlign( buf );
    
    // FRAME BUFFER
    sprintf( buf, "FB CUR: %d CACHE: %d", s_eFrameBufferCur, s_eFrameBufferCached );
    CDrawCell::DrawLogAlign( buf );
    
    // TEX CACHE
    sprintf( buf, "TEX CACHE: %d/%d", CTexCacheMgr::GetTexCacheNumUse(), CTexCacheMgr::GetTexCacheNumMax() );
    CDrawCell::DrawLogAlign( buf );
    
    // TEX STRING CACHE
    sprintf( buf, "STRING CACHE: %d/%d", CTexStringMgr::GetTexStringNumUse(), CTexStringMgr::GetTexStringNumMax() );
    CDrawCell::DrawLogAlign( buf );
    
    // 余白
    CDrawCell::DrawLogAlign( "" );
    
    // ANCHOR POINT USE
    sprintf( buf, "ANCHOR POINT: %d/%d", CAnchorPointData::GetAllocDataNumUse(), CAnchorPointData::GetAllocDataNumMax() );
    CDrawCell::DrawLogAlign( buf );
    
    // LINE OBJECT USE
    sprintf( buf, "LINE OBJ: %d/%d", CLineObjectData::GetAllocDataNumUse(), CLineObjectData::GetAllocDataNumMax() );
    CDrawCell::DrawLogAlign( buf );
    
    // FILL POINT USE
    sprintf( buf, "FILL POINT: %d/%d", CFillPointData::GetAllocDataNumUse(), CFillPointData::GetAllocDataNumMax() );
    CDrawCell::DrawLogAlign( buf );
    
    // PAINT OBJECT USE
    sprintf( buf, "PAINT OBJ: %d/%d", CPaintObjectData::GetAllocDataNumUse(), CPaintObjectData::GetAllocDataNumMax() );
    CDrawCell::DrawLogAlign( buf );
    
    // SLOT POINT USE
    sprintf( buf, "SLOT POINT: %d/%d", CSlotPointData::GetAllocDataNumUse(), CSlotPointData::GetAllocDataNumMax() );
    CDrawCell::DrawLogAlign( buf );
    
    // SLOT OBJECT USE
    sprintf( buf, "SLOT OBJ: %d/%d", CSlotObjectData::GetAllocDataNumUse(), CSlotObjectData::GetAllocDataNumMax() );
    CDrawCell::DrawLogAlign( buf );
    
    // LAYER USE
    sprintf( buf, "LAYER: %d/%d", CLayerData::GetAllocDataNumUse(), CLayerData::GetAllocDataNumMax() );
    CDrawCell::DrawLogAlign( buf );

    // BMP GEN SETTING
    sprintf( buf, "BMP GEN: %dx%d(%dx%d:%d)", CBmpGenerator::GetBaseTexSizeW(), CBmpGenerator::GetBaseTexSizeH(),
             CBmpGenerator::GetTotalTexSizeW(), CBmpGenerator::GetTotalTexSizeH(), CBmpGenerator::GetTexResolution() );
    CDrawCell::DrawLogAlign( buf );
    
    
    //------------------------------
    // 右上：解像度関連情報
    //------------------------------
    CDrawCell::SetLogAlignRT( 0.5f, 0, 96 );
    
    // DEVICE INFO
    sprintf( buf, "DEV PIX RAT : %d", s_nDisplayDevicePixelRate );
    CDrawCell::DrawLogAlign( buf, (s_nDisplayDevicePixelRate > 1)? 0xFF8080FF: 0xFFFFFFFF );
    
    sprintf( buf, "DEV NTV SCL : %.3f", s_fDisplayNativeScale );
    CDrawCell::DrawLogAlign( buf, (s_fDisplayNativeScale!=s_nDisplayDevicePixelRate)? 0xFF8080FF: 0xFFFFFFFF );
    
    sprintf( buf, "GM PIX RAT : %d", s_nGamePixelRate );
    CDrawCell::DrawLogAlign( buf, (s_nGamePixelRate>REQUIRED_P_RATE)? 0xFF8080FF: 0xFFFFFFFF );

    sprintf( buf, "REQ WxH: %dx%d", s_nRequiredW, s_nRequiredH );
    CDrawCell::DrawLogAlign( buf, (s_nRequiredW==STEP_W_FOR_P_RATE&&s_nRequiredH==STEP_H_FOR_P_RATE)? 0xFF8080FF: 0xFFFFFFFF );
    
    sprintf( buf, "GM WxH : %dx%d", s_nGameW, s_nGameH );
    CDrawCell::DrawLogAlign( buf );
    
    sprintf( buf, "SA T-B : %d-%d", s_nGameSafeAreaMarginT, s_nGameSafeAreaMarginB );
    CDrawCell::DrawLogAlign( buf, (s_nGameSafeAreaMarginT>0||s_nGameSafeAreaMarginB>0)? 0xFF8080FF: 0xFFFFFFFF );
    
    sprintf( buf, "SA L-R : %d-%d", s_nGameSafeAreaMarginL, s_nGameSafeAreaMarginR );
    CDrawCell::DrawLogAlign( buf, (s_nGameSafeAreaMarginL>0||s_nGameSafeAreaMarginR>0)? 0xFF8080FF: 0xFFFFFFFF );
    
    sprintf( buf, "SA WxH : %dx%d", s_nGameSafeAreaW, s_nGameSafeAreaH );
    CDrawCell::DrawLogAlign( buf );

    sprintf( buf, "FB WxH : %dx%d", s_nFrameBufferW, s_nFrameBufferH );
    CDrawCell::DrawLogAlign( buf );

    sprintf( buf, "SS WxH : %dx%d", s_nScreenShotW, s_nScreenShotH );
    CDrawCell::DrawLogAlign( buf );
}
