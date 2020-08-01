/*+----------------------------------------------------------------+
  |	Title:		MainRenderer.hpp [GLES2.0環境]
  |	Comment:	レンダラー
  |             指定は環境に依存しないゲーム座標で行う
  |             描画は[CDrawCell]経由で行い、[CShader]のレンダー時に
  |             フレームバッファ座標に変換される
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#ifndef __MAIN_RENDERER_HPP__
#define __MAIN_RENDERER_HPP__

/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"

#include "draw/shader/ShaderMgr.hpp"
#include "draw/frame_buffer/FrameBufferMgr.hpp"
#include "draw/img/ImgMgr.hpp"
#include "draw/spr/SprMgr.hpp"
#include "draw/font/FontMgr.hpp"
#include "draw/num/NumMgr.hpp"
#include "draw/grp/GrpMgr.hpp"
#include "draw/DrawCell.hpp"
#include "draw/DrawCommon.hpp"
#include "draw/tex/bezier/Bezier.hpp"
#include "draw/tex/stroke/StrokeMgr.hpp"
#include "draw/tex/fill/FillMgr.hpp"
#include "draw/tex/bmp/BmpGenerator.hpp"
#include "draw/tex/TexCacheMgr.hpp"
#include "draw/tex/TexStringMgr.hpp"
#include "draw/tex/TexMgr.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
//-----------------------------------------------------------------
// 各種サイズ
//-----------------------------------------------------------------
// 要求サイズ（※比率２倍の[3:2]以上を必須解像度とする）
#define REQUIRED_P_RATE         2
#define REQUIRED_W_FOR_P_RATE   480
#define REQUIRED_H_FOR_P_RATE   320

// ２倍を超える場合の判定値（※拡大判定は[16:9]の比率で行う）
#define STEP_W_FOR_P_RATE       640
#define STEP_H_FOR_P_RATE       360

// 各種値（※簡便のため）
#define GM_P_RATE   CMainRenderer::GetGamePixelRate()
#define REQUIRED_W  CMainRenderer::GetRequiredW()
#define REQUIRED_H  CMainRenderer::GetRequiredH()
#define GA_W        CMainRenderer::GetGameW()
#define GA_H        CMainRenderer::GetGameH()
#define SA_W        CMainRenderer::GetGameSafeAreaW()
#define SA_H        CMainRenderer::GetGameSafeAreaH()
#define SA_OFS_X    CMainRenderer::GetGameSafeAreaMarginL()
#define SA_OFS_Y    CMainRenderer::GetGameSafeAreaMarginT()
#define FB_W        CMainRenderer::GetFrameBufferW()
#define FB_H        CMainRenderer::GetFrameBufferH()
#define SS_W        CMainRenderer::GetScreenShotW()
#define SS_H        CMainRenderer::GetScreenShotH()
#define CFB_W       CMainRenderer::GetCurFrameBufferW()
#define CFB_H       CMainRenderer::GetCurFrameBufferH()

// セーフエリアに対する描画オフセット（※中央揃えの[+0.5f,+0.5f]調整は[CDrawCell:draw0]内部で行われる）
#define SA_XL       0.0f
#define SA_YT       0.0f
#define SA_XC       (SA_W/2.0f)
#define SA_YM       (SA_H/2.0f)
#define SA_XR       SA_W
#define SA_YB       SA_H

/*+----------------------------------------------------------------+
  |	Struct		構造体定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Class		クラス定義
  +----------------------------------------------------------------+*/
//----------------------------------------
// レンダラー
//----------------------------------------
class CMainRenderer{
private:
	// デバイス関連
    static int   s_nDisplayDevicePixelRate;             // デバイスサイズに対して適用するピクセル率（※[iOS]で有効／その他では[1]固定）
    static float s_fDisplayNativeScale;		            // ディスプレイに表示する際に適用するスケール（※[iOS]で有効／その他では[1.0f]固定）
    
