/*+----------------------------------------------------------------+
  |	Title:		env_native_android.cpp [Android環境]
  |	Comment:	プラットフォーム依存環境
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include <android/window.h>

#include "env.hpp"
#include "./env_native_android.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Struct		構造体定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
// 管理データ
stENV_NATIVE g_stEnvNative;

/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Program		プログラム実装
  +----------------------------------------------------------------+*/
//------------------------
// [Android] 環境初期化
//------------------------
bool env_native_init( struct android_app* pApp ){
#ifdef MGR_INIT_LOG
	LOGD( "@ env_native_init( pApp=%08X )\n", (uint32)pApp );
#endif

	// 環境データクリア
	memset( &g_stEnvNative, 0, sizeof(stENV_NATIVE) );

	// アプリケーション割当
	g_stEnvNative.pApp = pApp;

	return( true );
}

//------------------------
// [Android] 環境終了
//------------------------
void env_native_exit( void ){
#ifdef MGR_INIT_LOG
	LOGD( "@ env_native_exit()\n" );
#endif
}

//--------------------------------------
// [Android] 表示システムの初期化
//--------------------------------------
bool env_native_init_display( void ){
#ifdef MGR_INIT_LOG
	LOGD( "@ env_native_init_display()\n" );
#endif

	EGLint w, h;
	EGLint format;
	EGLint numConfigs;
	EGLConfig config;
	EGLSurface surface;
	EGLContext context;

	// EGLパラメータ
	const EGLint attribs[] = {
			// OpenGL ES2の指定
			EGL_RENDERABLE_TYPE,	EGL_OPENGL_ES2_BIT,

			// サーフェイスタイプの指定
			EGL_SURFACE_TYPE,		EGL_WINDOW_BIT,

			// 画素値の指定
			EGL_ALPHA_SIZE,		8,
			EGL_RED_SIZE,			8,
			EGL_GREEN_SIZE,		8,
			EGL_BLUE_SIZE,		8,

			// デプス
			EGL_DEPTH_SIZE,		16,

			// 終了
			EGL_NONE
	};

	// EGLディスプレイコネクション取得
	EGLDisplay display = eglGetDisplay( EGL_DEFAULT_DISPLAY );

	// EGL ディスプレイコネクション初期化
	eglInitialize( display, 0, 0 );

	// 条件に合ったEGLフレームバッファ設定のリストを返す
	eglChooseConfig( display, attribs, &config, 1, &numConfigs );

	// EGLフレームバッファ設定の情報を取得
	eglGetConfigAttrib( display, config, EGL_NATIVE_VISUAL_ID, &format );

	// NativeActivityへバッファを設定
	ANativeWindow_setBuffersGeometry( g_stEnvNative.pApp->window, 0, 0, format );

    // EGLウィンドウサーフェイスの取得
    surface = eglCreateWindowSurface( display, config, g_stEnvNative.pApp->window, NULL );

    // EGLレンダリングコンテキストの取得
    const EGLint attrib_list[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE };
    context = eglCreateContext( display, config, NULL, attrib_list );

    // EGLレンダリングコンテキストをEGLサーフェイスにアタッチ
    if( eglMakeCurrent( display, surface, surface, context ) == EGL_FALSE ){
        LOGE( "@ env_native_init_display: FAILED [eglMakeCurrent]\n" );
        env_push_error_log( "[eglMakeCurrent()]に失敗しました\n" );
        return( false );    // FATAL ERROR
    }

    // 画面解像度取得
	eglQuerySurface( display, surface, EGL_WIDTH, &w );
	eglQuerySurface( display, surface, EGL_HEIGHT, &h );

	// EGL関連データの保存
	g_stEnvNative.eglDisplay = display;
	g_stEnvNative.eglContext = context;
	g_stEnvNative.eglSurface = surface;
	g_stEnvNative.nDisplayWidth = w;
	g_stEnvNative.nDisplayHeight = h;

#ifdef MGR_INIT_LOG
	LOGD( "@ nDisplayWidth  = %d\n", g_stEnvNative.nDisplayWidth );
	LOGD( "@ nDisplayHeight = %d\n", g_stEnvNative.nDisplayHeight );
#endif

	return( true );
}

