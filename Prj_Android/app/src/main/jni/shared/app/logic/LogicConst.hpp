/*+----------------------------------------------------------------+
  |	Title:		LogicConst.hpp [共通環境]
  |	Comment:	ロジック定数関連（※ロジックプレイに関わる定数関連）
  |	Author:		K.Takayanagi
  +----------------------------------------------------------------+*/
/*+----------------------------------------------------------------+
  |	Header Define	ヘッダ定義
  +----------------------------------------------------------------+*/
#ifndef __LOGIC_CONST_HPP__
#define __LOGIC_CONST_HPP__

/*+----------------------------------------------------------------+
  |	Include		インクルードヘッダ
  +----------------------------------------------------------------+*/
#include "env.hpp"

/*+----------------------------------------------------------------+
  |	Define		デファイン定義
  +----------------------------------------------------------------+*/
// ロジックの縦横の最大サイズ（※[50x50]）
#define LOGIC_SOLID_WH_MAX          50

// ヒントの最大サイズ（※縦横の最大の半分）
#define LOGIC_CLUE_WH_MAX           ((LOGIC_SOLID_WH_MAX+1)/2)

// ヒント解析ログの数（※１マス削る度に登録される場合もあるので最大マス数）
#define LOGIC_CLUE_LOG_CELL_MAX     LOGIC_SOLID_WH_MAX

// ロジックログの数（※全てのログが十分に収まるように多めに確保しておく＆このログは保存しないので１解析分）
#define LOGIC_LOG_CELL_MAX          (2*LOGIC_SOLID_WH_MAX*LOGIC_SOLID_WH_MAX)

// ロジックイベントの数（※全てのイベントが十分に収まるように多めに確保しておく）
#define LOGIC_EVENT_CELL_MAX        (2*LOGIC_SOLID_WH_MAX*LOGIC_SOLID_WH_MAX)

// スペシャルゲージの基本長
#define LOGIC_SPECIAL_GAUGE_BASE    100

// フラグの基本効果値
#define LOGIC_FLAG_BASE_VAL         100

// スナップショットで保存できる枚数（※全ての経過は保存しない＝パズルの全行程は[CLogicRecord]で保持される）
#define LOGIC_SNAP_SHOT_CELL_MAX    8

// スナップショットファイル拡張子
#define LOGIC_SNAP_SHOT_FILE_EXT    ".ss"

// ロジックの記録で保存する最大セル数（※全ての操作を保存するのに十分な領域を確保する）
#define LOGIC_RECORD_CELL_MAX       (LOGIC_SOLID_WH_MAX*LOGIC_SOLID_WH_MAX)

// 記録ファイル拡張子
#define LOGIC_RECORD_FILE_EXT       ".rec"

// スコアの最大
#define LOGIC_SCORE_MAX             99999999

//------------------
// ロジックのプレイ状況
//------------------
enum eLOGIC_STATUS{
    eLOGIC_STATUS_INVALID = -1,
    
    //-------------------------------------------------------------------------
    // プレイ中（処理中）
    //-------------------------------------------------------------------------
    eLOGIC_STATUS_PLAYING,
    
    //----------------------------------------------------------------------
    // クリアした（※すべてのマス目が整合性を保った状態で埋まった）
    //----------------------------------------------------------------------
    eLOGIC_STATUS_CLEAR,
    
    //----------------------------------------------------------------------
    // 矛盾が発生した（※ヒントに対して不整合なマス目が埋められた）
    //（※仮定時であれば仮定開始時のマスが反転確定される）
    //----------------------------------------------------------------------
    eLOGIC_STATUS_IMCOMPATIBLE,
    
    //----------------------------------------------------------------------
    // 諦めた（※解析処理してもマス目の進展がなくなった：AUTO時）
    //（※AUTO時に進展がない場合は仮定で続行されるため、このステータスで終了することはない）
    //（※仮にこの状態で終了した場合はロジックのデータがおかしい（解けない）可能性がある）
    //----------------------------------------------------------------------
    eLOGIC_STATUS_GIVE_UP,
    
