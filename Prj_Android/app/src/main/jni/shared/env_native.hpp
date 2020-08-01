/*+----------------------------------------------------------------+
  |	Title:		env_native.hpp [共通環境]
  |	Comment:	プラットフォーム依存環境
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#ifndef __ENV_NATIVE_HPP__
#define __ENV_NATIVE_HPP__

/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#ifdef EXE_ANDROID
// [Android] 専用ヘッダの読み込み
#include "native/env_native_android.hpp"
#endif

#ifdef EXE_IOS
// [iOS] 専用ヘッダの読み込み
#include "native/env_native_ios.hpp"
#endif

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Struct		構造体型宣言
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
// [stENV_NATIVE]の宣言（※定義＆実装は各環境で行われる）
extern stENV_NATIVE	g_stEnvNative;

/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
//--------------------------------
// 環境の初期化／解放
//--------------------------------
#ifdef EXE_ANDROID
// [Android] アプリケーション構造体を受け取る
extern bool env_native_init( struct android_app* pApp );
#endif
#ifdef EXE_IOS
// [iOS] ビューとスクリーン領域情報を受け取る
extern bool env_native_init( MainView* pView, MainViewController* pVC, CGRect bounds, float scale, float nativeScale );
#endif
extern void env_native_exit( void );

//--------------------------------
// 表示システムの初期化／終了
//--------------------------------
extern bool env_native_init_display( void );
extern void env_native_exit_display( void );

//--------------------------------
// 表示開始／終了
//--------------------------------
extern bool env_native_render_start( void );
extern void env_native_render_end( void );

//--------------------------------
// ループのサスペンド/リジューム/終了
//--------------------------------
extern void env_native_suspend_loop( void );
extern void env_native_resume_loop( void );

//--------------------------------
// Native処理の呼び出し
//--------------------------------
// アプリ終了
extern void env_native_exitApp( bool isForce );

// スクリーン設定
extern void env_native_setKeepScreen( bool flag );

// ブラウザ起動
extern void env_native_openSite( const char* pUrl );

// バッテリー残量取得
extern int env_native_getBatteryRest( void );

// 端末のcpu種別を取得
extern const char* env_native_getCpuArchitecture( void );

// バージョンを取得
extern const char* env_native_getVersion( void );

// ビルドバージョンを取得
extern const char* env_native_getBuildVersion( void );

//------------------------
// 文字列テクスチャ作成
//------------------------
typedef struct{
    int origWidth, origHeight;
    int texWidth, texHeight;
    int left, top, right, bottom;
} stINFO_FOR_STRING_TEXTURE;
extern BYTE* env_native_createStringTexture( const char* pText, int type, int size, stINFO_FOR_STRING_TEXTURE* pInfo );

//-----------------------
// [DIALOG]：ダイアログ関連
// -----------------------
// 呼び出し
extern void env_native_callDialog( const char* pTitle, const char* pMess, const char* pPositive, const char* pNeutral, const char* pNegative );

// 呼び出し（文字入力）
extern void env_native_callDialogInputString( const char* pTitle, const char* pMess, const char* pPositive, const char* pNeutral, const char* pNegative, const char* pDefault );

// 状況確認
extern bool env_native_isDialogBusy( void );

// 結果取得
extern bool env_native_isDialogClickedPositive( void );
extern bool env_native_isDialogClickedNeutral( void );
extern bool env_native_isDialogClickedNegative( void );

// 入力文字取得
extern void env_native_getDialogInput( char* pBuf, int sizeBuf );

#endif	/* __ENV_NATIVE_HPP__ */