//--------------------------------------
// [Android] 表示システムの終了
//--------------------------------------
void env_native_exit_display( void ){
#ifdef MGR_INIT_LOG
	LOGD( "@ env_native_exit_display()\n" );
#endif

	if( g_stEnvNative.eglDisplay != EGL_NO_DISPLAY ){
		// EGLレンダリングコンテキストとEGLサーフェイスの関連を外す
		eglMakeCurrent( g_stEnvNative.eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT );

		// EGLレンダリングコンテキストの破棄
		if( g_stEnvNative.eglContext != EGL_NO_CONTEXT ){
			eglDestroyContext( g_stEnvNative.eglDisplay, g_stEnvNative.eglContext );
		}

		// EGLサーフェイスの破棄
		if( g_stEnvNative.eglSurface != EGL_NO_SURFACE ){
			eglDestroySurface( g_stEnvNative.eglDisplay, g_stEnvNative.eglSurface );
		}

		// EGLディスプレイの破棄
		eglTerminate( g_stEnvNative.eglDisplay );
	}

	// 無効化
	g_stEnvNative.eglDisplay = EGL_NO_DISPLAY;
	g_stEnvNative.eglContext = EGL_NO_CONTEXT;
	g_stEnvNative.eglSurface = EGL_NO_SURFACE;
}

//--------------------------------
// [Android] レンダー開始
//--------------------------------
bool env_native_render_start( void ){
	//----------------------------------
	// 有効性確認
	//----------------------------------
	if( g_stEnvNative.pApp->window == NULL ){
		return( false );
	}

	if( g_stEnvNative.eglDisplay == EGL_NO_DISPLAY ||
		g_stEnvNative.eglContext == EGL_NO_CONTEXT ||
		g_stEnvNative.eglSurface == EGL_NO_SURFACE ){
		return( false );
	}

	return( true );
}

//--------------------------------
// [Android] レンダー終了
//--------------------------------
void env_native_render_end( void ){
	eglSwapBuffers( g_stEnvNative.eglDisplay, g_stEnvNative.eglSurface );
}

//--------------------------------
// [Android] ループのサスペンド
//--------------------------------
extern void env_native_suspend_loop( void ){
	// 特に何もしない
}

//--------------------------------
// [Android] ループのリジューム
//--------------------------------
extern void env_native_resume_loop( void ){
	// 特に何もしない
}

//--------------------------------------
// [Android] java:アプリ終了
//--------------------------------------
void env_native_exitApp( bool isForce ) {
	START_JAVA_ACCESS();

	// メソッド呼び出し
	jclass c = pEnvForJni->GetObjectClass(obj);

	jmethodID m;
	if (isForce) {
		m = pEnvForJni->GetMethodID(c, "exitForce", "()V");
	}else{
		m = pEnvForJni->GetMethodID(c, "finish", "()V");
	}
	pEnvForJni->CallVoidMethod( obj, m );

	END_JAVA_ACCESS();
}

//--------------------------------------
// [Android] java:スクリーン設定
//--------------------------------------
void env_native_setKeepScreen( bool flag ){
	ANativeActivity* pActivity = g_stEnvNative.pApp->activity;

	if( flag ){
		ANativeActivity_setWindowFlags( pActivity, AWINDOW_FLAG_KEEP_SCREEN_ON, 0 );
	}else{
		ANativeActivity_setWindowFlags( pActivity, 0, AWINDOW_FLAG_KEEP_SCREEN_ON );
	}
}

//--------------------------------------
// [Android] java:ブラウザの起動
//--------------------------------------
void env_native_openSite( const char* pUrl ){
	START_JAVA_ACCESS();

	// 文字列を生成
	jstring strUrl = SAFE_ALLOC_JAVA_STR( pUrl );

	// メソッド呼び出し
	jclass c = pEnvForJni->GetObjectClass( obj );
	jmethodID m = pEnvForJni->GetMethodID( c, "openSite", "(Ljava/lang/String;)V" );
	pEnvForJni->CallVoidMethod( obj, m, strUrl );

	// 文字列の解放
	SAFE_DELETE_JAVA_OBJ( strUrl );

	END_JAVA_ACCESS();
}