    eLOGIC_STATUS_MAX,
};

#define IS_LOGIC_STATUS_VALID( _s )         ((_s)>=(eLOGIC_STATUS)0 && (_s)<eLOGIC_STATUS_MAX)

//------------------
// マスの種類
//------------------
enum eLOGIC_SOLID_TYPE{
    eLOGIC_SOLID_TYPE_ANY = -3,             // 評価時に利用：「いずれか」扱い
    eLOGIC_SOLID_TYPE_BLANK = -2,           // 評価時に利用：「不明」扱い
    
    eLOGIC_SOLID_TYPE_INVALID = -1,         // 無効（※評価時にも利用：「できなかった」扱い）
    
    eLOGIC_SOLID_TYPE_BLACK,                // 黒マス
    eLOGIC_SOLID_TYPE_WHITE,                // 白マス
    
    eLOGIC_SOLID_TYPE_MAX,
};

#define IS_LOGIC_SOLID_TYPE_VALID( _t )     ((_t)>=(eLOGIC_SOLID_TYPE)0 && (_t)<eLOGIC_SOLID_TYPE_MAX)

//------------------
// マスの状況
//------------------
enum eLOGIC_SOLID_STATUS{
    // 無効（※空欄：マスが埋まっていない＝処理可能）
    eLOGIC_SOLID_STATUS_INVALID = -1,
    
    // 確定状態（※マスが埋まっている＝処理不可）
    eLOGIC_SOLID_STATUS_DETERMINED,         // 確定された
    eLOGIC_SOLID_STATUS_ASSUMED,            // 仮定状況で確定された
    eLOGIC_SOLID_STATUS_ASSUMED_START,      // 仮定の開始点として確定された
    
    eLOGIC_SOLID_STATUS_MAX,
};

#define IS_LOGIC_SOLID_STAUTS_VALID( _s )   ((_s)>=(eLOGIC_SOLID_STATUS)0 && (_s)<eLOGIC_SOLID_STATUS_MAX)

//------------------
// ヒントの状況
//------------------
enum eLOGIC_CLUE_STATUS{
    eLOGIC_CLUE_STATUS_INVALID = -1,        // 無効（※未確定）
    
    eLOGIC_CLUE_STATUS_DETERMINED,          // 確定された
    eLOGIC_CLUE_STATUS_ASSUMED,             // 仮定状況で確定された
    eLOGIC_CLUE_STATUS_IMCOMPATIBLE,        // 矛盾した
    
    eLOGIC_CLUE_STATUS_MAX,
};

#define IS_LOGIC_CLUE_STATUS_VALID( _s )    ((_s)>=(eLOGIC_CLUE_STATUS_TYPE)0 && (_s)<eLOGIC_CLUE_STATUS_MAX)

//------------------
// ラインの状況
//------------------
enum eLOGIC_LINE_STATUS{
    eLOGIC_LINE_STATUS_INVALID = -1,        // 無効（※未確定）
    
    eLOGIC_LINE_STATUS_DETERMINED,          // 確定された
    eLOGIC_LINE_STATUS_ASSUMED,             // 仮定状況で確定された
    eLOGIC_LINE_STATUS_IMCOMPATIBLE,        // 矛盾した
    
    eLOGIC_LINE_STATUS_MAX,
};

#define IS_LOGIC_LINE_STATUS_VALID( _s )    ((_s)>=(eLOGIC_LINE_STATUS_TYPE)0 && (_s)<eLOGIC_LINE_STATUS_MAX)

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
extern const char* g_pArrLabelLogicStatus[];
extern const char* g_pArrLabelLogicSolidType[];
extern const char* g_pArrLabelLogicSolidStatus[];
extern const char* g_pArrLabelLogicClueStatus[];
extern const char* g_pArrLabelLogicLineStatus[];

#endif /* __LOGIC_CONST_HPP__ */
