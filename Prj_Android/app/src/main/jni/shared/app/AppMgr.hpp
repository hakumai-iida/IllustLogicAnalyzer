/*+----------------------------------------------------------------+
  |	Title:		AppMgr.hpp [共通環境]
  |	Comment:	アプリ管理
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#ifndef __APP_MGR_HPP__
#define __APP_MGR_HPP__

/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"

#include "sys/rand/Rand.hpp"
#include "app/bg/BgMgr.hpp"
#include "obj/Fade.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Struct		構造体定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Class		クラス定義
  +----------------------------------------------------------------+*/
//----------------------------------------
// アプリ管理インターフェイス
//----------------------------------------
class CAppMgr{
private:
	// 時間関連
	static int64 s_nEnvTimePastUs;	// 経過時間(※マイクロ秒)
	static float s_fEnvTimePast;	// 経過時間
	static float s_fEnvTimeDelta;	// 時間差分

	// フェード
	static CFade* s_pFade;

	// 停止フラグ：開発用
	static bool s_bPauseUpdate;

public:
	// 制御
	static bool OnAppStart( void );
	static void OnAppEnd( void );
	static void OnAppSuspend( void );
	static void OnAppResume( void );

    // 時間取得
    inline static int64 GetTimePastUs( void ){ return( s_nEnvTimePastUs ); }
    inline static float GetTimePastSec( void ){ return( s_fEnvTimePast ); }
    inline static float GetTimeDeltaSec( void ){ return( s_fEnvTimeDelta ); }
    inline static float GetTimePastFrame( void ){ return( s_fEnvTimePast * FPS ); }
    inline static float GetTimeDeltaFrame( void ){ return( s_fEnvTimeDelta * FPS ); }

	// デバッグ関連
	static void SetDebug( bool flag );
	static void SetDebugLog( bool flag, bool isAll=true );

	// タッチ演出指定
	static void SetTouchEffect( bool isDraw, bool isSe );

	// 時間リセット
	static void ResetTime( void );

	// バッファ切り替え（※ボス戦前の表示要素をボス戦後に引き継ぐための処理）
	static void SwitchMgrForBoss( bool isBoss, bool isClear );

	// ゲーム更新
	static void UpdateGame( int64 deltaUs );

	// ゲーム描画
	static void DrawGame( eDRAW_PROCESS process );

	// 開発関連
	static void DrawLog( void );		    // ログ描画
	static void CheckDebugInput( void );	// デバッグ入力確認

	// フェード指定
	inline static void SetFadeRGBA( int rgba ){ s_pFade->setRGBA( rgba ); }
	inline static void SetFadeIn( int frame=-1 ){ s_pFade->setFadeIn( frame ); }
	inline static void SetFadeOut( int frame=-1 ){ s_pFade->setFadeOut( frame ); }
	inline static bool IsFadeActive( void ){ return( s_pFade->isFadeActive() ); }

	//----------------------------------------
	// アプリ制御関連
	//----------------------------------------
	// アプリ終了
	static void Finish( void );
	static void ExitForce( void );

	// スクリーン設定（※消灯しない）
	static void SetKeepScreen( bool flag );

	// サイト呼び出し
	static void OpenSite( const char* pUrl );

	// バッテリー残量取得
	static int GetBatteryRest( void );

	// cpu種別の取得
	static const char* GetCpuArchitecture( void );

    // バージョンを取得
    static const char* GetVersion( void );

    // ビルドバージョンを取得
    static const char* GetBuildVersion( void );

	// 文字列テクスチャ作成
	static BYTE* CreateStringTexture( const char* pText, int type, int size, stINFO_FOR_STRING_TEXTURE* pInfo );

private:
	static void UpdateTime( int64 deltaUs );

	// インスタンス生成は不可
	CAppMgr( void ){};
	virtual ~CAppMgr( void ){};
};

/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
#endif	// __APP_MGR_HPP__
