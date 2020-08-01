/*+----------------------------------------------------------------+
  |	Title:		DialogHelper.cpp [共通環境]
  |	Comment:	ダイアログヘルパー
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "DialogHelper.hpp"

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
/*+----------------------------------------------------------------+
  |	Program		プログラム実装
  +----------------------------------------------------------------+*/
//--------------------------
// ダイアログ呼び出し
//--------------------------
void CDialogHelper::CallDialog( const char* pTitle, const char* pMess, const char* pPositive, const char* pNeutral, const char* pNegative ){
	env_native_callDialog( pTitle, pMess, pPositive, pNeutral, pNegative );
}

//--------------------------
// 入力ダイアログ呼び出し
//--------------------------
void CDialogHelper::CallDialogInputString( const char* pTitle, const char* pMess, const char* pPositive, const char* pNeutral, const char* pNegative, const char* pDefault ){
	env_native_callDialogInputString( pTitle, pMess, pPositive, pNeutral, pNegative, pDefault );
}

//--------------------------
// ダイアログ終了検出
//--------------------------
bool CDialogHelper::IsBusy( void ){
	return( env_native_isDialogBusy() );
}

//--------------------------
// ダイアログ入力取得
//--------------------------
bool CDialogHelper::IsSelectedPositive( void ){ return( env_native_isDialogClickedPositive() ); }
bool CDialogHelper::IsSelectedNeutral( void ){ return( env_native_isDialogClickedNeutral() ); }
bool CDialogHelper::IsSelectedNegative( void ){ return( env_native_isDialogClickedNegative() ); }

//--------------------------
// 入力文言取得
//--------------------------
void CDialogHelper::GetInputString( char* pBuf, int sizeBuf ){ env_native_getDialogInput( pBuf, sizeBuf ); }
