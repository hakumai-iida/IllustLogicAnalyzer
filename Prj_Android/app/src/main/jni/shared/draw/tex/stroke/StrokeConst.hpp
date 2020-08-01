/*+----------------------------------------------------------------+
  |	Title:		StrokeConst.hpp [共通環境]
  |	Comment:	ストローク定数
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#ifndef __STROKE_CONST_HPP__
#define __STROKE_CONST_HPP__

/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
//----------------------
// ストロークタイプ
//----------------------
enum eSTROKE_TYPE{
    eSTROKE_TYPE_INVALID = -1,  // 無効値
    
    eSTROKE_TYPE_OFF,           // オフ（※線を引かない＝塗りガイドのみ出力したい場合）   
    eSTROKE_TYPE_SQUARE,        // 四角：[CStrokeMgr::GetStrokeDotForSquare]を利用
    eSTROKE_TYPE_CIRCLE,        // 円：[CStrokeMgr::GetStrokeDotForCircle]を利用

    eSTROKE_TYPE_MAX,
};

#define IS_STROKE_TYPE_VALID( _t )  ((_t)>=(eSTROKE_TYPE)0 && (_t)<eSTROKE_TYPE_MAX)

//-------------------------------------------------------------------------------
// フック対象
//-------------------------------------------------------------------------------
// アンカーポイントの実描画座標を、別のアンカーポイントから参照することで、座標を同期させる仕組み
//（※先に描画する側が座標を登録し、後に描画される側が参照するため描画順で座標値が変わる点に注意）
//-------------------------------------------------------------------------------
enum eSTROKE_HOOK_TARGET{
    eSTROKE_HOOK_TARGET_INVALID = -1,

    //-----------------------------------------------------
    // 一時枠（※同一レイヤー内部でフックする場合のテンポラリ）
    // [CLayerData::draw]の呼び出し毎にリセットされる
    //-----------------------------------------------------
    eSTROKE_HOOK_TARGET_TEMP_A,    eSTROKE_HOOK_TARGET_TEMP_B,    eSTROKE_HOOK_TARGET_TEMP_C,    eSTROKE_HOOK_TARGET_TEMP_D,
    eSTROKE_HOOK_TARGET_TEMP_E,    eSTROKE_HOOK_TARGET_TEMP_F,    eSTROKE_HOOK_TARGET_TEMP_G,    eSTROKE_HOOK_TARGET_TEMP_H,
    eSTROKE_HOOK_TARGET_TEMP_I,    eSTROKE_HOOK_TARGET_TEMP_J,    eSTROKE_HOOK_TARGET_TEMP_K,    eSTROKE_HOOK_TARGET_TEMP_L,
    eSTROKE_HOOK_TARGET_TEMP_M,    eSTROKE_HOOK_TARGET_TEMP_N,    eSTROKE_HOOK_TARGET_TEMP_O,    eSTROKE_HOOK_TARGET_TEMP_P,
    eSTROKE_HOOK_TARGET_TEMP_Q,    eSTROKE_HOOK_TARGET_TEMP_R,    eSTROKE_HOOK_TARGET_TEMP_S,    eSTROKE_HOOK_TARGET_TEMP_T,
    eSTROKE_HOOK_TARGET_TEMP_U,    eSTROKE_HOOK_TARGET_TEMP_V,    eSTROKE_HOOK_TARGET_TEMP_W,    eSTROKE_HOOK_TARGET_TEMP_X,
    eSTROKE_HOOK_TARGET_TEMP_Y,    eSTROKE_HOOK_TARGET_TEMP_Z,

    //-----------------------------------------------------
    // 以下固定枠（※スロットをまたいだフックをする場合の消されない枠）
    // [CLayerData::draw]の呼び出し毎にリセットされない
    //-----------------------------------------------------
    
    //-----------------------------------------------------
    // 顔／頭（※パーツ単位での細かな組み替えが行われるので用途を限定）
    //-----------------------------------------------------
    // 首と顔の接続：[NkBase/FcBase]
    eSTROKE_HOOK_TARGET_NECK_TO_FACE_L,         eSTROKE_HOOK_TARGET_NECK_TO_FACE_R,

    // 頭の上部と土台に接続：[HdTop(HdCap)/HdBase]
    eSTROKE_HOOK_TARGET_HEAD_TOP_L,             eSTROKE_HOOK_TARGET_HEAD_TOP_R,

    // 頭の左右と土台に接続：[HdSide/HdBase]（※閉じる必要がない場合は[HdBase]側で無視する）
    eSTROKE_HOOK_TARGET_HEAD_SIDE_L,            eSTROKE_HOOK_TARGET_HEAD_SIDE_R,

    // 輪郭と生え際の接続：[FcHairLine/FcBase]
    eSTROKE_HOOK_TARGET_HAIR_LINE_L,            eSTROKE_HOOK_TARGET_HAIR_LINE_R,

    //-----------------------------------------------------
    // 特殊（※区分をまたぐもの）
    //-----------------------------------------------------
    // 腕から肩（広背筋）：[AmBase/UpBase]
    eSTROKE_HOOK_TARGET_ARM_TO_BACK_L,          eSTROKE_HOOK_TARGET_ARM_TO_BACK_R,
    
    //-----------------------------------------------------
    // 上半身（※パーツ単位での組み替えがないので汎用的な枠でもつ）
    //-----------------------------------------------------
    eSTROKE_HOOK_TARGET_UP_A,    eSTROKE_HOOK_TARGET_UP_B,    eSTROKE_HOOK_TARGET_UP_C,    eSTROKE_HOOK_TARGET_UP_D,
    eSTROKE_HOOK_TARGET_UP_E,    eSTROKE_HOOK_TARGET_UP_F,    eSTROKE_HOOK_TARGET_UP_G,    eSTROKE_HOOK_TARGET_UP_H,
    eSTROKE_HOOK_TARGET_UP_I,    eSTROKE_HOOK_TARGET_UP_J,    eSTROKE_HOOK_TARGET_UP_K,    eSTROKE_HOOK_TARGET_UP_L,
    eSTROKE_HOOK_TARGET_UP_M,    eSTROKE_HOOK_TARGET_UP_N,    eSTROKE_HOOK_TARGET_UP_O,    eSTROKE_HOOK_TARGET_UP_P,
    eSTROKE_HOOK_TARGET_UP_Q,    eSTROKE_HOOK_TARGET_UP_R,    eSTROKE_HOOK_TARGET_UP_S,    eSTROKE_HOOK_TARGET_UP_T,
    eSTROKE_HOOK_TARGET_UP_U,    eSTROKE_HOOK_TARGET_UP_V,    eSTROKE_HOOK_TARGET_UP_W,    eSTROKE_HOOK_TARGET_UP_X,
    eSTROKE_HOOK_TARGET_UP_Y,    eSTROKE_HOOK_TARGET_UP_Z,

    //-----------------------------------------------------
    // 下半身（※パーツ単位での組み替えがないので汎用的な枠でもつ）
    //-----------------------------------------------------
    eSTROKE_HOOK_TARGET_LOW_A,   eSTROKE_HOOK_TARGET_LOW_B,   eSTROKE_HOOK_TARGET_LOW_C,   eSTROKE_HOOK_TARGET_LOW_D,
    eSTROKE_HOOK_TARGET_LOW_E,   eSTROKE_HOOK_TARGET_LOW_F,   eSTROKE_HOOK_TARGET_LOW_G,   eSTROKE_HOOK_TARGET_LOW_H,
    eSTROKE_HOOK_TARGET_LOW_I,   eSTROKE_HOOK_TARGET_LOW_J,   eSTROKE_HOOK_TARGET_LOW_K,   eSTROKE_HOOK_TARGET_LOW_L,
    eSTROKE_HOOK_TARGET_LOW_M,   eSTROKE_HOOK_TARGET_LOW_N,   eSTROKE_HOOK_TARGET_LOW_O,   eSTROKE_HOOK_TARGET_LOW_P,
    eSTROKE_HOOK_TARGET_LOW_Q,   eSTROKE_HOOK_TARGET_LOW_R,   eSTROKE_HOOK_TARGET_LOW_S,   eSTROKE_HOOK_TARGET_LOW_T,
    eSTROKE_HOOK_TARGET_LOW_U,   eSTROKE_HOOK_TARGET_LOW_V,   eSTROKE_HOOK_TARGET_LOW_W,   eSTROKE_HOOK_TARGET_LOW_X,
    eSTROKE_HOOK_TARGET_LOW_Y,   eSTROKE_HOOK_TARGET_LOW_Z,

    //-----------------------------------------------------
    // スーツ（※パーツ単位での組み替えがないので汎用的な枠でもつ）
    //-----------------------------------------------------
    eSTROKE_HOOK_TARGET_SUIT_A,   eSTROKE_HOOK_TARGET_SUIT_B,   eSTROKE_HOOK_TARGET_SUIT_C,   eSTROKE_HOOK_TARGET_SUIT_D,
    eSTROKE_HOOK_TARGET_SUIT_E,   eSTROKE_HOOK_TARGET_SUIT_F,   eSTROKE_HOOK_TARGET_SUIT_G,   eSTROKE_HOOK_TARGET_SUIT_H,
    eSTROKE_HOOK_TARGET_SUIT_I,   eSTROKE_HOOK_TARGET_SUIT_J,   eSTROKE_HOOK_TARGET_SUIT_K,   eSTROKE_HOOK_TARGET_SUIT_L,
    eSTROKE_HOOK_TARGET_SUIT_M,   eSTROKE_HOOK_TARGET_SUIT_N,   eSTROKE_HOOK_TARGET_SUIT_O,   eSTROKE_HOOK_TARGET_SUIT_P,
    eSTROKE_HOOK_TARGET_SUIT_Q,   eSTROKE_HOOK_TARGET_SUIT_R,   eSTROKE_HOOK_TARGET_SUIT_S,   eSTROKE_HOOK_TARGET_SUIT_T,
    eSTROKE_HOOK_TARGET_SUIT_U,   eSTROKE_HOOK_TARGET_SUIT_V,   eSTROKE_HOOK_TARGET_SUIT_W,   eSTROKE_HOOK_TARGET_SUIT_X,
    eSTROKE_HOOK_TARGET_SUIT_Y,   eSTROKE_HOOK_TARGET_SUIT_Z,

    //-----------------------------------------------------------------
    // 腕（※パーツ単位での組み替えがないので汎用的な枠でもつ）
    // 素材作成時は[ARM_*_0]を指定する（※実行時にスロットインデックスで補正される）
    //（※胴体等のスロットからアクセスする場合は[*_0/*_1]により左右の切り替えが可能）
    //-----------------------------------------------------------------
    eSTROKE_HOOK_TARGET_ARM_A_0,  eSTROKE_HOOK_TARGET_ARM_A_1,  eSTROKE_HOOK_TARGET_ARM_B_0,  eSTROKE_HOOK_TARGET_ARM_B_1,
    eSTROKE_HOOK_TARGET_ARM_C_0,  eSTROKE_HOOK_TARGET_ARM_C_1,  eSTROKE_HOOK_TARGET_ARM_D_0,  eSTROKE_HOOK_TARGET_ARM_D_1,
    eSTROKE_HOOK_TARGET_ARM_E_0,  eSTROKE_HOOK_TARGET_ARM_E_1,  eSTROKE_HOOK_TARGET_ARM_F_0,  eSTROKE_HOOK_TARGET_ARM_F_1,
    eSTROKE_HOOK_TARGET_ARM_G_0,  eSTROKE_HOOK_TARGET_ARM_G_1,  eSTROKE_HOOK_TARGET_ARM_H_0,  eSTROKE_HOOK_TARGET_ARM_H_1,
    eSTROKE_HOOK_TARGET_ARM_I_0,  eSTROKE_HOOK_TARGET_ARM_I_1,  eSTROKE_HOOK_TARGET_ARM_J_0,  eSTROKE_HOOK_TARGET_ARM_J_1,
    eSTROKE_HOOK_TARGET_ARM_K_0,  eSTROKE_HOOK_TARGET_ARM_K_1,  eSTROKE_HOOK_TARGET_ARM_L_0,  eSTROKE_HOOK_TARGET_ARM_L_1,
    eSTROKE_HOOK_TARGET_ARM_M_0,  eSTROKE_HOOK_TARGET_ARM_M_1,  eSTROKE_HOOK_TARGET_ARM_N_0,  eSTROKE_HOOK_TARGET_ARM_N_1,
    eSTROKE_HOOK_TARGET_ARM_O_0,  eSTROKE_HOOK_TARGET_ARM_O_1,  eSTROKE_HOOK_TARGET_ARM_P_0,  eSTROKE_HOOK_TARGET_ARM_P_1,
    eSTROKE_HOOK_TARGET_ARM_Q_0,  eSTROKE_HOOK_TARGET_ARM_Q_1,  eSTROKE_HOOK_TARGET_ARM_R_0,  eSTROKE_HOOK_TARGET_ARM_R_1,
    eSTROKE_HOOK_TARGET_ARM_S_0,  eSTROKE_HOOK_TARGET_ARM_S_1,  eSTROKE_HOOK_TARGET_ARM_T_0,  eSTROKE_HOOK_TARGET_ARM_T_1,
    eSTROKE_HOOK_TARGET_ARM_U_0,  eSTROKE_HOOK_TARGET_ARM_U_1,  eSTROKE_HOOK_TARGET_ARM_V_0,  eSTROKE_HOOK_TARGET_ARM_V_1,
    eSTROKE_HOOK_TARGET_ARM_W_0,  eSTROKE_HOOK_TARGET_ARM_W_1,  eSTROKE_HOOK_TARGET_ARM_X_0,  eSTROKE_HOOK_TARGET_ARM_X_1,
    eSTROKE_HOOK_TARGET_ARM_Y_0,  eSTROKE_HOOK_TARGET_ARM_Y_1,  eSTROKE_HOOK_TARGET_ARM_Z_0,  eSTROKE_HOOK_TARGET_ARM_Z_1,

    //-------------------------------------------------------------------
    // 脚（※パーツ単位での組み替えがないので汎用的な枠でもつ）
    // 素材作成時は[LEG_*_0]を指定する（※実行時にスロットインデックスで補正される）
    //（※胴体等のスロットからアクセスする場合は[*_0/*_1]により左右の切り替えが可能）
    //-------------------------------------------------------------------
    eSTROKE_HOOK_TARGET_LEG_A_0,  eSTROKE_HOOK_TARGET_LEG_A_1,  eSTROKE_HOOK_TARGET_LEG_B_0,  eSTROKE_HOOK_TARGET_LEG_B_1,
    eSTROKE_HOOK_TARGET_LEG_C_0,  eSTROKE_HOOK_TARGET_LEG_C_1,  eSTROKE_HOOK_TARGET_LEG_D_0,  eSTROKE_HOOK_TARGET_LEG_D_1,
    eSTROKE_HOOK_TARGET_LEG_E_0,  eSTROKE_HOOK_TARGET_LEG_E_1,  eSTROKE_HOOK_TARGET_LEG_F_0,  eSTROKE_HOOK_TARGET_LEG_F_1,
    eSTROKE_HOOK_TARGET_LEG_G_0,  eSTROKE_HOOK_TARGET_LEG_G_1,  eSTROKE_HOOK_TARGET_LEG_H_0,  eSTROKE_HOOK_TARGET_LEG_H_1,
    eSTROKE_HOOK_TARGET_LEG_I_0,  eSTROKE_HOOK_TARGET_LEG_I_1,  eSTROKE_HOOK_TARGET_LEG_J_0,  eSTROKE_HOOK_TARGET_LEG_J_1,
    eSTROKE_HOOK_TARGET_LEG_K_0,  eSTROKE_HOOK_TARGET_LEG_K_1,  eSTROKE_HOOK_TARGET_LEG_L_0,  eSTROKE_HOOK_TARGET_LEG_L_1,
    eSTROKE_HOOK_TARGET_LEG_M_0,  eSTROKE_HOOK_TARGET_LEG_M_1,  eSTROKE_HOOK_TARGET_LEG_N_0,  eSTROKE_HOOK_TARGET_LEG_N_1,
    eSTROKE_HOOK_TARGET_LEG_O_0,  eSTROKE_HOOK_TARGET_LEG_O_1,  eSTROKE_HOOK_TARGET_LEG_P_0,  eSTROKE_HOOK_TARGET_LEG_P_1,
    eSTROKE_HOOK_TARGET_LEG_Q_0,  eSTROKE_HOOK_TARGET_LEG_Q_1,  eSTROKE_HOOK_TARGET_LEG_R_0,  eSTROKE_HOOK_TARGET_LEG_R_1,
    eSTROKE_HOOK_TARGET_LEG_S_0,  eSTROKE_HOOK_TARGET_LEG_S_1,  eSTROKE_HOOK_TARGET_LEG_T_0,  eSTROKE_HOOK_TARGET_LEG_T_1,
    eSTROKE_HOOK_TARGET_LEG_U_0,  eSTROKE_HOOK_TARGET_LEG_U_1,  eSTROKE_HOOK_TARGET_LEG_V_0,  eSTROKE_HOOK_TARGET_LEG_V_1,
    eSTROKE_HOOK_TARGET_LEG_W_0,  eSTROKE_HOOK_TARGET_LEG_W_1,  eSTROKE_HOOK_TARGET_LEG_X_0,  eSTROKE_HOOK_TARGET_LEG_X_1,
    eSTROKE_HOOK_TARGET_LEG_Y_0,  eSTROKE_HOOK_TARGET_LEG_Y_1,  eSTROKE_HOOK_TARGET_LEG_Z_0,  eSTROKE_HOOK_TARGET_LEG_Z_1,

    eSTROKE_HOOK_TARGET_MAX,
};

#define IS_STROKE_HOOK_TARGET_VALID( _t )  ((_t)>=(eSTROKE_HOOK_TARGET)0 && (_t)<eSTROKE_HOOK_TARGET_MAX)

// テンポラリの開始と終了ラベル
#define eSTROKE_HOOK_TARGET_TEMP_START      eSTROKE_HOOK_TARGET_TEMP_A
#define eSTROKE_HOOK_TARGET_TEMP_END        eSTROKE_HOOK_TARGET_TEMP_Z

// 左右の区別をする範囲：顔＆頭
#define eSTROKE_HOOK_TARGET_CHECK_START_FOR_HEAD    eSTROKE_HOOK_TARGET_NECK_TO_FACE_L
#define eSTROKE_HOOK_TARGET_CHECK_END_FOR_HEAD      eSTROKE_HOOK_TARGET_HAIR_LINE_R

// 左右の区別をする範囲：特殊
#define eSTROKE_HOOK_TARGET_CHECK_START_FOR_ETC     eSTROKE_HOOK_TARGET_ARM_TO_BACK_L
#define eSTROKE_HOOK_TARGET_CHECK_END_FOR_ETC       eSTROKE_HOOK_TARGET_ARM_TO_BACK_R

// 左右の区別をする範囲：腕
#define eSTROKE_HOOK_TARGET_CHECK_START_FOR_ARM     eSTROKE_HOOK_TARGET_ARM_A_0
#define eSTROKE_HOOK_TARGET_CHECK_END_FOR_ARM       eSTROKE_HOOK_TARGET_ARM_Z_1

// 左右の区別をする範囲：脚
#define eSTROKE_HOOK_TARGET_CHECK_START_FOR_LEG     eSTROKE_HOOK_TARGET_LEG_A_0
#define eSTROKE_HOOK_TARGET_CHECK_END_FOR_LEG       eSTROKE_HOOK_TARGET_LEG_Z_1

//----------------------------------------------------------------------
// タッチ対象
//----------------------------------------------------------------------
// ストロークの描画時に各座標を登録しておき、その座標に対してタッチを描画する仕組み
// TODO:処理が固まってきたら部位毎にラベルをつけるべきか？
//----------------------------------------------------------------------
enum eSTROKE_TOUCH_TARGET{
    eSTROKE_TOUCH_TARGET_INVALID = -1,
    
    //------------------------------------------------------
    // テンポラリ枠
    //------------------------------------------------------
    // [CLayerData::draw]の呼び出し毎にリセットされる
    // （※スロットインデックスの影響をうけないので１枠１データ）
    //------------------------------------------------------
    eSTROKE_TOUCH_TARGET_TEMP_A,
    eSTROKE_TOUCH_TARGET_TEMP_B,
    eSTROKE_TOUCH_TARGET_TEMP_C,
    eSTROKE_TOUCH_TARGET_TEMP_D,

    //------------------------------------------------------
    // 固定枠
    //------------------------------------------------------
    // [CLayerData::draw]の呼び出し毎にリセットされない
    // （※スロットインデックスの影響をうけるので１用途に左右の枠をもつ）
    //------------------------------------------------------
    eSTROKE_TOUCH_TARGET_A0,    eSTROKE_TOUCH_TARGET_A1,
    eSTROKE_TOUCH_TARGET_B0,    eSTROKE_TOUCH_TARGET_B1,
    eSTROKE_TOUCH_TARGET_C0,    eSTROKE_TOUCH_TARGET_C1,
    eSTROKE_TOUCH_TARGET_D0,    eSTROKE_TOUCH_TARGET_D1,
    eSTROKE_TOUCH_TARGET_E0,    eSTROKE_TOUCH_TARGET_E1,
    eSTROKE_TOUCH_TARGET_F0,    eSTROKE_TOUCH_TARGET_F1,
    eSTROKE_TOUCH_TARGET_G0,    eSTROKE_TOUCH_TARGET_G1,
    eSTROKE_TOUCH_TARGET_H0,    eSTROKE_TOUCH_TARGET_H1,

    eSTROKE_TOUCH_TARGET_I0,    eSTROKE_TOUCH_TARGET_I1,
    eSTROKE_TOUCH_TARGET_J0,    eSTROKE_TOUCH_TARGET_J1,
    eSTROKE_TOUCH_TARGET_K0,    eSTROKE_TOUCH_TARGET_K1,
    eSTROKE_TOUCH_TARGET_L0,    eSTROKE_TOUCH_TARGET_L1,
    eSTROKE_TOUCH_TARGET_M0,    eSTROKE_TOUCH_TARGET_M1,
    eSTROKE_TOUCH_TARGET_N0,    eSTROKE_TOUCH_TARGET_N1,
    eSTROKE_TOUCH_TARGET_O0,    eSTROKE_TOUCH_TARGET_O1,
    eSTROKE_TOUCH_TARGET_P0,    eSTROKE_TOUCH_TARGET_P1,
    
    eSTROKE_TOUCH_TARGET_MAX,
};

#define IS_STROKE_TOUCH_TARGET_VALID( _t )  ((_t)>=(eSTROKE_TOUCH_TARGET)0 && (_t)<eSTROKE_TOUCH_TARGET_MAX)

// テンポラリの開始と終了ラベル
#define eSTROKE_TOUCH_TARGET_TEMP_START     eSTROKE_TOUCH_TARGET_TEMP_A
#define eSTROKE_TOUCH_TARGET_TEMP_END       eSTROKE_TOUCH_TARGET_TEMP_D

// スロットインデックスに対するオフセット（※１スロットにつき１点を必要とする）
#define STROKE_TOUCH_TARGET_OFS_FOR_SLOT_INDEX  1

//----------------------------------------------------------------------
// フリルID
//----------------------------------------------------------------------
// タッチ表示の際にストロークではなくレイヤーデータ（線と塗り）を出力する際の指定ID
//----------------------------------------------------------------------
enum eSTROKE_FRILL{
    eSTROKE_FRILL_INVALID = -1,

    eSTROKE_FRILL_TEST_CIRCLE,      // テスト：丸
    eSTROKE_FRILL_TEST_TRIANGLE,    // テスト：三角
    eSTROKE_FRILL_TEST_SQUARE,      // テスト：四角

    eSTROKE_FRILL_MAX,
};

#define IS_STROKE_FRILL_VALID( _f )  ((_t)>=(eSTROKE_FRILL)0 && (_f)<eSTROKE_FRILL_MAX)

//-----------------------------------------------------------------
// はみ出し精度：ドットの中心からずらせる階調（※縦横それぞれ４段階の１６精度）
//（※はみ出し具合に応じた値を指定の色（パレットグループ）に足しこむ概念）
//（※あまり精度を高くしても見た目に変化がないようなので４程度で十分と思われる）
//-----------------------------------------------------------------
#define STROKE_DOT_OUT_PRECISION_W      4
#define STROKE_DOT_OUT_PRECISION_H      4
#define STROKE_DOT_OUT_PRECISION_MAX    (STROKE_DOT_OUT_PRECISION_W*STROKE_DOT_OUT_PRECISION_H)

// ストローク画素基本サイズ最大（※論理サイズの最大値）
#define STROKE_DOT_BASE_SIZE_MAX        5

// ストローク画素実サイズ最大（※ピクセル率の適用）
#define STROKE_DOT_SIZE_MAX             (STROKE_DOT_BASE_SIZE_MAX*GM_P_RATE)

// ストローク画素確保サイズ最大（※さらに最大割合[300%]の適用）
#define STROKE_DOT_ALLOC_SIZE_MAX       (STROKE_DOT_SIZE_MAX*3)

// ストローク画素精度（※[1.0]のサイズに対して４階調[0.25刻み]の精度をもつ）
#define STROKE_DOT_SIZE_PRECISION       4

/*+----------------------------------------------------------------+
  |	Struct		構造体型宣言
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Class		クラス定義
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Global		グローバルデータ型定義
  +----------------------------------------------------------------+*/
extern const char* g_pArrLabelStrokeType[];
extern const char* g_pArrLabelStrokeHookTarget[];
extern const char* g_pArrLabelStrokeTouchTarget[];
extern const char* g_pArrLabelStrokeFrill[];

/*+----------------------------------------------------------------+
  |	Prototype	プロトタイプ宣言
  +----------------------------------------------------------------+*/
#endif	/* __STROKE_CONST_HPP__ */
