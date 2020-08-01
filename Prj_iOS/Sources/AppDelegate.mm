/*+----------------------------------------------------------------+
 |	Title:		AppDelegate.mm [iOS環境]
 |	Comment:	アプリケーションデリゲート（※アプリ制御）
 |	Author:		K.Takayanagi
 +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
 |	Include		インクルードヘッダ
 +----------------------------------------------------------------+*/
#include "env.hpp"
#include "AppDelegate.hpp"

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
static void uncaughtExceptionHandler( NSException *exception );

/*+----------------------------------------------------------------+
 |	Implementation		実装
 +----------------------------------------------------------------+*/
//----------------------
// 例外ログ
//----------------------
static void uncaughtExceptionHandler( NSException *exception ){
    NSLog( @"%@", exception.name );
    NSLog( @"%@", exception.reason );
    NSLog( @"%@", exception.callStackSymbols );
    
    // ログを[UserDefaults]に保存しておく
    // 次の起動の際に存在チェックすれば、前の起動時に異常終了したことを検知できる
    NSString *log = [NSString stringWithFormat: @"%@, %@, %@",
                     exception.name, exception.reason, exception.callStackSymbols];
    [[NSUserDefaults standardUserDefaults] setValue:log forKey: @"failLog"];
}

@implementation AppDelegate

//----------------------------------------------------
// アプリが起動された（※バックグラウンドにない状態からの起動）
//----------------------------------------------------
- (BOOL) application:( UIApplication*) application didFinishLaunchingWithOptions: (NSDictionary*) launchOptions{
    LOGD( "@ [AppDelegate application didFinishLaunchingWithOptions]\n" );

    //---------------------------
    // 情報出力
    //---------------------------
#ifdef __LP64__
    LOGD( "@ environment: [64] bit\n" );
#else
    LOGD( "@ environment: [32] bit\n" );
#endif
    LOGD( "@  sizeof(int)       = %d\n", (int)sizeof(int) );
    LOGD( "@  sizeof(long)      = %d\n", (int)sizeof(long) );
    LOGD( "@  sizeof(long long) = %d\n", (int)sizeof(long long) );
    LOGD( "@  sizeof(float)     = %d\n", (int)sizeof(float) );
    LOGD( "@  sizeof(double)    = %d\n", (int)sizeof(double) );
    LOGD( "@  sizeof(int8)      = %d\n", (int)sizeof(int8) );
    LOGD( "@  sizeof(uint8)     = %d\n", (int)sizeof(uint8) );
    LOGD( "@  sizeof(int16)     = %d\n", (int)sizeof(int16) );
    LOGD( "@  sizeof(uint16)    = %d\n", (int)sizeof(uint16) );
    LOGD( "@  sizeof(int32)     = %d\n", (int)sizeof(int32) );
    LOGD( "@  sizeof(uint32)    = %d\n", (int)sizeof(uint32) );
    LOGD( "@  sizeof(int64)     = %d\n", (int)sizeof(int64) );
    LOGD( "@  sizeof(uint64)    = %d\n", (int)sizeof(uint64) );
    LOGD( "@  address           = %p\n", launchOptions );
    
    //------------------------
    // 例外ハンドラ設定
    //------------------------
    NSSetUncaughtExceptionHandler( &uncaughtExceptionHandler );
    
    //-----------------------
    // 表示関連の初期化
    //-----------------------
    // スクリーンの領域の取得
    CGRect bounds = [[UIScreen mainScreen] bounds];

    // デバイスピクセル比の取得
    float scale = [[UIScreen mainScreen] scale];
    float nativeScale = scale;
    if ([UIScreen instancesRespondToSelector:@selector(nativeScale)]) {
        nativeScale = [[UIScreen mainScreen] nativeScale];
    }
    
    // ビューの初期化
    m_pView = [[MainView alloc] initWithFrame: bounds];

    // ビューコントローラの初期化
    m_pViewController = [[MainViewController alloc] init];
    [m_pViewController setView: m_pView];
    
    // 窓の初期化
    m_pWindow = [[UIWindow alloc] initWithFrame: bounds];
    [m_pWindow setRootViewController: m_pViewController];
    [m_pWindow makeKeyAndVisible];
    
    // 窓の領域を取得しなおし
    bounds = m_pView.bounds;
    LOGD( "@ bounds(window) = { %.1f, %.1f, %.1f, %.1f}\n", bounds.origin.x, bounds.origin.y, bounds.size.width, bounds.size.height );
    
    //----------------------
    // 環境初期化
    //----------------------
    if( !env_init() ){
        LOGE( "@ env_init(): FAILED\n" );
        return( NO );
    }
        
    // 依存環境初期化
    if( !env_native_init( m_pView, m_pViewController, bounds, scale, nativeScale ) ){
        LOGE( "@ env_native_init(): FAILED\n" );
        return( NO );
    }
   
    //---------------------------------------
    // ヘルパー登録（※面倒なので広域データに直接代入）
    //---------------------------------------
    
    // 初期化
    env_set_callback_flag( eECF_INIT_SYSTEM );
    env_set_callback_flag( eECF_INIT_DRAW );
    env_set_callback_flag( eECF_INIT_TOUCH );
    env_set_callback_flag( eECF_INIT_APP );

    // ゲームの開始
    env_set_callback_flag( eECF_PLAY_GAME );
    
    return( YES );
}

