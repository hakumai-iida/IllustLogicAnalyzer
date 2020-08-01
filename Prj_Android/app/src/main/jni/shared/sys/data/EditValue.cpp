/*+----------------------------------------------------------------+
  |	Title:		EditValue.hpp [共通環境]
  |	Comment:	値編集処理：[int32]で収まる範囲の数値のみに対応
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"
#include "EditValue.hpp"

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
//---------------------
// 値の取得
//---------------------
int CEditValue::GetValue( void* pVal, eEDIT_VALUE_TYPE type ){
	// 無効は無視
	if( pVal == NULL ){
		return( 0 );
	}

	int32 val = 0;
	switch( type ){
	case eEDIT_VALUE_TYPE_INT8:		val = *((int8*)pVal);				break;
	case eEDIT_VALUE_TYPE_UINT8:	val = *((uint8*)pVal);				break;
	case eEDIT_VALUE_TYPE_INT16:	val = *((int16*)pVal);				break;
	case eEDIT_VALUE_TYPE_UINT16:	val = *((uint16*)pVal);				break;
	case eEDIT_VALUE_TYPE_INT32:	val = *((int32*)pVal);				break;
	}

	return( val );
}

//---------------------
// 値の取得（ビット）
//---------------------
int32 CEditValue::GetValueForBit( void* pVal, eEDIT_VALUE_TYPE type, int bit ){
	int val = GetValue( pVal, type );

	if( (val&(0x1<<bit)) != 0 ){ return( 1 ); }
	return( 0 );
}

//---------------------
// 値の設定
//---------------------
bool CEditValue::SetValue( void* pVal, eEDIT_VALUE_TYPE type, int val ){
    // 無効は無視
    if( pVal == NULL ){
        return( false );
    }

	int32 val0 = GetValue( pVal, type );

	switch( type ){
	case eEDIT_VALUE_TYPE_INT8:		*((int8*)pVal) = (int8) val;		break;
	case eEDIT_VALUE_TYPE_UINT8:	*((uint8*)pVal) = (uint8) val;		break;
	case eEDIT_VALUE_TYPE_INT16:	*((int16*)pVal) = (int16) val;		break;
	case eEDIT_VALUE_TYPE_UINT16:	*((uint16*)pVal) = (uint16) val;	break;
	case eEDIT_VALUE_TYPE_INT32:	*((int32*)pVal) = (int32) val;		break;
	}

	return( val != val0 );
}

//---------------------
// 値の設定（ビット）
//---------------------
bool CEditValue::SetValueForBit( void* pVal, eEDIT_VALUE_TYPE type, int val, int bit ){
	int32 bits = GetValue( pVal, type );

	if( val != 0 ){ bits |= (0x01<<bit); }
	else{ bits &= ~(0x01<<bit); }

	return( SetValue( pVal, type, bits ) );
}

//---------------------
// 値のスワップ
//---------------------
void CEditValue::SwapValue( void* pVal, eEDIT_VALUE_TYPE type ){
	int val = GetValue( pVal, type );

	if( val == 0 ){ val = 1; }
	else{ val = 0; }

	SetValue( pVal, type, val );
}

//---------------------
// 値のスワップ（ビット）
//---------------------
void CEditValue::SwapValueForBit( void* pVal, eEDIT_VALUE_TYPE type, int bit ){
	int val = GetValueForBit( pVal, type, bit );

	if( val == 0 ){ val = 1; }
	else{ val = 0; }

	SetValueForBit( pVal, type, val, bit );
}
