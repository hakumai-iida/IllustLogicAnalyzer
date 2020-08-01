/*+----------------------------------------------------------------+
  |	Title:		BezierConst.hpp [共通環境]
  |	Comment:	ベジェ曲線関連定数
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#ifndef __BEZIER_CONST_HPP__
#define __BEZIER_CONST_HPP__

/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
// ベジェ関連の計測ログを出すか？
//#define BEZIER_TIME_LOG

//========================================================================
// パレット関連（※塗り画像の色管理＝線の画像はパレットではなく２５５階調のグレースケール）
//========================================================================
// 色のパレットは１本６色の４１本で構成される（※グループ内の値は[０〜５]で指定）
// パレット内部の色の配置は[0：明色]、[2:基本色]、[5:暗色]となる
//========================================================================
// パレットグループ関連（※１本目のオフセットは[0x06]なので[ID_HEAD]は[1]となる）
#define BEZIER_COL_NUM_IN_PAL_GRP                   6
#define BEZIER_PAL_GRP_NUM                          41
#define BEZIER_PAL_GRP_ID_HEAD                      1
#define BEZIER_PAL_GRP_HEAD                         (BEZIER_PAL_GRP_ID_HEAD*BEZIER_COL_NUM_IN_PAL_GRP)
#define BEZIER_PAL_VAL_MIN                          0
#define BEZIER_PAL_VAL_BASE                         2
#define BEZIER_PAL_VAL_MAX                          5

// 変換
#define BEZIER_CONV_PAL_ID_TO_PAL_GRP( _id )        ((BYTE)((_id)*BEZIER_COL_NUM_IN_PAL_GRP))
#define BEZIER_CONV_DOT_TO_PAL_GRP( _d )            ((BYTE)(BEZIER_COL_NUM_IN_PAL_GRP*((_d)/BEZIER_COL_NUM_IN_PAL_GRP)))
#define BEZIER_CONV_DOT_TO_PAL_VAL( _d )            ((BYTE)((_d)%BEZIER_COL_NUM_IN_PAL_GRP))
#define BEZIER_CONV_PAL_INFO_TO_DOT( _pg, _pv )     ((BYTE)((_pg)+(_pv)))

//---------------------------------------------------------------------------
// 塗りつぶしガイド用：[CFill::fillUndercoat]での塗りつぶし判定に利用される値
//（※ストロークに渡された[pBufFillGuide]に対して下塗り色で閉領域を作り塗りつぶすための情報）
//---------------------------------------------------------------------------
// 抜き色
#define BEZIER_PAL_WITHOUT_COL                  (0x00)

// 下塗り色（※この色に囲まれた領域を塗りつぶす）
#define BEZIER_PAL_UNDERCOAT_COL                (0x01)

// 塗りつぶし完了フラグ（※下塗り色で塗りつぶされた画素が処理された）
#define BEZIER_PAL_UNDERCOAT_FILL_DONE_FLAG     (0x02)

// 出力フラグ（※下塗り色の塗りつぶしの際に値が出力された）
#define BEZIER_PAL_UNDERCOAT_FILL_OUT_FLAG      (0x04)

//===========================================================================================
// ワークパス用（※作業表示用設定）
//（※ワークパスは専用パレット[eIMG_PAL_WORK_PATH]で行う：開発用なので色の並びは以下に直書きしておく）
//===========================================================================================
#define BEZIER_WP_LINE_SIZE                 1
#define BEZIER_WP_POINT_SIZE                4
#define BEZIER_WP_SELECTED_SIZE             6
#define BEZIER_WP_TRIANGLE_SIZE             8
#define BEZIER_WP_CROSS_SIZE                5
#define BEZIER_WP_PLUS_SIZE                 6

// 値が大きいほど強い（※手前に描かれる）
#define BEZIER_WP_LINE_PAL                  0x10    // 線色
#define BEZIER_WP_DIR_IN_PAL                0x20    // 方向線色（進入）
#define BEZIER_WP_DIR_OUT_PAL               0x30    // 方向線色（出立）
#define BEZIER_WP_POINT_PAL                 0x40    // アンカーポイント色
#define BEZIER_WP_PAINT_PAL                 0x50    // 塗り色
#define BEZIER_WP_SLOT_PAL                  0x60    // スロット色
#define BEZIER_WP_BASE_PAL                  0x70    // 基準点色

#define BEZIER_WP_PAL_VAL_FOR_SLEEP         0x0     // 休眠色（暗い）
#define BEZIER_WP_PAL_VAL_FOR_ACTIVE        0x1     // 活動色
#define BEZIER_WP_PAL_VAL_FOR_SELECTED      0x2     // 選択色（明るい）

/*+----------------------------------------------------------------+
  |	Struct		構造体型宣言
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
#endif	/* __BEZIER_CONST_HPP__ */
