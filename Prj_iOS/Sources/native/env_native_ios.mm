/*+----------------------------------------------------------------+
  |	Title:		env_native_ios.cpp [iOS環境]
  |	Comment:	プラットフォーム依存環境
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "env_native_ios.hpp"

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

// サスペンド判定用
static bool s_bNativeLoopActive = false;

/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Program		プログラム実装
  +----------------------------------------------------------------+*/
//------------------------
// [iOS]:環境初期化
//------------------------
bool env_native_init( MainView* pView, MainViewController* pVC, CGRect bounds, float scale, float nativeScale ){
#ifdef MGR_INIT_LOG
	LOGD( "@ env_native_init()\n" );
#endif

	// 環境データクリア
	memset( &g_stEnvNative, 0, sizeof(stENV_NATIVE) );
    
    // ビューへの参照
    g_stEnvNative.pView = pView;
    g_stEnvNative.pViewController = pVC;
    
    // 画面サイズ取得
    g_stEnvNative.nDisplayWidth = bounds.size.width;
    g_stEnvNative.nDisplayHeight = bounds.size.height;
    g_stEnvNative.nDisplayDevicePixelRate = (int)scale;
    g_stEnvNative.fDisplayNativeScale = nativeScale;
    
    // セーフエリアのマージン取得（※[iOS 11]以上)
    if( @available( iOS 11, * ) ){
        g_stEnvNative.fSafeAreaMarginTop = g_stEnvNative.pView.safeAreaInsets.top;
        g_stEnvNative.fSafeAreaMarginBottom = g_stEnvNative.pView.safeAreaInsets.bottom;
        g_stEnvNative.fSafeAreaMarginLeft = g_stEnvNative.pView.safeAreaInsets.left;
        g_stEnvNative.fSafeAreaMarginRight = g_stEnvNative.pView.safeAreaInsets.right;
    }

    // ログ
    LOGD( "@  nDisplayWidth  = %d\n", g_stEnvNative.nDisplayWidth );
    LOGD( "@  nDisplayHeight = %d\n", g_stEnvNative.nDisplayHeight );
    LOGD( "@  nDisplayDevicePixelRate = %d\n", g_stEnvNative.nDisplayDevicePixelRate );
    LOGD( "@  nDisplayNativeScale = %f\n", g_stEnvNative.fDisplayNativeScale );
    LOGD( "@  safeAreaMargin = top:%f bottom:%f left:%f right:%f\n",
         g_stEnvNative.fSafeAreaMarginTop, g_stEnvNative.fSafeAreaMarginBottom,
         g_stEnvNative.fSafeAreaMarginLeft, g_stEnvNative.fSafeAreaMarginRight );
    
	return( true );
}

//------------------------
// [iOS]:環境終了
//------------------------
void env_native_exit( void ){
#ifdef MGR_INIT_LOG
	LOGD( "@ env_native_exit()\n" );
#endif
}

//---------------------------
// [iOS]:表示システムの初期化
//---------------------------
bool env_native_init_display( void ){
#ifdef MGR_INIT_LOG
	LOGD( "@ env_native_init_display()\n" );
#endif

    // メインビューの初期化を呼ぶ
    MainView* pView = g_stEnvNative.pView;
    if( ![pView onSurfaceCreate] ){
        return( false );
    }
   
    // ここまできたら情報の取得
    g_stEnvNative.nRenderbuffer = pView.glRenderbuffer;
    g_stEnvNative.nFramebuffer = pView.glFramebuffer;

    // ログ
    LOGD( "@  nRenderbuffer  = %d\n", g_stEnvNative.nRenderbuffer );
    LOGD( "@  nFramebuffer = %d\n", g_stEnvNative.nFramebuffer );
    
    return( true );
}

//---------------------------
// [iOS]:表示システムの終了
//---------------------------
void env_native_exit_display( void ){
#ifdef MGR_INIT_LOG
	LOGD( "@ env_native_exit_display()\n" );
#endif
    
    MainView* pView = g_stEnvNative.pView;
    [pView onExit];
}

//---------------------------
// [iOS]:レンダー開始
//---------------------------
bool env_native_render_start( void ){
    if( g_stEnvNative.pView.glContext == nil ){
        return( false );
    }
	return( true );
}

//---------------------------
// [iOS]:レンダー終了
//---------------------------
void env_native_render_end( void ){
    MainView* pView = g_stEnvNative.pView;
    EAGLContext* pContext = pView.glContext;
    
    [pContext presentRenderbuffer: GL_RENDERBUFFER];
}

//--------------------------------
// [iOS]:ループのサスペンド
//--------------------------------
extern void env_native_suspend_loop( void ){
    if( ! s_bNativeLoopActive ){
        return;
    }
   
    MainView* pView = g_stEnvNative.pView;
    [pView onSuspend];
    
    
    s_bNativeLoopActive = false;
}

//--------------------------------
// [iOS]:ループのリジューム
//--------------------------------
void env_native_resume_loop( void ){
    if( s_bNativeLoopActive ){
        return;
    }
    
    MainView* pView = g_stEnvNative.pView;
    [pView onResume];
    
    s_bNativeLoopActive = true;
}

