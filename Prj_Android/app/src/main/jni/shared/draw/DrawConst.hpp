/*+----------------------------------------------------------------+
  |	Title:		DrawConst.hpp [共通環境]
  |	Comment:	描画定数
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#ifndef __DRAW_CONST_HPP__
#define __DRAW_CONST_HPP__

/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
//--------------------
// 描画タイプ
//--------------------
enum eDRAW_TYPE{
	eDRAW_TYPE_INVALID = -1,

	eDRAW_TYPE_RECT,		// 矩形：setRect
	eDRAW_TYPE_TEX,		    // テクスチャ：setTex
	eDRAW_TYPE_SPR,		    // スプライト：setSpr
	eDRAW_TYPE_STR,		    // 文字列：setStr
	eDRAW_TYPE_NUM,		    // 数字：setNum
	eDRAW_TYPE_POLY,		// ポリゴン：setPoly

	eDRAW_TYPE_MAX
};

#define IS_DRAW_TYPE_VALID( _t )    ((_t)>=(eDRAW_TYPE)0 && (_t)<eDRAW_TYPE_MAX)

//--------------------
// シェーダータイプ
//--------------------
enum eDRAW_SHADER{
	eDRAW_SHADER_INVALID = -1,

	eDRAW_SHADER_NORMAL,	// 通常（パレット／フルカラー判別）
	eDRAW_SHADER_RGBA,		// フルカラー
	eDRAW_SHADER_ALPHA,		// α

	eDRAW_SHADER_MAX
};

#define IS_DRAW_SHADER_VALID( _s )    ((_s)>=(eDRAW_SHADER)0 && (_s)<eDRAW_SHADER_MAX)

//--------------------
// ブレンドタイプ
//--------------------
enum eDRAW_BLEND{
	eDRAW_BLEND_INVALID = -1,

	eDRAW_BLEND_OFF,		// 無し（※不透明）
	eDRAW_BLEND_ALPHA,		// α ブレンド
	eDRAW_BLEND_ALPHA_ONE,	// α ブレンド（※ソースのαは１）
	eDRAW_BLEND_ADD,		// 加算
	eDRAW_BLEND_MULTI,		// 乗算

	eDRAW_BLEND_MAX
};

#define IS_DRAW_BLEND_VALID( _b )    ((_b)>=(eDRAW_BLEND)0 && (_b)<eDRAW_BLEND_MAX)

//--------------------
// 位置揃え：Ｘ
//--------------------
enum eDRAW_ALIGN_X{
    eDRAW_ALIGN_X_INVALID = -1,
    
	eDRAW_ALIGN_X_L,	// 左寄せ
	eDRAW_ALIGN_X_C,	// 中寄せ
	eDRAW_ALIGN_X_R,	// 右寄せ
    
    eDRAW_ALIGN_X_MAX
};

#define IS_DRAW_ALIGN_X_VALID( _aX )    ((_aX)>=(eDRAW_ALIGN_X)0 && (_aX)<eDRAW_ALIGN_X_MAX)

//--------------------
// 位置揃え：Ｙ
//--------------------
enum eDRAW_ALIGN_Y{
    eDRAW_ALIGN_Y_INVALID = -1,
    
	eDRAW_ALIGN_Y_T,	// 上揃え
	eDRAW_ALIGN_Y_M,	// 中揃え
	eDRAW_ALIGN_Y_B,	// 下揃え
    
    eDRAW_ALIGN_Y_MAX
};

#define IS_DRAW_ALIGN_Y_VALID( _aY )    ((_aY)>=(eDRAW_ALIGN_Y)0 && (_aY)<eDRAW_ALIGN_Y_MAX)

//----------------------------------------------
// 描画設定タイプ（※フォント／ナンバー描画指定時に利用）
//----------------------------------------------
enum eDRAW_SET_TYPE{
    eDRAW_SET_TYPE_INVALID = -1,
    
	eDRAW_SET_TYPE_P8,			// パレット
	eDRAW_SET_TYPE_P8_ALPHA,	// パレット（α付き）
	eDRAW_SET_TYPE_ALPHA,		// α
	eDRAW_SET_TYPE_RGBA,		// RGBA
    
    eDRAW_SET_TYPE_MAX
};

#define IS_DRAW_SET_TYPE_VALID( _t )    ((_t)>=(eDRAW_SET_TYPE)0 && (_t)<eDRAW_SET_TYPE_MAX)

//-------------------------------
// レイアウト関連（※[CLoop]に関連）
//-------------------------------
// 各種座標
#define LAYOUT_MARGIN_X     (4)
#define LAYOUT_MARGIN_Y     (4)
#define LAYOUT_X_L			(SA_XL+LAYOUT_MARGIN_X)
#define LAYOUT_X_C			(SA_XC)
#define LAYOUT_X_R			(SA_XR - LAYOUT_MARGIN_X)
#define LAYOUT_Y_T			(SA_YT+LAYOUT_MARGIN_Y)
#define LAYOUT_Y_M			(SA_YM)
#define LAYOUT_Y_B			(SA_YB - LAYOUT_MARGIN_Y)

// レイアウト方向
enum eLAYOUT_DIR{
    eLAYOUT_DIR_INVALID = -1,
    
	eLAYOUT_DIR_N,		// 無し
	eLAYOUT_DIR_F,		// 前方
	eLAYOUT_DIR_B,		// 後方
    
    eLAYOUT_DIR_MAX
};

#define IS_LAYOUT_DIR_VALID( _d )    ((_d)>=(eLAYOUT_DIR)0 && (_d)<eLAYOUT_DIR_MAX)

/*+----------------------------------------------------------------+
  |	Struct		構造体定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Class		クラス定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
extern const char* g_pArrLabelDrawType[];
extern const char* g_pArrLabelDrawShader[];
extern const char* g_pArrLabelDrawBlend[];
extern const char* g_pArrLabelDrawAlignX[];
extern const char* g_pArrLabelDrawAlignY[];
extern const char* g_pArrLabelDrawSetType[];
extern const char* g_pArrLabelLayoutDir[];
#endif	// __DRAW_CONST_HPP__
