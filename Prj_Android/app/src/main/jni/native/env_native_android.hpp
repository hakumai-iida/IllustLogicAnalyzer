/*+----------------------------------------------------------------+
  |	Title:		env_native_android.hpp [Android環境]
  |	Comment:	プラットフォーム依存環境
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#ifndef __ENV_NATIVE_ANDROID_HPP__
#define __ENV_NATIVE_ANDROID_HPP__

/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include <jni.h>
#include <errno.h>

#include <android/log.h>
#include <android_native_app_glue.h>
#include <android/input.h>
#include <android/sensor.h>

// for OpenGL
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

// for OpenSL
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
// ログ
#define TAG "AppEnv"
#define LOGD(...)	((void)__android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__))
#define LOGE(...)	((void)__android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__))

// [Android] java:処理の開始
#define START_JAVA_ACCESS()							\
	JavaVM* pVm = g_stEnvNative.pApp->activity->vm;		\
	jobject obj = g_stEnvNative.pApp->activity->clazz;	\
														\
	/* メインスレッドにアタッチ */						\
	JNIEnv* pEnvForJni;									\
	pVm->AttachCurrentThread( &pEnvForJni, NULL )

// [Android] java:処理の終了
#define END_JAVA_ACCESS()								\
	/* デタッチ */										\
	pVm->DetachCurrentThread()

// [Android] java:文字列確保
#define SAFE_ALLOC_JAVA_STR( _pStr )	(( (_pStr) != NULL && (_pStr)[0] != '\0' )? pEnvForJni->NewStringUTF( _pStr ): NULL)

// [Android] java:ローカルオブジェクト削除
#define SAFE_DELETE_JAVA_OBJ( _obj )	if( (_obj) != NULL ) pEnvForJni->DeleteLocalRef( _obj )

/*+----------------------------------------------------------------+
  |	Struct		構造体型宣言
  +----------------------------------------------------------------+*/
// 依存環境
typedef struct{
	// アプリケーション
	struct android_app* pApp;

	// フォーカスを保持しているか？
	bool bFocusGained;

	// 表示関連
	EGLDisplay eglDisplay;
	EGLContext eglContext;
	EGLSurface eglSurface;
	int nDisplayWidth;
	int nDisplayHeight;
} stENV_NATIVE;

/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/

#endif	/* __ENV_NATIVE_ANDROID_HPP__ */
