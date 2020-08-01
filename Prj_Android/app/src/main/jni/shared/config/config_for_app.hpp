/*+----------------------------------------------------------------+
  |	Title:		config_for_app.hpp [共通環境]
  |	Comment:	アプリ内容に依存したコンフィグ
  |             [app]以下の内容に依存する項目をまとめてみる（気分の問題）
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#ifndef __CONFIG_FOR_APP_HPP__
#define __CONFIG_FOR_APP_HPP__

/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
//----------------------------------------------------------------
// [for CMemMgr] フィールド割り当て（※１フィールド内で確保単位が指定できる）
//----------------------------------------------------------------
enum eMEM_FIELD{
    // シェア領域（※システム処理等で使いまわされる領域）
    eMEM_FIELD_S_SHARE,       // XX:静的：[eMEM_FIELD_SHARE_***]により細分化される
    
    // 管理領域（※各種マネージャ等から確保されたままにされる領域）
    eMEM_FIELD_D_BASE,        // 動的：for [shared/rank, shared/store, shared/blockchain, shared/touch]
    eMEM_FIELD_D_CSV,         // 動的：for [各種CsvTable]
    eMEM_FIELD_D_DRAW,        // 動的：for [shared/draw/]
    eMEM_FIELD_D_STROKE,      // 動的：for [shared/draw/tex/stroke/]（※解像度調整）
    eMEM_FIELD_D_FILL,        // 動的：for [shared/draw/tex/fill/]（※解像度調整）
    eMEM_FIELD_D_BEZIER,      // 動的：for [shared/draw/tex/bezier/]（※解像度調整）
    eMEM_FIELD_D_TEX,         // 動的：for [shared/draw/tex/]
    eMEM_FIELD_D_LOGIC,       // 動的：for [shared/app/logic/]
    eMEM_FIELD_D_OBJ,         // 動的：for [shared/obj, shared/app/obj/]
    eMEM_FIELD_D_BG,          // 動的：for [shared/app/bg]
    eMEM_FIELD_D_APP,         // 動的：for [shared/loop, shared/app]
    
    // その他
    eMEM_FIELD_D_FREE,        // XX:動的：汎用領域
    
    eMEM_FIELD_MAX,
};

//-----------------------------------
// [for CMemMgr] 動的確保可能領域指定用
//-----------------------------------
#define eMEM_FIELD_D_START      eMEM_FIELD_D_BASE
#define eMEM_FIELD_D_END        eMEM_FIELD_D_FREE

//-------------------------------------------------------------------
// [for CMemMgr] 共有メモリ割り当て（※フィールドの前後で領域が重ねることができる）
//-------------------------------------------------------------------
enum eMEM_FIELD_SHARE{
    eMEM_FIELD_SHARE_FPACK,                 // ファイルパック
    eMEM_FIELD_SHARE_DATA,                  // データバッファ
    eMEM_FIELD_SHARE_SDARC_IN,              // sdarc入力バッファ
    eMEM_FIELD_SHARE_SDARC_OUT,             // sdarc出力バッファ
    eMEM_FIELD_SHARE_SDARC_WORK,            // sdarc作業バッファ
    eMEM_FIELD_SHARE_BMP_GEN_OR_SS,         // BMP生成バッファ／スクリーンショット
    eMEM_FIELD_SHARE_FILL_CELL_OR_SS,       // 塗りつぶしセル／スクリーンショット
    eMEM_FIELD_SHARE_SS_OUT,                // スクリーンショット出力バッファ[RGB]
    eMEM_FIELD_SHARE_SS_WORK,               // スクリーンショット作業バッファ[RGBA]
    
    // 汎用バッファ
    eMEM_FIELD_SHARE_TEMP_STR,              // 一時文字列
    
    eMEM_FIELD_SHARE_MAX,
};

//----------------------------------------------------------------
// [for CMemMgr] 一時文字列領域（4096byteの領域を256個：計1024Kbyte）
//----------------------------------------------------------------
#define MEM_TEMP_STR_BUF_SIZE   4096
#define MEM_TEMP_STR_BUF_NUM     256
#define MEM_TEMP_STR_BUF_FIXED    16
#define MEM_TEMP_STR_SIZE        (MEM_TEMP_STR_BUF_SIZE*MEM_TEMP_STR_BUF_NUM)

//-----------------------------
// [for CDrawer] 描画プロセス
//-----------------------------
enum eDRAW_PROCESS{
    eDRAW_PROCESS_INVALID = -1,    // 無効（描画されない）
    
    // ゲームとして表示するものの描画
    eDRAW_PROCESS_MAIN,
    
    eDRAW_PROCESS_MAX,
};

//-------------------------------------------------------------------
// [for CDrawer] 表示レイヤー（※表示順の値そのまま＆：値が小さいほど手前に表示）
//-------------------------------------------------------------------
enum eDP_LAYER{
    eDP_LAYER_INVALID = -1,          // 無効（非表示）
    
    eDP_LAYER_TOUCH_EFFECT,     // タッチエフェクト
    eDP_LAYER_FADE,             // フェード
    eDP_LAYER_UI,               // UI
    
    eDP_LAYER_BG_F,             // 背景：近景
    eDP_LAYER_GAME,             // ゲーム
    eDP_LAYER_LOOP,             // ループ（※ログ等の表示）
    eDP_LAYER_BG_B,             // 背景：遠景
    
    // [CDrawer.m_nCurDrawPriority]との兼ね合いがあるので「２０」までに収めること
    eDP_LAYER_MAX,
};

//-----------------------------
// [for CLoopMgr] ループID
//-----------------------------
enum eLOOP_ID{
    eLOOP_ID_INVALID = -1,
    
    eLOOP_ID_LOGIC_TEST,
    
    eLOOP_ID_MAX,
};

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

#endif	// __CONFIG_FOR_APP_HPP__