    // ゲーム座標関連
    static int   s_nGamePixelRate;                      // ゲーム座標 → フレームバッファ座標への変換率
    static int   s_nRequiredW, s_nRequiredH;            // 要件サイズ
    static int   s_nGameW, s_nGameH;                    // ゲーム座標系サイズ
    static int   s_nGameSafeAreaMarginT;                // セーフエリア：上（※[iOS]で有効／その他では[0]固定）
    static int   s_nGameSafeAreaMarginB;                // セーフエリア：下（※[iOS]で有効／その他では[0]固定）
    static int   s_nGameSafeAreaMarginL;                // セーフエリア：左（※[iOS]で有効／その他では[0]固定）
    static int   s_nGameSafeAreaMarginR;                // セーフエリア：右（※[iOS]で有効／その他では[0]固定）
    static int   s_nGameSafeAreaW, s_nGameSafeAreaH;    // セーフエリアサイズ

    // フレームバッファ関連
    static int   s_nFrameBufferW, s_nFrameBufferH;      // フレームバッファサイズ（※画面出力用の描画バッファ）
    static int   s_nScreenShotW, s_nScreenShotH;        // スクリーンショット出力サイズ（※フレームバッファが大きすぎると困るので別サイズで確保）

    // 描画対象
    static eFRAME_BUFFER s_eFrameBufferCur;		        // 描画対象
    static eFRAME_BUFFER s_eFrameBufferCached;	        // キャッシュ（描画内容をテクスチャとして扱いたい場合）
    static int s_nCurFrameBufferW, s_nCurFrameBufferH;  // 設定中のフレームバッファサイズ

    // フラグ
    static bool s_bLogOn;               // ログ指定
    static bool s_bLogAll;              // 全ログ指定
    static bool s_bReserveCache;        // 描画内容をキャッシュするか？
    static bool s_bOffScreenCache;      // オフスクリーン（表示しない）でキャッシュするか？

    // 特殊
    static BYTE s_nAlphaForDrawMono;    // モノクロ表示割合
    static BYTE s_nAlphaForDrawNega;    // ネガポジ表示割合
    static BYTE s_nAlphaForDrawSepia;   // セピア表示割合

    // スクリーンショット関連
    static bool s_bSSDump;
    static int  s_nSSCount;
    static char s_cArrDumpFileName[COMMON_NAME_LEN];

public:
    // アプリ起動後に真っ先によばれる処理（※メモリ確保よりも前に解像度等を確定させる）
    static bool OnDisplayChange( int devW, int devH );

    // 初期化／終了
    static bool OnCreate( void );
    static void OnDestroy( void );
    
    // 中断／復帰
    static void OnSuspend( void );
    static void OnResume( void );

      // 表示
    static void OnDrawFrame( void );

    inline static bool IsLogOn( void ){ return( s_bLogOn ); }
    inline static void SetLogOn( bool flag ){ s_bLogOn = flag; }
    inline static void SwapLogOn( void ){ s_bLogOn = !s_bLogOn; }

    inline static bool IsLogAll( void ){ return( s_bLogAll ); }
    inline static void SetLogAll( bool flag ){ s_bLogAll = flag; }
    inline static void SwapLogAll( void ){ s_bLogAll = !s_bLogAll; }

    inline static void SetAlphaForDrawMono( BYTE a ){ s_nAlphaForDrawMono = a; }
    inline static void SetAlphaForDrawNega( BYTE a ){ s_nAlphaForDrawNega = a; }
    inline static void SetAlphaForDrawSepia( BYTE a ){ s_nAlphaForDrawSepia = a; }

    inline static void ResetAlphaForDraw( void ){
    	s_nAlphaForDrawMono = 0;
    	s_nAlphaForDrawNega = 0;
    	s_nAlphaForDrawSepia = 0;
    }