//---------------------------------------
// [iOS]:アプリ終了
//---------------------------------------
void env_native_exitApp( bool isForce ){
    if( isForce ){
        exit( 0 );
    }else{
        LOGD( "@ [iOS] env_native_exitApp() is IGNORED.\n" );
    }
}

//----------------------------------
// [iOS]:スクリーン設定（※消灯しない設定）
//----------------------------------
void env_native_setKeepScreen( bool flag ){
    UIApplication *pApp = [UIApplication sharedApplication];
    pApp.idleTimerDisabled = flag;
}

//---------------------------
// [iOS]:ブラウザの起動
//---------------------------
void env_native_openSite( const char* pUrl ){
    NSString* pStrUrl = [[NSString alloc] initWithUTF8String:pUrl];
    
    [[UIApplication sharedApplication] openURL: [NSURL URLWithString:pStrUrl] options:@{}
        completionHandler:^(BOOL success) {
            if( success ){
                LOGD( "@ openURL: SUCCESS\n" );
            }else{
                LOGD( "@ openURL: FAILED\n" );
            }
        }
    ];
}

//---------------------------
// [iOS]:バッテリーの残量取得
//---------------------------
int env_native_getBatteryRest( void ){
    UIDevice* pDevice = [UIDevice currentDevice];
    
    pDevice.batteryMonitoringEnabled = YES;
    
    return( (int)(100*pDevice.batteryLevel) );
}

//------------------------
// [iOS]:端末のcpu種別を取得
//------------------------
const char* env_native_getCpuArchitecture( void ){
    // とりあえず
    return( "arm" );
}

//---------------------------
// [iOS]:バージョンを取得
//---------------------------
const char* env_native_getVersion( void ){
    NSString *version = [[[NSBundle mainBundle] infoDictionary] objectForKey:@"CFBundleShortVersionString"];
    return( [version UTF8String] );
}

//----------------------------------
// [iOS]:ビルドバージョンを取得
//----------------------------------
const char* env_native_getBuildVersion( void ){
    NSString *build = [[[NSBundle mainBundle] infoDictionary] objectForKey:@"CFBundleVersion"];
    return( [build UTF8String] );
}

//---------------------------
// [iOS]:テクスチャフォント作成
//---------------------------
#define BASE_FONT_SIZE  32
BYTE* env_native_createStringTexture( const char* pText, int type, int size, stINFO_FOR_STRING_TEXTURE* pInfo ){
    // 作業バッファ取得
    BYTE* pBuf = (BYTE*)CMemMgr::GetAddrFromShare( eMEM_FIELD_SHARE_SDARC_IN );
    
    // テキスト設定
    NSString* text = [[NSString alloc] initWithUTF8String: pText];
    UIFont* font = [UIFont boldSystemFontOfSize:BASE_FONT_SIZE];
    NSMutableParagraphStyle* style = [[NSMutableParagraphStyle alloc] init];
    style.lineBreakMode = NSLineBreakByClipping;
    style.alignment = NSTextAlignmentLeft;
    
    NSDictionary* attributes = @{
                                 NSForegroundColorAttributeName: [UIColor whiteColor],
                                 NSFontAttributeName: font,
                                 NSParagraphStyleAttributeName: style
                                 };
    // 描画範囲の検出
    CGSize textSize = [text boundingRectWithSize: CGSizeMake( 1024, 512 )
                                         options:NSStringDrawingUsesLineFragmentOrigin
                                      attributes:@{NSFontAttributeName: font}
                                         context:nil].size;
    
    int w = (int)textSize.width;
    int h = (int)textSize.height;
    
    // クリア（※これをしないとゴミがでる）
    memset( pBuf, 0, w*h );
    
    // コンテキスト設定
    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceGray();
    CGContextRef context = CGBitmapContextCreate( pBuf, w, h, 8, w, colorSpace, kCGImageAlphaOnly );
    
    // 描画
    UIGraphicsPushContext( context );
    [text drawInRect:CGRectMake( 0, 0, w, h) withAttributes: attributes ];
    UIGraphicsPopContext();
    
    // コンテキストの解放
    CGContextRelease( context );
    CGColorSpaceRelease( colorSpace );
    
    // 出力バッファ設定
    int fixW = 4*((w+4-1)/4);
    int fixH = 4*((h+4-1)/4);
    BYTE* pBufRet = (BYTE*)CMemMgr::GetAddrFromShare( eMEM_FIELD_SHARE_SDARC_OUT );
    memset( pBufRet, 0, fixH*fixW );
    
    // 上下を反転して出力
    for( int i=0; i<h; i++ ){
        for( int j=0; j<w; j++ ){
            pBufRet[fixW*i+j] = pBuf[w*(h-1-i)+j];
        }
    }
    
    // 情報設定
    pInfo->origWidth  = w;
    pInfo->origHeight = h;
    pInfo->texWidth   = fixW;
    pInfo->texHeight  = fixH;
    pInfo->left       = 0;
    pInfo->top        = -BASE_FONT_SIZE;    // 文字の下が基準になるように
    pInfo->right      = textSize.width;
    pInfo->bottom     = h - BASE_FONT_SIZE;
    
    return( pBufRet );
}