//----------------------------------------------------
// アプリが非アクティブになろうとしている
//----------------------------------------------------
// タスクメニュー時（ホームボタンがダブルタップされた等）は、
//   [applicationWillResignActive]
// のみ呼ばれる（※バックグラウンドへは移行していない）
//----------------------------------------------------
- (void) applicationWillResignActive: (UIApplication*) application{
    LOGD( "@ [AppDelegate applicationWillResignActive]\n" );
    
    // ゲームの中断
    env_set_callback_flag( eECF_SUSPEND_GAME );
    env_set_callback_flag( eECF_STOP_GAME );    
}

//----------------------------------------------------
// アプリがバックグラウンドへ移行した
//----------------------------------------------------
// アプリ中断時（ホームボタンが押された等）は、
//   [applicationWillResignActive]
//   [applicationDidEnterBackground]
// の順に呼ばれる
//----------------------------------------------------
- (void) applicationDidEnterBackground: (UIApplication*) application{
    LOGD( "@ [AppDelegate applicationDidEnterBackground]\n" );
}

//----------------------------------------------------
// アプリがバックグラウンドから復帰しようとしている
//----------------------------------------------------
- (void) applicationWillEnterForeground: (UIApplication*) application{
    LOGD( "@ [AppDelegate applicationWillEnterForeground]\n" );
}

//----------------------------------------------------
// アプリがアクティブになった
//----------------------------------------------------
// アプリ起動時は、
//   [application didFinishLaunchingWithOptions]
//   [applicationDidBecomeActive]
// の順に呼ばれる
//----------------------------------------------------
// アプリの再開時（バックグラウンドから）は、
//   [applicationWillEnterForeground]
//   [applicationDidBecomeActive]
// の順に呼ばれる
//----------------------------------------------------
// アプリの再開時（タスクメニューへ遷移しそのまま復帰）は、
//   [applicationDidBecomeActive]
// のみ呼ばれる（※バックグラウンドへは移行していない）
//----------------------------------------------------
- (void) applicationDidBecomeActive: (UIApplication*) application{
    LOGD( "@ [AppDelegate applicationDidBecomeActive]\n" );
        
    // 更新ループを起こす（※これをしないと下記コールバックフラグが処理されない）
    env_native_resume_loop();

    // ゲームの再開
    env_set_callback_flag( eECF_PLAY_GAME );
    env_set_callback_flag( eECF_RESUME_GAME );
}

//----------------------------------------------------
// アプリが終了しようとしている
//----------------------------------------------------
// 終了時は、
//   [applicationDidEnterBackground]
//   [applicationDidEnterBackground]（※２回呼ばれることがある？）
//   [applicationWillTerminate]
// の順に呼ばれる
//----------------------------------------------------
- (void) applicationWillTerminate: (UIApplication*) application{
    LOGD( "@ [AppDelegate applicationWillTerminate]\n" );

    // 終了
    env_set_callback_flag( eECF_EXIT_APP );
    env_set_callback_flag( eECF_EXIT_TOUCH );
    env_set_callback_flag( eECF_EXIT_DRAW );
    env_set_callback_flag( eECF_EXIT_SYSTEM );
}

@end