    // レンダーターゲット設定
    static void SetRenderTarget( eFRAME_BUFFER fbId, bool isClear=false, int r=0, int g=0, int b=0, int a=0 );
    inline static void ResetRenderTarget( bool isClear=false, int r=0, int g=0, int b=0, int a=0 ){
    	SetRenderTarget( s_eFrameBufferCur, isClear, r, g, b, a );
    }

    // 直接描画キャッシュ（※現在のフレームバッファをテクスチャとして扱えるように）
    static eFRAME_BUFFER CacheCurFrameBuffer( void );
    static eFRAME_BUFFER GetTexCachedFB( void ){ return( s_eFrameBufferCached ); }

    // 描画キャッシュ（※直前の出力フレームバッファをテクスチャとして扱えるように）
    static void ReserveFrameBufferCache( bool isOffScreen ){ s_bReserveCache = true; s_bOffScreenCache = isOffScreen; }

    // ブレンド設定
    static void SetBlendOff( void );
    static void SetBlendAlpha( void );
    static void SetBlendAlphaOne( void );
    static void SetBlendAdd( void );
    static void SetBlendMulti( void );

    // デプス設定
    static void SetDepthTestOn( void );
    static void SetDepthTestOff( void );

    // シザリング設定（※返値は指定が有効か＝描画領域があるか）
    static bool SetScissor( float x, float y, float w, float h, bool isGame=true, bool isSafeArea=true );
    static void SetScissorOff( void );
    
    // 座標関連
    inline static int GetDisplayDevicePixelRate( void ){ return( s_nDisplayDevicePixelRate ); }
    inline static float GetDisplayNativeScale( void ){ return( s_fDisplayNativeScale ); }
    
    inline static int GetGamePixelRate ( void ){ return( s_nGamePixelRate ); }
    inline static int GetRequiredW( void ){ return( s_nRequiredW ); }
    inline static int GetRequiredH( void ){ return( s_nRequiredH ); }
    inline static int GetGameW( void ){ return( s_nGameW ); }
    inline static int GetGameH( void ){ return( s_nGameH ); }
    inline static int GetGameSafeAreaMarginT( void ){ return( s_nGameSafeAreaMarginT ); }
    inline static int GetGameSafeAreaMarginB( void ){ return( s_nGameSafeAreaMarginB ); }
    inline static int GetGameSafeAreaMarginL( void ){ return( s_nGameSafeAreaMarginL ); }
    inline static int GetGameSafeAreaMarginR( void ){ return( s_nGameSafeAreaMarginR ); }
    inline static int GetGameSafeAreaW( void ){ return( s_nGameSafeAreaW ); }
    inline static int GetGameSafeAreaH( void ){ return( s_nGameSafeAreaH ); }

    inline static int GetFrameBufferW( void ){ return( s_nFrameBufferW ); }
    inline static int GetFrameBufferH( void ){ return( s_nFrameBufferH ); }

    inline static int GetScreenShotW( void ){ return( s_nScreenShotW ); }
    inline static int GetScreenShotH( void ){ return( s_nScreenShotH ); }

    inline static int GetCurFrameBufferW( void ){ return( s_nCurFrameBufferW ); }
    inline static int GetCurFrameBufferH( void ){ return( s_nCurFrameBufferH ); }

    // スクリーンショット予約（※予約の発行されたフレームの描画終了時の内容が出力される）
    static void ReserveScreenShot( const char* pFileName = NULL );

private:
	// インスタンス作成は不可
	CMainRenderer( void ){}
	virtual ~CMainRenderer( void ){}

    // スクリーンショット出力
    static bool DumpScreenShotToLocal( const char* pFileName );

    // レンダーログ
#ifdef RENDER_LOG
    static void DrawRenderLog( void );
#else
    inline static void DrawRenderLog( void ){}
#endif  // RENDER_LOG

    // ログ
    static void DrawLog( void );
};

/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
#endif	// __MAIN_RENDERER_HPP__
