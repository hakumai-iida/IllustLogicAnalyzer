/*+----------------------------------------------------------------+
  |	Title:		env_native_ios_for_dialog.cpp [iOS環境]
  |	Comment:	プラットフォーム依存環境：DIALOG
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
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Program		プログラム実装
  +----------------------------------------------------------------+*/
//---------------------------
// [iOS/DIALOG]: 呼び出し
//---------------------------
void env_native_callDialog( const char* pTitle, const char* pMess, const char* pPositive, const char* pNeutral, const char* pNegative ){
    
    // 入力ダイアログを入力無効で呼び出す
    env_native_callDialogInputString( pTitle, pMess, pPositive, pNeutral, pNegative, NULL );
}

//---------------------------
// [iOS/DIALOG]: 呼び出し
//---------------------------
void env_native_callDialogInputString( const char* pTitle, const char* pMess, const char* pPositive, const char* pNeutral, const char* pNegative, const char* pDefault ){
    NSString* pStrTitle = nil;
    if( pTitle != NULL ){
        pStrTitle = [[NSString alloc] initWithUTF8String: pTitle];
    }
    
    NSString* pStrMess = nil;
    if( pMess != NULL  ){
        pStrMess = [[NSString alloc] initWithUTF8String: pMess];
    }
    
    NSString* pStrPositive = nil;
    if( pPositive != NULL ){
        pStrPositive = [[NSString alloc] initWithUTF8String: pPositive];
    }
    
    NSString* pStrNeutral = nil;
    if( pNeutral != NULL  ){
        pStrNeutral = [[NSString alloc] initWithUTF8String: pNeutral];
    }
    
    NSString* pStrNegative = nil;
    if( pNegative != NULL ){
        pStrNegative = [[NSString alloc ] initWithUTF8String: pNegative];
    }
    
    NSString* pStrDefault = nil;
    if( pDefault != NULL ){
        pStrDefault = [[NSString alloc ] initWithUTF8String: pDefault];
    }
    
    // ダイアログ呼び出し
    [g_stEnvNative.pView callDialog: pStrTitle withMess:pStrMess withPositiveButton:pStrPositive withNeutralButton: pStrNeutral withNegativeButton:pStrNegative withDefaultText: pStrDefault];
}

//-----------------------------
// [iOS/DIALOG]:ダイアログ終了検出
//-----------------------------
bool env_native_isDialogBusy( void ){
    return( g_stEnvNative.pView.dialogActive );
}

//-----------------------------
// [iOS/DIALOG]:ダイアログ入力取得
//-----------------------------
// [pPositive]ボタンが押されたか？
bool env_native_isDialogClickedPositive( void ){
    return( g_stEnvNative.pView.dialogSelectedIndex == 0 );
}

// [pNeutral]ボタンが押されたか？
bool env_native_isDialogClickedNeutral( void ){
    return( g_stEnvNative.pView.dialogSelectedIndex == 1 );
}

// [pNegative]ボタンが押されたか？
bool env_native_isDialogClickedNegative( void ){
    return( g_stEnvNative.pView.dialogSelectedIndex == 2 );
}

// 入力文字列の取得
void env_native_getDialogInput( char* pBuf, int sizeBuf ){
    if( g_stEnvNative.pView.dialogSelectedIndex == 0 ){
        const char *pStr = [g_stEnvNative.pView.dialogInput UTF8String];
        if( pStr != NULL ){
            util_str_cpy( pBuf, pStr, sizeBuf );
        }
    }
}