//--------------------------------------
// [Android] java:バッテリーの残量取得
//--------------------------------------
int env_native_getBatteryRest( void ){
	START_JAVA_ACCESS();

	// メソッド呼び出し
	jclass c = pEnvForJni->GetObjectClass( obj );
	jmethodID m = pEnvForJni->GetMethodID( c, "getBatteryRest", "()I" );
	int rest = pEnvForJni->CallIntMethod( obj, m );

	END_JAVA_ACCESS();

	return( rest );
}

//------------------------
// cpu種別を取得
//------------------------
const char* env_native_getCpuArchitecture( void ){
#if defined(__arm__)
	return( "arm" );
#endif

#if defined(__i386__)
	return( "i386" );
#endif

#if defined(__mips64)
	return( "mips64" );
#endif

#if defined(__mips__)
	return( "mips" );
#endif

#if defined(__aarch64__)
	return( "aarch64" );
#endif

#if defined(__x86_64__)
	return( "x86_64" );
#endif

	return( "unknown" );
}

//-----------------------------------
// [Android]:バージョンを取得
//-----------------------------------
const char* env_native_getVersion( void ){
	START_JAVA_ACCESS();

	// メソッド呼び出し
	jclass c = pEnvForJni->GetObjectClass( obj );
	jmethodID m = pEnvForJni->GetMethodID( c, "getVersionName", "()Ljava/lang/String;" );
	jstring str = (jstring) pEnvForJni->CallObjectMethod( obj, m );

	char* pBuf = CMemMgr::GetBufFromTempStr();
	int sizeBuf = (int)CMemMgr::GetSizeFromTempStr();

	if( str != NULL ){
		const char* cStr = pEnvForJni->GetStringUTFChars( str, NULL );

		// バッファにコピー（※収まらない場合はエラー）
		if( ! util_str_cpy( pBuf, cStr, sizeBuf ) ){
			LOGE( "@ env_native_getVersion: BUF SHORTAGE: %d >= %d, cStr=%s\n", util_str_len(cStr), sizeBuf, cStr );

#if 1
			// ここに来るのは問題
			sprintf( pBuf, "E.R.R.O.R" );
#endif
		}

		pEnvForJni->ReleaseStringUTFChars( str, cStr );
	}else{
		sprintf( pBuf, "?.?.?" );
	}

	END_JAVA_ACCESS();

	return( pBuf );
}

//---------------------------------
// [Android]:ビルドバージョンを取得
//---------------------------------
const char* env_native_getBuildVersion( void ){
	START_JAVA_ACCESS();

	// メソッド呼び出し
	jclass c = pEnvForJni->GetObjectClass( obj );
	jmethodID m = pEnvForJni->GetMethodID( c, "getVersionCode", "()I" );
	int code = pEnvForJni->CallIntMethod( obj, m );

	END_JAVA_ACCESS();

	char* buf = CMemMgr::GetBufFromTempStr();
	sprintf( buf, "%d", code );
	return( buf );
}

