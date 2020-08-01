/*+----------------------------------------------------------------+
  |	Title:		env_native_ios.hpp [iOS環境]
  |	Comment:	プラットフォーム依存環境
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#ifndef __ENV_NATIVE_IOS_HPP__
#define __ENV_NATIVE_IOS_HPP__

/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include <UIKit/UIKit.h>
#include <GLKit/GLKit.h>
#include <GLKit/GLKView.h>
#include <QuartzCore/QuartzCore.h>

#include <OpenGLES/EAGL.h>
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>

#import <AudioToolbox/AudioToolbox.h>

#include <GameKit/GameKit.h>

#include "MainView.hpp"
#include "MainViewController.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
//--------------------------
// ログ
//--------------------------
#define LOGD(...)	printf( __VA_ARGS__ )
#define LOGE(...)   printf( __VA_ARGS__ )

/*+----------------------------------------------------------------+
  |	Struct		構造体型宣言
  +----------------------------------------------------------------+*/
//---------------
// 依存環境
//---------------
typedef struct{
    MainView* pView;                        // メインビュー
    MainViewController* pViewController;    // メインビューコントローラー
    
    uint32 nRenderbuffer;                   // レンダーフレームID
    uint32 nFramebuffer;                    // フレームバッファID
	int nDisplayWidth;                      // ディスプレイの横幅
	int nDisplayHeight;                     // ディスプレイの縦幅
    int nDisplayDevicePixelRate;            // ディスプレイ単位に対するピクセル割合
    float fDisplayNativeScale;              // 実際に画面に表示される際の表示割合
    float fSafeAreaMarginTop;               // セーフエリアのマージン:top
    float fSafeAreaMarginBottom;            // セーフエリアのマージン:bottom
    float fSafeAreaMarginLeft;              // セーフエリアのマージン:left
    float fSafeAreaMarginRight;             // セーフエリアのマージン:right
} stENV_NATIVE;

/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/

#endif	/* __ENV_NATIVE_IOS_HPP__ */
