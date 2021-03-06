/*+----------------------------------------------------------------+
  |	Title:		FillConst.hpp [共通環境]
  |	Comment:	塗り定数
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#ifndef __FILL_CONST_HPP__
#define __FILL_CONST_HPP__

/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
 +----------------------------------------------------------------+*/
//------------------------------------
// 塗りタイプ
//------------------------------------
enum eFILL_TYPE{
    eFILL_TYPE_INVALID = -1,    // 無効値

    eFILL_TYPE_OFF,             // オフ（※色を出力しない＝テスト用に下塗りだけしたい場合）
    eFILL_TYPE_MONO,            // 単色塗り（※色テスト付き）
    
    eFILL_TYPE_MAX,
};

//-----------------------------------------------------------
// 塗りオプション（※特殊な処理をしたい場合オプションとして指定する）
//-----------------------------------------------------------
enum eFILL_OPTION{
    eFILL_OPTION_INVALID = -1,      // 無効値

    //--------------------------------------------------------------------
    // 明暗塗り（※タッチの指定がある場合は境界線上をテスト指定のパレットでアンチする）
    //（※明暗の度合いを最小／最大で塗りつぶす＆テスト指定が適用される）
    //--------------------------------------------------------------------
    eFILL_OPTION_BRIGHT,            // 領域を明色で塗りつぶす
    eFILL_OPTION_DARK,              // 領域を暗色で塗りつぶ
    
    //--------------------------------------------------------------------
    // ポイント（※指定の座標に対し１ストロークの明暗補正を行う）
    //（※明暗の度合いを最小／最大まで変化させる＆テスト指定が適用される）
    //--------------------------------------------------------------------
    eFILL_OPTION_POINT_BRIGHT,      // １ドットの明色ストロークを出力する
    eFILL_OPTION_POINT_DARK,        // １ドットの暗色ストロークを出力する
    
    //--------------------------------------------------------------------
    // 領域補正（※塗りガイドで指定される領域に対して色の補正を行う）
    //（※明暗の度合いを一段階変化させる＆テスト指定が適用される）
    //--------------------------------------------------------------------
    eFILL_OPTION_AREA_BRIGHT,       // 領域にパレット値を減算（明るく）する
    eFILL_OPTION_AREA_DARK,         // 領域にパレット値を加算（暗く）する

    //--------------------------------------------------------------------
    // 全画素補正（※塗りバッファの各画素を相対位置で比較し、その差分で色の補正を行う）
    //（※明暗の度合いを一段階変化させる＆テスト指定は適用されない）
    //--------------------------------------------------------------------
    eFILL_OPTION_COLOR_BRIGHT,      // 全画素を明るく補正する
    eFILL_OPTION_COLOR_DARK,        // 全画素を暗く補正する

    //--------------------------------------------------------------------
    // 補正予約（※直前の下塗り領域を流用して、画素が未出力の領域に色の補正予約を行う）
    //（※明暗の度合いを一段階変化させる＆テストパレット指定は適用されない）
    //（※この処理との兼ね合いにで塗りバッファのクリア色が0ではない点に注意）
    //--------------------------------------------------------------------
    eFILL_OPTION_RESERVE_BRIGHT,    // 明るさ予約
    eFILL_OPTION_RESERVE_DARK,      // 暗さ予約
    
    //--------------------------------------------------------------------
    // マスク
    //--------------------------------------------------------------------
    eFILL_OPTION_MASK,              // マスク塗り

    eFILL_OPTION_MAX,
};

//-----------------------------------------------------------------------------
// [scanLineForUndercoat]から[fillUndercoat]へ渡すヒント（※無駄な検索を省くための情報）
//-----------------------------------------------------------------------------
// [scanLineForUndercoat]の検索時に、左側が閉じていた
#define FILL_UNDERCOAT_SCAN_HINT_CLOSE_LEFT     0x01

// [scanLineForUndercoat]の検索時に、右側が閉じていた
#define FILL_UNDERCOAT_SCAN_HINT_CLOSE_RIGHT    0x02

// [scanLineForUndercoat]には、下から上に移動してきた
#define FILL_UNDERCOAT_SCAN_HINT_MOVE_UP        0x10

// [scanLineForUndercoat]には、上から下に移動してきた
#define FILL_UNDERCOAT_SCAN_HINT_MOVE_DOWN      0x20

/*+----------------------------------------------------------------+
  |	Struct		構造体型宣言
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Class		クラス定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
extern const char* g_pArrLabelFillType[];
extern const char* g_pArrLabelFillOption[];

/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
#endif	/* __FILL_CONST_HPP__ */