//----------------------------------------
// [Android] java:文字列テクスチャ作成
//----------------------------------------
BYTE* env_native_createStringTexture( const char* pText, int type, int size, stINFO_FOR_STRING_TEXTURE* pInfo ){
	// データを受け取るバッファ
	BYTE* pBuf = (BYTE*) CMemMgr::GetAddrFromShare( eMEM_FIELD_SHARE_SS_WORK );
	int sizeBuf = CMemMgr::GetSizeFromShare( eMEM_FIELD_SHARE_SS_WORK );

	//-----------------------------------------
	// アクセス開始
	START_JAVA_ACCESS();

	// バッファをバイトバッファに変換
	jobject bb = pEnvForJni->NewDirectByteBuffer( pBuf, sizeBuf );

	// 文字列を生成
	jstring strText = SAFE_ALLOC_JAVA_STR( pText );

	// メソッド呼び出し
	jclass c = pEnvForJni->GetObjectClass( obj );
	jmethodID m = pEnvForJni->GetMethodID( c, "createStringTexture", "(Ljava/lang/String;Ljava/nio/ByteBuffer;)V" );
	pEnvForJni->CallVoidMethod( obj, m, strText, bb );

	// 文字列の解放
	SAFE_DELETE_JAVA_OBJ( strText );

	END_JAVA_ACCESS();
	// アクセス終了
	//-----------------------------------------

	// バッファから画像の取り出し
	CMemMgr::PushTargetField( eMEM_FIELD_D_FREE );
	CInputBuffer *pIB = new CInputBuffer();
	CMemMgr::PopTargetField();

	// リトルエンディアンで処理
    pIB->setBuffer( pBuf, sizeBuf );
    pIB->setForLittleEndian();

	int w = pIB->readInt32();
	int h = pIB->readInt32();
	int texW = pIB->readInt32();
	int texH = pIB->readInt32();
	int l = pIB->readInt32();
	int t = pIB->readInt32();
	int r = pIB->readInt32();
	int b = pIB->readInt32();

	if( pInfo != NULL ){
		pInfo->origWidth  = w;
		pInfo->origHeight = h;
		pInfo->texWidth   = texW;
		pInfo->texHeight  = texH;
		pInfo->left       = l;
		pInfo->top        = t;
		pInfo->right      = r;
		pInfo->bottom     = b;
	}

	SAFE_DELETE( pIB );

	// パレット画像に変換
	pBuf = &pBuf[4*8];
	BYTE* pDot0 = (BYTE*) CMemMgr::GetAddrFromShare( eMEM_FIELD_SHARE_SS_OUT );
	BYTE* pDot = pDot0;
	for( int i=0; i<texH; i++ ){
		for( int j=0; j<texW; j++ ){
			*pDot = *pBuf;	// α部分だけ抜き出す

			pDot += 1;
			pBuf += 4;
		}
	}

	return( pDot0 );
}

