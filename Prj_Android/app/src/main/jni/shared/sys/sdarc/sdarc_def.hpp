/*+----------------------------------------------------------------+
  |	Title:		sdarc_def.hpp [共通環境]
  |	Comment:	simple data archive定義関連
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#ifndef	__SDARC_DEF_HPP__
#define	__SDARC_DEF_HPP__

/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
// 長さデータのオフセット（※データを浮かせるため）
#define PTN_LENGTH_OFS                  2

// パターン検出１（※4096文字までさかのぼり、17文字までパターン化）
#define PTN1_MAX_TARGET                 4096
#define PTN1_MAX_LENGTH                 15
#define PTN1_SHIFT_LENGTH	            12
#define PTN1_MASK_LENGTH                0x0F
#define PTN1_MASK_TARGET                0xFFF
#define PTN1_PACK( _length, _target )   ((((_length) - PTN_LENGTH_OFS) << PTN1_SHIFT_LENGTH) + (_target))
#define PTN1_GET_LENGTH( _dat )         (((_dat)>>PTN1_SHIFT_LENGTH) & PTN1_MASK_LENGTH)
#define PTN1_GET_TARGET( _dat )         ((_dat)&PTN1_MASK_TARGET)

/*+----------------------------------------------------------------+
  |	Struct		構造体型宣言
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/

#endif	// __SDARC_DEF_HPP__