//--------------------------------------
// [Android] java:ダイアログ呼び出し
//--------------------------------------
void env_native_callDialog( const char* pTitle, const char* pMess, const char* pPositive, const char* pNeutral, const char* pNegative ){
	START_JAVA_ACCESS();

	// 文字列を生成
	jstring strTitle = SAFE_ALLOC_JAVA_STR( pTitle );
	jstring strMess = SAFE_ALLOC_JAVA_STR( pMess );
	jstring strPositive = SAFE_ALLOC_JAVA_STR( pPositive );
	jstring strNeutral = SAFE_ALLOC_JAVA_STR( pNeutral );
	jstring strNegative = SAFE_ALLOC_JAVA_STR( pNegative );

	// メソッド呼び出し
	jclass c = pEnvForJni->GetObjectClass( obj );
	jmethodID m = pEnvForJni->GetMethodID( c, "callDialog", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V" );
	pEnvForJni->CallVoidMethod( obj, m, strTitle, strMess, strPositive, strNeutral, strNegative );

	// 文字列の解放
	SAFE_DELETE_JAVA_OBJ( strTitle );
	SAFE_DELETE_JAVA_OBJ( strMess );
	SAFE_DELETE_JAVA_OBJ( strPositive );
	SAFE_DELETE_JAVA_OBJ( strNeutral );
	SAFE_DELETE_JAVA_OBJ( strNegative );

	END_JAVA_ACCESS();
}

//-------------------------------------------------
// [Android] java:ダイアログ呼び出し（※文字列入力）
//-------------------------------------------------
void env_native_callDialogInputString( const char* pTitle, const char* pMess, const char* pPositive, const char* pNeutral, const char* pNegative, const char* pDefault ){
	START_JAVA_ACCESS();

	// 文字列を生成
	jstring strTitle = SAFE_ALLOC_JAVA_STR( pTitle );
	jstring strMess = SAFE_ALLOC_JAVA_STR( pMess );
	jstring strPositive = SAFE_ALLOC_JAVA_STR( pPositive );
	jstring strNeutral = SAFE_ALLOC_JAVA_STR( pNeutral );
	jstring strNegative = SAFE_ALLOC_JAVA_STR( pNegative );
	jstring strDefault = SAFE_ALLOC_JAVA_STR( pDefault );

	// メソッド呼び出し
	jclass c = pEnvForJni->GetObjectClass( obj );
	jmethodID m = pEnvForJni->GetMethodID( c, "callDialogInputString", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V" );
	pEnvForJni->CallVoidMethod( obj, m, strTitle, strMess, strPositive, strNeutral, strNegative, strDefault );

	// 文字列の解放
	SAFE_DELETE_JAVA_OBJ( strTitle );
	SAFE_DELETE_JAVA_OBJ( strMess );
	SAFE_DELETE_JAVA_OBJ( strPositive );
	SAFE_DELETE_JAVA_OBJ( strNeutral );
	SAFE_DELETE_JAVA_OBJ( strNegative );
	SAFE_DELETE_JAVA_OBJ( strDefault );

	END_JAVA_ACCESS();
}

//--------------------------------------
// [Android] java:ダイアログ終了検出
//--------------------------------------
bool env_native_isDialogBusy( void ){
	START_JAVA_ACCESS();

	// メソッド呼び出し
	jclass c = pEnvForJni->GetObjectClass( obj );
	jmethodID m = pEnvForJni->GetMethodID( c, "isDialogBusy", "()Z" );
	jboolean b = pEnvForJni->CallBooleanMethod( obj, m );

	END_JAVA_ACCESS();

	return( b );
}

//--------------------------------------
// [Android] java:ダイアログ入力取得
//--------------------------------------
// [pPositive]ボタンが押されたか？
bool env_native_isDialogClickedPositive( void ){
	START_JAVA_ACCESS();

	// メソッド呼び出し
	jclass c = pEnvForJni->GetObjectClass( obj );
	jmethodID m = pEnvForJni->GetMethodID( c, "isDialogClickedPositive", "()Z" );
	jboolean b = pEnvForJni->CallBooleanMethod( obj, m );

	END_JAVA_ACCESS();

	return( b );
}

// [pNeutral]ボタンが押されたか？
bool env_native_isDialogClickedNeutral( void ){
	START_JAVA_ACCESS();

	// メソッド呼び出し
	jclass c = pEnvForJni->GetObjectClass( obj );
	jmethodID m = pEnvForJni->GetMethodID( c, "isDialogClickedNeutral", "()Z" );
	jboolean b = pEnvForJni->CallBooleanMethod( obj, m );

	END_JAVA_ACCESS();

	return( b );
}

// [pNegative]ボタンが押されたか？
bool env_native_isDialogClickedNegative( void ){
	START_JAVA_ACCESS();

	// メソッド呼び出し
	jclass c = pEnvForJni->GetObjectClass( obj );
	jmethodID m = pEnvForJni->GetMethodID( c, "isDialogClickedNegative", "()Z" );
	jboolean b = pEnvForJni->CallBooleanMethod( obj, m );

	END_JAVA_ACCESS();

	return( b );
}

//--------------------------------------
// [Android] java:入力文字列取得
//--------------------------------------
void env_native_getDialogInput( char* pBuf, int sizeBuf ){
	START_JAVA_ACCESS();

	// メソッド呼び出し
	jclass c = pEnvForJni->GetObjectClass( obj );
	jmethodID m = pEnvForJni->GetMethodID( c, "getStrDialogInput", "()Ljava/lang/String;" );
	jstring str = (jstring) pEnvForJni->CallObjectMethod( obj, m );

	if( str != NULL ){
		const char* cStr = pEnvForJni->GetStringUTFChars( str, NULL );

		int len = strlen( cStr );
		if( len >= sizeBuf ){
			LOGE( "@ env_native_getStrDialogInput: BUF SHORTAGE: %d >= %d\n", len, sizeBuf );
			strncpy( pBuf, cStr, sizeBuf-1 );
			pBuf[sizeBuf-1] = '\0';
		}else{
			strcpy( pBuf, cStr );
		}

		pEnvForJni->ReleaseStringUTFChars( str, cStr );
	}else{
		pBuf[0] = '\0';
	}

	END_JAVA_ACCESS();
}
